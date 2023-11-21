#include "common/utils/diagnostic.h"

#include "driver/bgiVulkan/instance.h"
#include "driver/bgiVulkan/diagnostic.h"

#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

BgiVulkanInstance::BgiVulkanInstance()
    : vkDebugMessenger(nullptr)
    , vkCreateDebugUtilsMessengerEXT(nullptr)
    , vkDestroyDebugUtilsMessengerEXT(nullptr)
    , _vkInstance(nullptr)
{
    VkApplicationInfo appInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "GungnirEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Birfost";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    createInfo.pApplicationInfo = &appInfo;

    // Setup instance extensions.
    std::vector<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,

        // Pick platform specific surface extension
        #if defined(VK_USE_PLATFORM_WIN32_KHR)
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        #elif defined(VK_USE_PLATFORM_XLIB_KHR)
            VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
        #elif defined(VK_USE_PLATFORM_MACOS_MVK)
            VK_MVK_MACOS_SURFACE_EXTENSION_NAME,
        #else
            #error Unsupported Platform
        #endif

        // Extensions for interop with OpenGL
        VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
        VK_KHR_EXTERNAL_SEMAPHORE_CAPABILITIES_EXTENSION_NAME,

        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
    };

    // Enable validation layers extension.
    // Requires VK_LAYER_PATH to be set.
    if (BgiVulkanIsDebugEnabled()) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        const char* debugLayers[] = {
            "VK_LAYER_KHRONOS_validation"
        };
        createInfo.ppEnabledLayerNames = debugLayers;
        createInfo.enabledLayerCount = (uint32_t)std::size(debugLayers);
    }

    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledExtensionCount = (uint32_t) extensions.size();

    UTILS_VERIFY(vkCreateInstance(
            &createInfo,
            BgiVulkanAllocator(),
            &_vkInstance) == VK_SUCCESS
    );

    BgiVulkanCreateDebug(this);
}

BgiVulkanInstance::~BgiVulkanInstance()
{
    BgiVulkanDestroyDebug(this);
    vkDestroyInstance(_vkInstance, BgiVulkanAllocator());
}

VkInstance const&
BgiVulkanInstance::GetVulkanInstance() const
{
    return _vkInstance;
}

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
