#include "common/utils/diagnostic.h"

#include "driver/bgiVulkan/diagnostic.h"
#include "driver/bgiVulkan/commandBuffer.h"
#include "driver/bgiVulkan/commandQueue.h"
#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/instance.h"

#include <cstring>

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

// "Enable debugging for HgiVulkan"
#define VULKAN_DEBUG 1

// "Enable verbose debugging for HgiVulkan"
#define VULKAN_DEBUG_VERBOSE 0

bool
BgiVulkanIsDebugEnabled()
{
    static bool _v = VULKAN_DEBUG == 1;
    return _v;
}

bool
BgiVulkanIsVerboseDebugEnabled()
{
    static bool _v = VULKAN_DEBUG_VERBOSE == 1;
    return _v;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
_VulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT msgType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData)
{
    const char* type =
        (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) ?
            "VULKAN_ERROR" : "VULKAN_MESSAGE";

    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        UTILS_CODING_ERROR("%s: %s\n", type, callbackData->pMessage);
    } else {
        UTILS_WARN("%s: %s\n", type, callbackData->pMessage);
    }

    return VK_FALSE;
}

void
BgiVulkanCreateDebug(BgiVulkanInstance* instance)
{
    if (!BgiVulkanIsDebugEnabled()) {
        return;
    }

    VkInstance vkInstance = instance->GetVulkanInstance();

    instance->vkCreateDebugUtilsMessengerEXT =
        (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            vkInstance,
            "vkCreateDebugUtilsMessengerEXT");

    instance->vkDestroyDebugUtilsMessengerEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            vkInstance,
            "vkDestroyDebugUtilsMessengerEXT");

    if (!UTILS_VERIFY(instance->vkCreateDebugUtilsMessengerEXT)) {
        return;
    }
    if (!UTILS_VERIFY(instance->vkDestroyDebugUtilsMessengerEXT)) {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT dbgMsgCreateInfo =
        {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    dbgMsgCreateInfo.pNext = nullptr;
    dbgMsgCreateInfo.flags = 0;
    dbgMsgCreateInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    // Verbose debugging will cause many bits of information to be printed by
    // the vulkan validation layers. It is only useful for debugging.
    if (BgiVulkanIsVerboseDebugEnabled()) {
        dbgMsgCreateInfo.messageSeverity |=
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    }

    dbgMsgCreateInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    dbgMsgCreateInfo.pfnUserCallback = _VulkanDebugCallback;
    dbgMsgCreateInfo.pUserData = nullptr;

    UTILS_VERIFY(
        instance->vkCreateDebugUtilsMessengerEXT(
            vkInstance,
            &dbgMsgCreateInfo,
            BgiVulkanAllocator(),
            &instance->vkDebugMessenger) == VK_SUCCESS
    );
}

void
BgiVulkanDestroyDebug(BgiVulkanInstance* instance)
{
    if (!BgiVulkanIsDebugEnabled()) {
        return;
    }

    VkInstance vkInstance = instance->GetVulkanInstance();

    if (!UTILS_VERIFY(instance->vkDestroyDebugUtilsMessengerEXT)) {
        return;
    }

    instance->vkDestroyDebugUtilsMessengerEXT(
        vkInstance, instance->vkDebugMessenger, BgiVulkanAllocator());
}

void
BgiVulkanSetupDeviceDebug(
    BgiVulkanInstance* instance,
    BgiVulkanDevice* device)
{
    VkInstance vkInstance = instance->GetVulkanInstance();
    device->vkCmdBeginDebugUtilsLabelEXT =
        (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(
        vkInstance,
        "vkCmdBeginDebugUtilsLabelEXT");

    device->vkCmdEndDebugUtilsLabelEXT =
        (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(
        vkInstance,
        "vkCmdEndDebugUtilsLabelEXT");

    device->vkCmdInsertDebugUtilsLabelEXT =
        (PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(
        vkInstance,
        "vkCmdInsertDebugUtilsLabelEXT");

    device->vkSetDebugUtilsObjectNameEXT =
        (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(
        vkInstance,
        "vkSetDebugUtilsObjectNameEXT");

    device->vkQueueBeginDebugUtilsLabelEXT =
        (PFN_vkQueueBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(
        vkInstance,
        "vkQueueBeginDebugUtilsLabelEXT");

    device->vkQueueEndDebugUtilsLabelEXT =
        (PFN_vkQueueEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(
        vkInstance,
        "vkQueueEndDebugUtilsLabelEXT");
}

void
BgiVulkanSetDebugName(
    BgiVulkanDevice* device,
    uint64_t vulkanObject,
    VkObjectType objectType,
    const char* name)
{
    if (!BgiVulkanIsDebugEnabled() || !name) {
        return;
    }

    if (!UTILS_VERIFY(device && device->vkSetDebugUtilsObjectNameEXT)) {
        return;
    }

    VkDebugUtilsObjectNameInfoEXT debugInfo =
        {VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
    debugInfo.objectHandle = vulkanObject;
    debugInfo.objectType = objectType;
    debugInfo.pObjectName = name;
    device->vkSetDebugUtilsObjectNameEXT(device->GetVulkanDevice(), &debugInfo);
}


void
BgiVulkanBeginLabel(
    BgiVulkanDevice* device,
    BgiVulkanCommandBuffer* cb,
    const char* label)
{
    if (!BgiVulkanIsDebugEnabled() || !label) {
        return;
    }

    VkCommandBuffer vkCmbuf = cb->GetVulkanCommandBuffer();
    VkDebugUtilsLabelEXT labelInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT};
    labelInfo.pLabelName = label;
    device->vkCmdBeginDebugUtilsLabelEXT(vkCmbuf, &labelInfo);
}

void
BgiVulkanEndLabel(
    BgiVulkanDevice* device,
    BgiVulkanCommandBuffer* cb)
{
    if (!BgiVulkanIsDebugEnabled()) {
        return;
    }

    VkCommandBuffer vkCmbuf = cb->GetVulkanCommandBuffer();
    device->vkCmdEndDebugUtilsLabelEXT(vkCmbuf);
}

void
BgiVulkanBeginQueueLabel(
    BgiVulkanDevice* device,
    const char* label)
{
    if (!BgiVulkanIsDebugEnabled() || !label) {
        return;
    }

    VkQueue gfxQueue = device->GetCommandQueue()->GetVulkanGraphicsQueue();
    VkDebugUtilsLabelEXT labelInfo = {VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT};
    labelInfo.pLabelName = label;
    device->vkQueueBeginDebugUtilsLabelEXT(gfxQueue, &labelInfo);
}

void
BgiVulkanEndQueueLabel(BgiVulkanDevice* device)
{
    if (!BgiVulkanIsDebugEnabled()) {
        return;
    }

    VkQueue gfxQueue = device->GetCommandQueue()->GetVulkanGraphicsQueue();
    device->vkQueueEndDebugUtilsLabelEXT(gfxQueue);
}

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
