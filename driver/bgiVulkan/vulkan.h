#ifndef GUNGNIR_DRIVER_VULKAN_VULKAN_H
#define GUNGNIR_DRIVER_VULKAN_VULKAN_H

#include "defines.h"

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"
// Use the default allocator (nullptr)
inline VkAllocationCallbacks*
VulkanAllocator() {
    return nullptr;
}

#endif
