#ifndef GUNGNIR_DRIVER_VULKAN_DIAGNOSTIC_H
#define GUNGNIR_DRIVER_VULKAN_DIAGNOSTIC_H

#include "core/base.h"

#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

class BgiVulkanCommandBuffer;
class BgiVulkanDevice;
class BgiVulkanInstance;

/// Returns true if debugging is enabled (HGIVULKAN_DEBUG=1)
BGIVULKAN_API
bool BgiVulkanIsDebugEnabled();

/// Setup vulkan debug callbacks
BGIVULKAN_API
void BgiVulkanCreateDebug(BgiVulkanInstance* instance);

/// Tear down vulkan debug callbacks
BGIVULKAN_API
void BgiVulkanDestroyDebug(BgiVulkanInstance* instance);

/// Setup vulkan device debug callbacks
BGIVULKAN_API
void BgiVulkanSetupDeviceDebug(
    BgiVulkanInstance* instance,
    BgiVulkanDevice* device);

/// Add a debug name to a vulkan object
BGIVULKAN_API
void BgiVulkanSetDebugName(
    BgiVulkanDevice* device,
    uint64_t vulkanObject, /*Handle to vulkan object cast to uint64_t*/
    VkObjectType objectType,
    const char* name);

/// Begin a label in a vulkan command buffer
BGIVULKAN_API
void BgiVulkanBeginLabel(
    BgiVulkanDevice* device,
    BgiVulkanCommandBuffer* cb,
    const char* label);

/// End the last pushed label in a vulkan command buffer
BGIVULKAN_API
void BgiVulkanEndLabel(
    BgiVulkanDevice* device,
    BgiVulkanCommandBuffer* cb);

/// Begin a label in the vulkan device gfx queue
BGIVULKAN_API
void BgiVulkanBeginQueueLabel(
    BgiVulkanDevice* device,
    const char* label);

/// End the last pushed label in the vulkan device gfx queue
BGIVULKAN_API
void BgiVulkanEndQueueLabel(BgiVulkanDevice* device);

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_VULKAN_DIAGNOSTIC_H
