#include "core/utils/diagnostic.h"

#include "driver/bgiVulkan/shaderFunction.h"
#include "driver/bgiVulkan/conversions.h"
#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/diagnostic.h"
#include "driver/bgiVulkan/garbageCollector.h"
#include "driver/bgiVulkan/bgi.h"
#include "driver/bgiVulkan/shaderCompiler.h"
#include "driver/bgiVulkan/shaderGenerator.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

BgiVulkanShaderFunction::BgiVulkanShaderFunction(
    BgiVulkanDevice* device,
    Bgi const* bgi,
    BgiShaderFunctionDesc const& desc,
    int shaderVersion)
    : BgiShaderFunction(desc)
    , _device(device)
    , _spirvByteSize(0)
    , _vkShaderModule(nullptr)
    , _inflightBits(0)
{
    VkShaderModuleCreateInfo shaderCreateInfo =
        {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};

    std::vector<unsigned int> spirv;

    const char* debugLbl = _descriptor.debugName.empty() ?
        "unknown" : _descriptor.debugName.c_str();

    BgiVulkanShaderGenerator shaderGenerator(bgi, desc);
    shaderGenerator.Execute();
    const char *shaderCode = shaderGenerator.GetGeneratedShaderCode();

    // Compile shader and capture errors
    bool result = BgiVulkanCompileGLSL(
        debugLbl,
        &shaderCode,
        1,
        desc.shaderStage,
        &spirv,
        &_errors);

    // Create vulkan module if there were no errors.
    if (result) {
        _spirvByteSize = spirv.size() * sizeof(unsigned int);

        shaderCreateInfo.codeSize = _spirvByteSize;
        shaderCreateInfo.pCode = (uint32_t*) spirv.data();

        UTILS_VERIFY(
            vkCreateShaderModule(
                device->GetVulkanDevice(),
                &shaderCreateInfo,
                BgiVulkanAllocator(),
                &_vkShaderModule) == VK_SUCCESS
        );

        // Debug label
        if (!_descriptor.debugName.empty()) {
            std::string debugLabel = "ShaderModule " + _descriptor.debugName;
            BgiVulkanSetDebugName(
                device,
                (uint64_t)_vkShaderModule,
                VK_OBJECT_TYPE_SHADER_MODULE,
                debugLabel.c_str());
        }

        // Perform reflection on spirv to create descriptor set info for
        // this module. This will be needed during pipeline creation when
        // we know the shader modules, but not the resource bindings.
        // Hgi does not require resource bindings information to be provided
        // for its HgiPipeline descriptor, but does provide the shader program.
        // We mimic Metal where the resource binding info is inferred from the
        // Metal shader program.
        _descriptorSetInfo = BgiVulkanGatherDescriptorSetInfo(spirv);
    }

    // Clear these pointers in our copy of the descriptor since we
    // have to assume they could become invalid after we return.
    _descriptor.shaderCodeDeclarations = nullptr;
    _descriptor.shaderCode = nullptr;
    _descriptor.generatedShaderCodeOut = nullptr;
}

BgiVulkanShaderFunction::~BgiVulkanShaderFunction()
{
    if (_vkShaderModule) {
        vkDestroyShaderModule(
            _device->GetVulkanDevice(),
            _vkShaderModule,
            BgiVulkanAllocator());
    }
}

VkShaderStageFlagBits
BgiVulkanShaderFunction::GetShaderStage() const
{
    return VkShaderStageFlagBits(
        BgiVulkanConversions::GetShaderStages(_descriptor.shaderStage));
}

VkShaderModule
BgiVulkanShaderFunction::GetShaderModule() const
{
    return _vkShaderModule;
}

const char*
BgiVulkanShaderFunction::GetShaderFunctionName() const
{
    static const std::string entry("main");
    return entry.c_str();
}

bool
BgiVulkanShaderFunction::IsValid() const
{
    return _errors.empty();
}

std::string const&
BgiVulkanShaderFunction::GetCompileErrors()
{
    return _errors;
}

size_t
BgiVulkanShaderFunction::GetByteSizeOfResource() const
{
    return _spirvByteSize;
}

uint64_t
BgiVulkanShaderFunction::GetRawResource() const
{
    return (uint64_t) _vkShaderModule;
}

BgiVulkanDescriptorSetInfoVector const&
BgiVulkanShaderFunction::GetDescriptorSetInfo() const
{
    return _descriptorSetInfo;
}

BgiVulkanDevice*
BgiVulkanShaderFunction::GetDevice() const
{
    return _device;
}

uint64_t &
BgiVulkanShaderFunction::GetInflightBits()
{
    return _inflightBits;
}

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
