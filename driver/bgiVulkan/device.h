#ifndef GUNGNIR_DRIVER_VULKAN_DEVICE_H
#define GUNGNIR_DRIVER_VULKAN_DEVICE_H

#include "core/base.h"

#include "driver/bgiVulkan/vulkan.h"

#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

class BgiVulkanCapabilities;
class BgiVulkanCommandQueue;
class BgiVulkanInstance;
class BgiVulkanPipelineCache;

/// \class HgiVulkanDevice
///
/// Vulkan implementation of GPU device.
///
class BgiVulkanDevice final
{
public:
    BgiVulkanDevice(BgiVulkanInstance* instance);

    ~BgiVulkanDevice();

    /// Returns the vulkan device
    VkDevice GetVulkanDevice() const;

    /// Returns the vulkan memory allocator.
    VmaAllocator GetVulkanMemoryAllocator() const;

    /// Returns the command queue which manages command buffers submission.
    BgiVulkanCommandQueue* GetCommandQueue() const;

    /// Returns the device capablities / features it supports.
    HGIVULKAN_API
    BgiVulkanCapabilities const& GetDeviceCapabilities() const;

    /// Returns the type (or family index) for the graphics queue.
    uint32_t GetGfxQueueFamilyIndex() const;

    /// Returns vulkan physical device
    VkPhysicalDevice GetVulkanPhysicalDevice() const;

    /// Returns the pipeline cache.
    BgiVulkanPipelineCache* GetPipelineCache() const;

    /// Wait for all queued up commands to have been processed on device.
    /// This should ideally never be used as it creates very big stalls, but
    /// is useful for unit testing.
    void WaitForIdle();

    /// Returns true if the provided extension is supported by the device
    bool IsSupportedExtension(const char* extensionName) const;

    /// Device extension function pointers
    PFN_vkCreateRenderPass2KHR vkCreateRenderPass2KHR = 0;
    PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT = 0;
    PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT = 0;
    PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT = 0;
    PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = 0;
    PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT = 0;
    PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT = 0;

private:
    BgiVulkanDevice() = delete;
    BgiVulkanDevice & operator=(const BgiVulkanDevice&) = delete;
    BgiVulkanDevice(const BgiVulkanDevice&) = delete;

    // Vulkan device objects
    VkPhysicalDevice _vkPhysicalDevice;
    VkDevice _vkDevice;
    std::vector<VkExtensionProperties> _vkExtensions;
    VmaAllocator _vmaAllocator;
    uint32_t _vkGfxsQueueFamilyIndex;
    BgiVulkanCommandQueue* _commandQueue;
    BgiVulkanCapabilities* _capabilities;
    BgiVulkanPipelineCache* _pipelineCache;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif
