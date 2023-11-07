#pragma once

#include "core/base.h"
#include "core/math/math.h"

#include "driver/bgiBase/texture.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

class BgiVulkan;
class BgiVulkanBuffer;
class BgiVulkanCommandBuffer;
class BgiVulkanDevice;

/// \class HgiVulkanTexture
///
/// Represents a Vulkan GPU texture resource.
///
class BgiVulkanTexture final : public BgiTexture
{
public:
    static const uint32_t NO_PENDING_WRITES = 0;

    BGIVULKAN_API
    ~BgiVulkanTexture() override;

    BGIVULKAN_API
    size_t GetByteSizeOfResource() const override;

    BGIVULKAN_API
    uint64_t GetRawResource() const override;

    /// Creates (on first use) and returns the CPU staging buffer that can be
    /// used to upload new texture data to the image.
    /// After memcpy-ing new data into the returned address the client
    /// must use BlitCmds CopyTextureCpuToGpu to schedule the transfer
    /// from this staging buffer to the GPU texture.
    BGIVULKAN_API
    void* GetCPUStagingAddress();

    /// Returns true if the provided ptr matches the address of staging buffer.
    BGIVULKAN_API
    bool IsCPUStagingAddress(const void* address) const;

    /// Returns the staging buffer.
    BGIVULKAN_API
    BgiVulkanBuffer* GetStagingBuffer() const;

    /// Returns the image of the texture
    BGIVULKAN_API
    VkImage GetImage() const;

    /// Returns the image view of the texture
    BGIVULKAN_API
    VkImageView GetImageView() const;

    /// Returns the image layout of the texture
    BGIVULKAN_API
    VkImageLayout GetImageLayout() const;

    /// Returns the device used to create this object.
    BGIVULKAN_API
    BgiVulkanDevice* GetDevice() const;

    /// Returns the (writable) inflight bits of when this object was trashed.
    BGIVULKAN_API
    uint64_t & GetInflightBits();

    /// Schedule a copy of texels from the provided buffer into the texture.
    /// If mipLevel is less than one, all mip levels will be copied from buffer.
    BGIVULKAN_API
    void CopyBufferToTexture(
        BgiVulkanCommandBuffer* cb,
        BgiVulkanBuffer* srcBuffer,
        Vector3i const& dstTexelOffset = Vector3i(0),
        int mipLevel=-1);

    /// Transition image from oldLayout to newLayout.
    /// `producerAccess` of 0 means:
    ///    Only invalidation barrier, no flush barrier. For read-only resources.
    ///    Meaning: There are no pending writes.
    ///    Multiple passes can go back to back which all read the resource.
    /// If mipLevel is > -1 only that mips level will be transitioned.
    BGIVULKAN_API
    static void TransitionImageBarrier(
        BgiVulkanCommandBuffer* cb,
        BgiVulkanTexture* tex,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkAccessFlags producerAccess,
        VkAccessFlags consumerAccess,
        VkPipelineStageFlags producerStage,
        VkPipelineStageFlags consumerStage,
        int32_t mipLevel=-1);

    /// Returns the layout for a texture based on its usage flags.
    BGIVULKAN_API
    static VkImageLayout GetDefaultImageLayout(BgiTextureUsage usage);

    /// Returns the access flags for a texture based on its usage flags.
    BGIVULKAN_API
    static VkAccessFlags GetDefaultAccessFlags(BgiTextureUsage usage);

protected:
    friend class BgiVulkan;

    BGIVULKAN_API
    BgiVulkanTexture(
        BgiVulkan* hgi,
        BgiVulkanDevice* device,
        BgiTextureDesc const & desc);

    // Texture view constructor to alias another texture's data.
    BGIVULKAN_API
    BgiVulkanTexture(
        BgiVulkan* hgi,
        BgiVulkanDevice* device,
        BgiTextureViewDesc const & desc);

private:
    BgiVulkanTexture() = delete;
    BgiVulkanTexture & operator=(const BgiVulkanTexture&) = delete;
    BgiVulkanTexture(const BgiVulkanTexture&) = delete;

    bool _isTextureView;
    VkImage _vkImage;
    VkImageView _vkImageView;
    VkImageLayout _vkImageLayout;
    VmaAllocation _vmaImageAllocation;
    BgiVulkanDevice* _device;
    uint64_t _inflightBits;
    BgiVulkanBuffer* _stagingBuffer;
    void* _cpuStagingAddress;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE
