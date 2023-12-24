#pragma once

#include "common/arch/defines.h"

// Define the platform for Vulkan so vulkan.h below picks the correct includes.
#if defined(ARCH_OS_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(ARCH_OS_LINUX)
#define VK_USE_PLATFORM_XLIB_KHR
#elif defined(ARCH_OS_OSX)
#define VK_USE_PLATFORM_MACOS_MVK
#else
#error Unsupported Platform
#endif

#include <vulkan/vulkan.h>

#define VMA_VULKAN_VERSION 1003000 
#include <vk_mem_alloc.h>

// Use the default allocator (nullptr)
inline VkAllocationCallbacks*
BgiVulkanAllocator() {
    return nullptr;
}
