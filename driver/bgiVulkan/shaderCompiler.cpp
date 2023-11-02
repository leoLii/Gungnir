#include "core/utils/diagnostic.h"

#include "driver/bgiVulkan/shaderCompiler.h"
#include "driver/bgiVulkan/conversions.h"
#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/diagnostic.h"
#include "driver/bgiVulkan/spirv_reflect.h"

#include <shaderc/shaderc.hpp>

#include <unordered_map>

GUNGNIR_NAMESPACE_OPEN_SCOPE

static shaderc_shader_kind
_GetShaderStage(BgiShaderStage stage)
{
    switch(stage) {
        case BgiShaderStageVertex:
            return shaderc_glsl_vertex_shader;
        case BgiShaderStageTessellationControl:
            return shaderc_glsl_tess_control_shader;
        case BgiShaderStageTessellationEval:
            return shaderc_glsl_tess_evaluation_shader;
        case BgiShaderStageGeometry:
            return shaderc_glsl_geometry_shader;
        case BgiShaderStageFragment:
            return shaderc_glsl_fragment_shader;
        case BgiShaderStageCompute:
            return shaderc_glsl_compute_shader;
    }

    TF_CODING_ERROR("Unknown stage");
    return shaderc_glsl_infer_from_source;
}

bool
BgiVulkanCompileGLSL(
    const char* name,
    const char* shaderCodes[],
    uint8_t numShaderCodes,
    BgiShaderStage stage,
    std::vector<unsigned int>* spirvOUT,
    std::string* errors)
{
    if (numShaderCodes==0 || !spirvOUT) {
        if (errors) {
            errors->append("No shader to compile %s", name);
        }
        return false;
    }

    std::string source;
    for (uint8_t i=0; i<numShaderCodes; ++i) {
        source += shaderCodes[i];
    }

    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_vulkan,
                                 shaderc_env_version_vulkan_1_0);
    options.SetTargetSpirv(shaderc_spirv_version_1_0);

    shaderc_shader_kind const kind = _GetShaderStage(stage);

    shaderc::Compiler compiler;
    shaderc::SpvCompilationResult result =
        compiler.CompileGlslToSpv(source, kind, name, options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        *errors = result.GetErrorMessage();
        return false;
    }

    spirvOUT->assign(result.cbegin(), result.cend());

    return true;
}

static bool
_VerifyResults(SpvReflectShaderModule* module, SpvReflectResult const& result)
{
    if (!UTILS_VERIFY(result == SPV_REFLECT_RESULT_SUCCESS)) {
        spvReflectDestroyShaderModule(module);
        return false;
    }

    return true;
}

static VkDescriptorSetLayout
_CreateDescriptorSetLayout(
    BgiVulkanDevice* device,
    VkDescriptorSetLayoutCreateInfo const& createInfo,
    std::string const& debugName)
{
    VkDescriptorSetLayout layout = nullptr;
    UTILS_VERIFY(
        vkCreateDescriptorSetLayout(
            device->GetVulkanDevice(),
            &createInfo,
            BgiVulkanAllocator(),
            &layout) == VK_SUCCESS
    );

    // Debug label
    if (!debugName.empty()) {
        std::string debugLabel = "DescriptorSetLayout " + debugName;
        BgiVulkanSetDebugName(
            device,
            (uint64_t)layout,
            VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
            debugLabel.c_str());
    }

    return layout;
}

BgiVulkanDescriptorSetInfoVector
BgiVulkanGatherDescriptorSetInfo(
    std::vector<unsigned int> const& spirv)
{
    // This code is based on main_descriptors.cpp in the SPIRV-Reflect repo.

    SpvReflectShaderModule module = {};
    SpvReflectResult result = spvReflectCreateShaderModule(
        spirv.size()*sizeof(uint32_t), spirv.data(), &module);
    if (!_VerifyResults(&module, result)) {
        return BgiVulkanDescriptorSetInfoVector();
    }

    uint32_t count = 0;
    result = spvReflectEnumerateDescriptorSets(&module, &count, NULL);
    if (!_VerifyResults(&module, result)) {
        return BgiVulkanDescriptorSetInfoVector();
    }

    std::vector<SpvReflectDescriptorSet*> sets(count);
    result = spvReflectEnumerateDescriptorSets(&module, &count, sets.data());
    if (!_VerifyResults(&module, result)) {
        return BgiVulkanDescriptorSetInfoVector();
    }

    // Generate all necessary data structures to create a VkDescriptorSetLayout
    // for each descriptor set in this shader.
    std::vector<BgiVulkanDescriptorSetInfo> infos(sets.size());

    for (size_t s = 0; s < sets.size(); s++) {
        SpvReflectDescriptorSet const& reflSet = *(sets[s]);
        BgiVulkanDescriptorSetInfo& info = infos[s];
        info.bindings.resize(reflSet.binding_count);

        for (uint32_t b = 0; b < reflSet.binding_count; b++) {
            SpvReflectDescriptorBinding const& reflBinding =
                *(reflSet.bindings[b]);

            VkDescriptorSetLayoutBinding& layoutBinding = info.bindings[b];
            layoutBinding.binding = reflBinding.binding;
            layoutBinding.descriptorType =
                static_cast<VkDescriptorType>(reflBinding.descriptor_type);
            layoutBinding.descriptorCount = 1;

            for (uint32_t d = 0; d < reflBinding.array.dims_count; d++) {
                layoutBinding.descriptorCount *= reflBinding.array.dims[d];
            }
            layoutBinding.stageFlags =
                static_cast<VkShaderStageFlagBits>(module.shader_stage);
        }

        info.setNumber = reflSet.set;
    }

    spvReflectDestroyShaderModule(&module);

    return infos;
}

