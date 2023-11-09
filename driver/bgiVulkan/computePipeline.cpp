#include "common/utils/diagnostic.h"

#include "driver/bgiVulkan/computePipeline.h"
#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/diagnostic.h"
#include "driver/bgiVulkan/pipelineCache.h"
#include "driver/bgiVulkan/shaderFunction.h"
#include "driver/bgiVulkan/shaderProgram.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

BgiVulkanComputePipeline::BgiVulkanComputePipeline(
    BgiVulkanDevice* device,
    BgiComputePipelineDesc const& desc)
    : BgiComputePipeline(desc)
    , _device(device)
    , _inflightBits(0)
    , _vkPipeline(nullptr)
    , _vkPipelineLayout(nullptr)
{
    VkComputePipelineCreateInfo pipeCreateInfo =
        {VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};

    BgiShaderFunctionHandleVector const& sfv =
        desc.shaderProgram->GetShaderFunctions();

    if (sfv.empty()) {
        UTILS_CODING_ERROR("Missing compute program");
        return;
    }

    BgiVulkanShaderFunction const* s =
        static_cast<BgiVulkanShaderFunction const*>(sfv.front().Get());

    BgiVulkanDescriptorSetInfoVector const& setInfo = s->GetDescriptorSetInfo();

    pipeCreateInfo.stage.sType =
        {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    pipeCreateInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeCreateInfo.stage.module = s->GetShaderModule();
    pipeCreateInfo.stage.pName = s->GetShaderFunctionName();
    pipeCreateInfo.stage.pNext = nullptr;
    pipeCreateInfo.stage.pSpecializationInfo = nullptr;
    pipeCreateInfo.stage.flags = 0;

    //
    // Generate Pipeline layout
    //
    bool usePushConstants = desc.shaderConstantsDesc.byteSize > 0;
    VkPushConstantRange pcRanges;
    if (usePushConstants) {
        UTILS_VERIFY(desc.shaderConstantsDesc.byteSize % 4 == 0,
            "Push constants not multipes of 4");
        pcRanges.offset = 0;
        pcRanges.size = desc.shaderConstantsDesc.byteSize;
        pcRanges.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    VkPipelineLayoutCreateInfo pipeLayCreateInfo =
        {VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipeLayCreateInfo.pushConstantRangeCount = usePushConstants ? 1 : 0;
    pipeLayCreateInfo.pPushConstantRanges = &pcRanges;

    _vkDescriptorSetLayouts = BgiVulkanMakeDescriptorSetLayouts(
        device, {setInfo}, desc.debugName);
    pipeLayCreateInfo.setLayoutCount = (uint32_t)_vkDescriptorSetLayouts.size();
    pipeLayCreateInfo.pSetLayouts = _vkDescriptorSetLayouts.data();

    UTILS_VERIFY(
        vkCreatePipelineLayout(
            _device->GetVulkanDevice(),
            &pipeLayCreateInfo,
            BgiVulkanAllocator(),
            &_vkPipelineLayout) == VK_SUCCESS
    );

    // Debug label
    if (!desc.debugName.empty()) {
        std::string debugLabel = "PipelineLayout " + desc.debugName;
        BgiVulkanSetDebugName(
            device,
            (uint64_t)_vkPipelineLayout,
            VK_OBJECT_TYPE_PIPELINE_LAYOUT,
            debugLabel.c_str());
    }

    pipeCreateInfo.layout = _vkPipelineLayout;

    //
    // Create pipeline
    //
    BgiVulkanPipelineCache* pCache = device->GetPipelineCache();

    UTILS_VERIFY(
        vkCreateComputePipelines(
            _device->GetVulkanDevice(),
            pCache->GetVulkanPipelineCache(),
            1,
            &pipeCreateInfo,
            BgiVulkanAllocator(),
            &_vkPipeline) == VK_SUCCESS
    );

    // Debug label
    if (!desc.debugName.empty()) {
        std::string debugLabel = "Pipeline " + desc.debugName;
        BgiVulkanSetDebugName(
            device,
            (uint64_t)_vkPipeline,
            VK_OBJECT_TYPE_PIPELINE,
            debugLabel.c_str());
    }
}

BgiVulkanComputePipeline::~BgiVulkanComputePipeline()
{
    vkDestroyPipelineLayout(
        _device->GetVulkanDevice(),
        _vkPipelineLayout,
        BgiVulkanAllocator());

    vkDestroyPipeline(
        _device->GetVulkanDevice(),
        _vkPipeline,
        BgiVulkanAllocator());

    for (VkDescriptorSetLayout layout : _vkDescriptorSetLayouts) {
        vkDestroyDescriptorSetLayout(
            _device->GetVulkanDevice(),
            layout,
            BgiVulkanAllocator());
    }
}

void
BgiVulkanComputePipeline::BindPipeline(VkCommandBuffer cb)
{
    vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_COMPUTE, _vkPipeline);
}

VkPipelineLayout
BgiVulkanComputePipeline::GetVulkanPipelineLayout() const
{
    return _vkPipelineLayout;
}

BgiVulkanDevice*
BgiVulkanComputePipeline::GetDevice() const
{
    return _device;
}

uint64_t &
BgiVulkanComputePipeline::GetInflightBits()
{
    return _inflightBits;
}

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
