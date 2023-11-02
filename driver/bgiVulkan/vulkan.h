#ifndef GUNGNIR_DRIVER_VULKAN_VULKAN_H
#define GUNGNIR_DRIVER_VULKAN_VULKAN_H

#include "core/base.h"

#include <vulkan/vulkan.h>

#include "driver/bgiVulkan/vk_mem_alloc.h"

// Use the default allocator (nullptr)
inline VkAllocationCallbacks*
BgiVulkanAllocator() {
    return nullptr;
}

#endif