static bool
_IsDescriptorTextureType(VkDescriptorType descType) {
    return (descType == VK_DESCRIPTOR_TYPE_SAMPLER ||
            descType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
            descType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE ||
            descType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
}

VkDescriptorSetLayoutVector
BgiVulkanMakeDescriptorSetLayouts(
    BgiVulkanDevice* device,
    std::vector<BgiVulkanDescriptorSetInfoVector> const& infos,
    std::string const& debugName)
{
    std::unordered_map<uint32_t, BgiVulkanDescriptorSetInfo> mergedInfos;

    // Merge the binding info of each of the infos such that the resource
    // bindings information for each of the shader stage modules is merged
    // together. For example a vertex shader may have different buffers and
    // textures bound than a fragment shader. We merge them all together to
    // create the descriptor set layout for that shader program.

    for (BgiVulkanDescriptorSetInfoVector const& infoVec : infos) {
        for (BgiVulkanDescriptorSetInfo const& info : infoVec) {

            // Get the set (or create one)
            BgiVulkanDescriptorSetInfo& trg = mergedInfos[info.setNumber];

            for (VkDescriptorSetLayoutBinding const& bi : info.bindings) {

                // If two shader modules have the same binding information for
                // a specific resource, we only want to insert it once.
                // For example both the vertex shaders and fragment shader may
                // have a texture bound at the same binding index.

                VkDescriptorSetLayoutBinding* dst = nullptr;
                for (VkDescriptorSetLayoutBinding& bind : trg.bindings) {
                    if (bind.binding == bi.binding) {
                        dst = &bind;
                        break;
                    }
                }

                // It is a new binding we haven't seen before. Add it
                if (!dst) {
                    trg.setNumber = info.setNumber;
                    trg.bindings.push_back(bi);
                    dst = &trg.bindings.back();
                }

                // These need to match the shader stages used when creating the
                // VkDescriptorSetLayout in HgiVulkanResourceBindings.
                if (dst->stageFlags != BgiVulkanConversions::GetShaderStages(
                    BgiShaderStageCompute)) {
                    
                    if (_IsDescriptorTextureType(dst->descriptorType)) {
                        dst->stageFlags = 
                            BgiVulkanConversions::GetShaderStages(
                                BgiShaderStageGeometry |
                                BgiShaderStageFragment);
                    } else {
                        dst->stageFlags = 
                            BgiVulkanConversions::GetShaderStages(
                                BgiShaderStageVertex |
                                BgiShaderStageTessellationControl |
                                BgiShaderStageTessellationEval |
                                BgiShaderStageGeometry | 
                                BgiShaderStageFragment);
                    }
                }
            }
        }
    }

    // Generate the VkDescriptorSetLayoutCreateInfos for the bindings we merged.
    for (auto& pair : mergedInfos) {
        BgiVulkanDescriptorSetInfo& info = pair.second;
        info.createInfo.sType =
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.createInfo.bindingCount = info.bindings.size();
        info.createInfo.pBindings = info.bindings.data();
    }

    // Create VkDescriptorSetLayouts out of the merge infos above.
    VkDescriptorSetLayoutVector layouts;

    for (auto const& pair : mergedInfos) {
        BgiVulkanDescriptorSetInfo const& info = pair.second;
        VkDescriptorSetLayout layout = _CreateDescriptorSetLayout(
            device, info.createInfo, debugName);
        layouts.push_back(layout);
    }

    return layouts;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
