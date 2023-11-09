#pragma once

#include "common/base.h"

#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

class BgiVulkanDevice;

/// \class HgiVulkanPipelineCache
///
/// Wrapper for Vulkan pipeline cache.
///
class BgiVulkanPipelineCache final
{
public:
    BGIVULKAN_API
    BgiVulkanPipelineCache(BgiVulkanDevice* device);

    BGIVULKAN_API
    ~BgiVulkanPipelineCache();

    /// Returns the vulkan pipeline cache.
    BGIVULKAN_API
    VkPipelineCache GetVulkanPipelineCache() const;

private:
    BgiVulkanPipelineCache() = delete;
    BgiVulkanPipelineCache & operator=(const BgiVulkanPipelineCache&) = delete;
    BgiVulkanPipelineCache(const BgiVulkanPipelineCache&) = delete;

    BgiVulkanDevice* _device;
    VkPipelineCache _vkPipelineCache;
};

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
