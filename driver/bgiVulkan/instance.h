#ifndef GUNGNIR_DRIVER_VULKAN_INSTANCE_H
#define GUNGNIR_DRIVER_VULKAN_INSTANCE_H

#include "core/base.h"

#include "driver/bgiVulkan/vulkan.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

/// \class HgiVkInstance
///
/// Initializes the Vulkan library and contains the apps Vulkan state.
///
class BgiVulkanInstance final
{
public:
    BgiVulkanInstance();

    ~BgiVulkanInstance();

    /// Return the vulkan instance
    VkInstance const& GetVulkanInstance() const;

    /// Instance Extension function pointers
    VkDebugUtilsMessengerEXT vkDebugMessenger = 0;
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = 0;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = 0;

private:
    VkInstance _vkInstance;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif
