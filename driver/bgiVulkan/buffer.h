#ifndef GUNGNIR_DRIVER_VULKAN_BUFFER_H
#define GUNGNIR_DRIVER_VULKAN_BUFFER_H

#include "core/base.h"

#include "driver/bgiBase/buffer.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkan.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

class BgiVulkan;
class BgiVulkanCommandBuffer;
class BgiVulkanDevice;

///
/// \class HgiVulkanBuffer
///
/// Vulkan implementation of HgiBuffer
///
class BgiVulkanBuffer final : public BgiBuffer
{
public:
    BGIVULKAN_API
    ~BgiVulkanBuffer() override;

    BGIVULKAN_API
    size_t GetByteSizeOfResource() const override;

    BGIVULKAN_API
    uint64_t GetRawResource() const override;

    BGIVULKAN_API
    void* GetCPUStagingAddress() override;

    /// Returns true if the provided ptr matches the address of staging buffer.
    BGIVULKAN_API
    bool IsCPUStagingAddress(const void* address) const;

    /// Returns the vulkan buffer.
    BGIVULKAN_API
    VkBuffer GetVulkanBuffer() const;

    /// Returns the memory allocation
    BGIVULKAN_API
    VmaAllocation GetVulkanMemoryAllocation() const;

    /// Returns the staging buffer.
    BGIVULKAN_API
    BgiVulkanBuffer* GetStagingBuffer() const;

    /// Returns the device used to create this object.
    BGIVULKAN_API
    BgiVulkanDevice* GetDevice() const;

    /// Returns the (writable) inflight bits of when this object was trashed.
    BGIVULKAN_API
    uint64_t & GetInflightBits();

    /// Creates a staging buffer.
    /// The caller is responsible for the lifetime (destruction) of the buffer.
    BGIVULKAN_API
    static BgiVulkanBuffer* CreateStagingBuffer(
        BgiVulkanDevice* device,
        BgiBufferDesc const& desc);

protected:
    friend class BgiVulkan;

    // Constructor for making buffers
    BGIVULKAN_API
    BgiVulkanBuffer(
        BgiVulkan* bgi,
        BgiVulkanDevice* device,
        BgiBufferDesc const& desc);

    // Constructor for making staging buffers
    BGIVULKAN_API
    BgiVulkanBuffer(
        BgiVulkanDevice* device,
        VkBuffer vkBuffer,
        VmaAllocation vmaAllocation,
        BgiBufferDesc const& desc);

private:
    BgiVulkanBuffer() = delete;
    BgiVulkanBuffer & operator=(const BgiVulkanBuffer&) = delete;
    BgiVulkanBuffer(const BgiVulkanBuffer&) = delete;

    BgiVulkanDevice* _device;
    VkBuffer _vkBuffer;
    VmaAllocation _vmaAllocation;
    uint64_t _inflightBits;
    BgiVulkanBuffer* _stagingBuffer;
    void* _cpuStagingAddress;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_VULKAN_BUFFER_H
