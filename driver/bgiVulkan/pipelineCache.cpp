#include "driver/bgiVulkan/pipelineCache.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiVulkanPipelineCache::BgiVulkanPipelineCache(
    BgiVulkanDevice* device)
    : _device(device)
    , _vkPipelineCache(nullptr)
{
    // xxx we need to add a pipeline cache to avoid app having to keep compiling
    // shader micro-code for every pipeline combination. We except that the
    // spir-V shader code is not compiled for the target device until this point
    // where we create the pipeline. So a pipeline cache can be helpful.
}

BgiVulkanPipelineCache::~BgiVulkanPipelineCache()
{
}

VkPipelineCache
BgiVulkanPipelineCache::GetVulkanPipelineCache() const
{
    return _vkPipelineCache;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
