#include "common/utils/diagnostic.h"

#include "driver/bgiBase/blitCmdsOps.h"
#include "driver/bgiVulkan/blitCmds.h"
#include "driver/bgiVulkan/buffer.h"
#include "driver/bgiVulkan/commandBuffer.h"
#include "driver/bgiVulkan/commandQueue.h"
#include "driver/bgiVulkan/conversions.h"
#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/diagnostic.h"
#include "driver/bgiVulkan/bgi.h"
#include "driver/bgiVulkan/texture.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

BgiVulkanBlitCmds::BgiVulkanBlitCmds(BgiVulkan* bgi)
    : _bgi(bgi)
    , _commandBuffer(nullptr)
{
    // We do not acquire the command buffer here, because the Cmds object may
    // have been created on the main thread, but used on a secondary thread.
    // We need to acquire a command buffer for the thread that is doing the
    // recording so we postpone acquiring cmd buffer until first use of Cmds.
}

BgiVulkanBlitCmds::~BgiVulkanBlitCmds() = default;

void
BgiVulkanBlitCmds::PushDebugGroup(const char* label)
{
    _CreateCommandBuffer();
    BgiVulkanBeginLabel(_bgi->GetPrimaryDevice(), _commandBuffer, label);
}

void
BgiVulkanBlitCmds::PopDebugGroup()
{
    _CreateCommandBuffer();
    BgiVulkanEndLabel(_bgi->GetPrimaryDevice(), _commandBuffer);
}

void
BgiVulkanBlitCmds::CopyTextureGpuToCpu(
    BgiTextureGpuToCpuOp const& copyOp)
{
    _CreateCommandBuffer();

    BgiVulkanTexture* srcTexture =
        static_cast<BgiVulkanTexture*>(copyOp.gpuSourceTexture.Get());

    if (!UTILS_VERIFY(srcTexture && srcTexture->GetImage(),
        "Invalid texture handle")) {
        return;
    }

    if (copyOp.destinationBufferByteSize == 0) {
        UTILS_WARN("The size of the data to copy was zero (aborted)");
        return;
    }

    BgiTextureDesc const& texDesc = srcTexture->GetDescriptor();

    bool isTexArray = texDesc.layerCount>1;
    int depthOffset = isTexArray ? 0 : copyOp.sourceTexelOffset[2];

    VkOffset3D origin;
    origin.x = copyOp.sourceTexelOffset[0];
    origin.y = copyOp.sourceTexelOffset[1];
    origin.z = depthOffset;

    VkExtent3D size;
    size.width = texDesc.dimensions[0] - copyOp.sourceTexelOffset[0];
    size.height = texDesc.dimensions[1] - copyOp.sourceTexelOffset[1];
    size.depth = texDesc.dimensions[2] - depthOffset;

    VkImageSubresourceLayers imageSub;
    imageSub.aspectMask =
        BgiVulkanConversions::GetImageAspectFlag(texDesc.usage);
    imageSub.baseArrayLayer = isTexArray ? copyOp.sourceTexelOffset[2] : 0;
    imageSub.layerCount = 1;
    imageSub.mipLevel = copyOp.mipLevel;

    // See vulkan docs: Copying Data Between Buffers and Images
    VkBufferImageCopy region;
    region.bufferImageHeight = 0; // Buffer is tightly packed, like image
    region.bufferRowLength = 0;   // Buffer is tightly packed, like image
    region.bufferOffset = 0;      // We offset cpuDestinationBuffer. Not here.
    region.imageExtent = size;
    region.imageOffset = origin;
    region.imageSubresource = imageSub;

    // Transition image to TRANSFER_READ
    VkImageLayout oldLayout = srcTexture->GetImageLayout();
    srcTexture->TransitionImageBarrier(
        _commandBuffer,
        srcTexture,
        oldLayout,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, // transition tex to this layout
        BgiVulkanTexture::NO_PENDING_WRITES,  // no pending writes
        VK_ACCESS_TRANSFER_READ_BIT,          // type of access
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,    // producer stage
        VK_PIPELINE_STAGE_TRANSFER_BIT);      // consumer stage

    // Copy gpu texture to gpu staging buffer.
    // We reuse the texture's staging buffer, assuming that any new texel
    // uploads this frame will have been consumed from the staging buffer
    // before any downloads (read backs) overwrite the staging buffer texels.
    uint8_t* src = static_cast<uint8_t*>(srcTexture->GetCPUStagingAddress());
    BgiVulkanBuffer* stagingBuffer = srcTexture->GetStagingBuffer();
    UTILS_VERIFY(src && stagingBuffer);

    vkCmdCopyImageToBuffer(
        _commandBuffer->GetVulkanCommandBuffer(),
        srcTexture->GetImage(),
        srcTexture->GetImageLayout(),
        stagingBuffer->GetVulkanBuffer(),
        1,
        &region);

    // Transition image back to what it was.
    VkAccessFlags access = BgiVulkanTexture::GetDefaultAccessFlags(
        srcTexture->GetDescriptor().usage);

    srcTexture->TransitionImageBarrier(
        _commandBuffer,
        srcTexture,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        oldLayout,                           // transition tex to this layout
        BgiVulkanTexture::NO_PENDING_WRITES, // no pending writes
        access,                              // type of access
        VK_PIPELINE_STAGE_TRANSFER_BIT,      // producer stage
        VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT); // consumer stage

    // Offset into the dst buffer
    char* dst = ((char*) copyOp.cpuDestinationBuffer) +
        copyOp.destinationByteOffset;

    // bytes to copy
    size_t byteSize = copyOp.destinationBufferByteSize;

    // Copy to cpu buffer when cmd buffer has been executed
    _commandBuffer->AddCompletedHandler(
        [dst, src, byteSize]{ memcpy(dst, src, byteSize);}
    );
}

void
BgiVulkanBlitCmds::CopyTextureCpuToGpu(
    BgiTextureCpuToGpuOp const& copyOp)
{
    _CreateCommandBuffer();

    BgiVulkanTexture* dstTexture = static_cast<BgiVulkanTexture*>(
        copyOp.gpuDestinationTexture.Get());
    BgiTextureDesc const& texDesc = dstTexture->GetDescriptor();

    // If we used GetCPUStagingAddress as the cpuSourceBuffer when the copyOp
    // was created, we can skip the memcpy since the src and dst buffer are
    // the same and dst staging buffer already contains the desired data.
    // See also: HgiVulkanTexture::GetCPUStagingAddress.
    if (!dstTexture->IsCPUStagingAddress(copyOp.cpuSourceBuffer)) {

        // We need to memcpy at the mip's location in the staging buffer.
        // It is possible we CopyTextureCpuToGpu a bunch of mips in a row
        // before submitting the cmd buf. So we can't just use the start
        // of the staging buffer each time.
        const std::vector<BgiMipInfo> mipInfos =
            BgiGetMipInfos(
                texDesc.format,
                texDesc.dimensions,
                1 /*HgiTextureCpuToGpuOp does one layer at a time*/);

        if (mipInfos.size() > copyOp.mipLevel) {
            BgiMipInfo const& mipInfo = mipInfos[copyOp.mipLevel];

            uint8_t* dst = static_cast<uint8_t*>(
                dstTexture->GetCPUStagingAddress());
            UTILS_VERIFY(dst && dstTexture->GetStagingBuffer());

            dst += mipInfo.byteOffset;
            const size_t size =
                std::min(copyOp.bufferByteSize, 1 * mipInfo.byteSizePerLayer);
            memcpy(dst, copyOp.cpuSourceBuffer, size);
        }
    }

    // Schedule transfer from staging buffer to device-local texture
    BgiVulkanBuffer* stagingBuffer = dstTexture->GetStagingBuffer();
    if (UTILS_VERIFY(stagingBuffer, "Invalid staging buffer for texture")) {
        dstTexture->CopyBufferToTexture(
            _commandBuffer, 
            dstTexture->GetStagingBuffer(),
            copyOp.destinationTexelOffset,
            copyOp.mipLevel);
    }
}

void BgiVulkanBlitCmds::CopyBufferGpuToGpu(
    BgiBufferGpuToGpuOp const& copyOp)
{
    _CreateCommandBuffer();

    BgiBufferHandle const& srcBufHandle = copyOp.gpuSourceBuffer;
    BgiVulkanBuffer* srcBuffer =
        static_cast<BgiVulkanBuffer*>(srcBufHandle.Get());

    if (!UTILS_VERIFY(srcBuffer && srcBuffer->GetVulkanBuffer(),
        "Invalid source buffer handle")) {
        return;
    }

    BgiBufferHandle const& dstBufHandle = copyOp.gpuDestinationBuffer;
    BgiVulkanBuffer* dstBuffer =
        static_cast<BgiVulkanBuffer*>(dstBufHandle.Get());

    if (!UTILS_VERIFY(dstBuffer && dstBuffer->GetVulkanBuffer(),
        "Invalid destination buffer handle")) {
        return;
    }

    if (copyOp.byteSize == 0) {
        UTILS_WARN("The size of the data to copy was zero (aborted)");
        return;
    }

    // Copy data from staging buffer to destination (gpu) buffer
    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = copyOp.sourceByteOffset;
    copyRegion.dstOffset = copyOp.destinationByteOffset;
    copyRegion.size = copyOp.byteSize;

    vkCmdCopyBuffer(
        _commandBuffer->GetVulkanCommandBuffer(),
        srcBuffer->GetVulkanBuffer(),
        dstBuffer->GetVulkanBuffer(),
        1, // regionCount
        &copyRegion);
}

void BgiVulkanBlitCmds::CopyBufferCpuToGpu(
    BgiBufferCpuToGpuOp const& copyOp)
{
    _CreateCommandBuffer();

    if (copyOp.byteSize == 0 ||
        !copyOp.cpuSourceBuffer ||
        !copyOp.gpuDestinationBuffer)
    {
        return;
    }

    BgiVulkanBuffer* buffer = static_cast<BgiVulkanBuffer*>(
        copyOp.gpuDestinationBuffer.Get());

    // If we used GetCPUStagingAddress as the cpuSourceBuffer when the copyOp
    // was created, we can skip the memcpy since the src and dst buffer are
    // the same and dst staging buffer already contains the desired data.
    // See also: HgiBuffer::GetCPUStagingAddress.
    if (!buffer->IsCPUStagingAddress(copyOp.cpuSourceBuffer) ||
        copyOp.sourceByteOffset != copyOp.destinationByteOffset) {

        uint8_t* dst = static_cast<uint8_t*>(buffer->GetCPUStagingAddress());
        size_t dstOffset = copyOp.destinationByteOffset;

        // Offset into the src buffer
        uint8_t* src = ((uint8_t*) copyOp.cpuSourceBuffer) +
            copyOp.sourceByteOffset;

        // Offset into the dst buffer
        memcpy(dst + dstOffset, src, copyOp.byteSize);
    }

    // Schedule copy data from staging buffer to device-local buffer.
    BgiVulkanBuffer* stagingBuffer = buffer->GetStagingBuffer();

    if (UTILS_VERIFY(stagingBuffer)) {
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = copyOp.sourceByteOffset;
        copyRegion.dstOffset = copyOp.destinationByteOffset;
        copyRegion.size = copyOp.byteSize;

        vkCmdCopyBuffer(
            _commandBuffer->GetVulkanCommandBuffer(), 
            stagingBuffer->GetVulkanBuffer(),
            buffer->GetVulkanBuffer(),
            1, 
            &copyRegion);
    }
}

void
BgiVulkanBlitCmds::CopyBufferGpuToCpu(BgiBufferGpuToCpuOp const& copyOp)
{
    _CreateCommandBuffer();

    if (copyOp.byteSize == 0 ||
        !copyOp.cpuDestinationBuffer ||
        !copyOp.gpuSourceBuffer)
    {
        return;
    }

    BgiVulkanBuffer* buffer = static_cast<BgiVulkanBuffer*>(
        copyOp.gpuSourceBuffer.Get());

    // Make sure there is a staging buffer in the buffer by asking for cpuAddr.
    void* cpuAddress = buffer->GetCPUStagingAddress();
    BgiVulkanBuffer* stagingBuffer = buffer->GetStagingBuffer();
    if (!UTILS_VERIFY(stagingBuffer)) {
        return;
    }

    // Copy from device-local GPU buffer into GPU staging buffer
    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = copyOp.sourceByteOffset;
    copyRegion.dstOffset = copyOp.destinationByteOffset;
    copyRegion.size = copyOp.byteSize;
    vkCmdCopyBuffer(
        _commandBuffer->GetVulkanCommandBuffer(), 
        buffer->GetVulkanBuffer(),
        stagingBuffer->GetVulkanBuffer(),
        1, 
        &copyRegion);

    // Next schedule a callback when the above GPU-GPU copy completes.

    // Offset into the dst buffer
    char* dst = ((char*) copyOp.cpuDestinationBuffer) +
        copyOp.destinationByteOffset;
    
    // Offset into the src buffer
    const char* src = ((const char*) cpuAddress) + copyOp.sourceByteOffset;

    // bytes to copy
    size_t size = copyOp.byteSize;

    // Copy to cpu buffer when cmd buffer has been executed
    _commandBuffer->AddCompletedHandler(
        [dst, src, size]{ memcpy(dst, src, size);}
    );
}

void
BgiVulkanBlitCmds::CopyTextureToBuffer(BgiTextureToBufferOp const& copyOp)
{
    UTILS_CODING_ERROR("Missing Implementation");
}

void
BgiVulkanBlitCmds::CopyBufferToTexture(BgiBufferToTextureOp const& copyOp)
{
    UTILS_CODING_ERROR("Missing Implementation");
}

void
BgiVulkanBlitCmds::GenerateMipMaps(BgiTextureHandle const& texture)
{
    _CreateCommandBuffer();

    BgiVulkanTexture* vkTex = static_cast<BgiVulkanTexture*>(texture.Get());
    BgiVulkanDevice* device = vkTex->GetDevice();

    BgiTextureDesc const& desc = texture->GetDescriptor();

    bool const isDepthBuffer = desc.usage & BgiTextureUsageBitsDepthTarget;
    VkFormat format = BgiVulkanConversions::GetFormat(
        desc.format, isDepthBuffer);
    
    int32_t width = desc.dimensions[0];
    int32_t height = desc.dimensions[1];

    if (desc.layerCount > 1) {
        UTILS_CODING_ERROR("Missing implementation generating mips for layers");
    }

    // Ensure texture format supports blit src&dst required for mips
    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(
        device->GetVulkanPhysicalDevice(), format, &formatProps);
    if (!(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) ||
        !(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT))
    {
        UTILS_CODING_ERROR("Texture format does not support "
                        "blit source and destination");
        return;                    
    }

    VkImageLayout const oldLayout = vkTex->GetImageLayout();

    // Transition first mip to TRANSFER_SRC so we can read it
    BgiVulkanTexture::TransitionImageBarrier(
        _commandBuffer,
        vkTex,
        oldLayout,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        BgiVulkanTexture::NO_PENDING_WRITES,
        VK_ACCESS_TRANSFER_READ_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0);

    // Copy down the whole mip chain doing a blit from mip-1 to mip
    for (uint32_t i = 1; i < desc.mipLevels; i++) {
        VkImageBlit imageBlit{};

        // Source
        imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlit.srcSubresource.layerCount = 1;
        imageBlit.srcSubresource.mipLevel = i - 1;
        imageBlit.srcOffsets[1].x = std::max(width >> (i - 1), 1);
        imageBlit.srcOffsets[1].y = std::max(height >> (i - 1), 1);
        imageBlit.srcOffsets[1].z = 1;

        // Destination
        imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlit.dstSubresource.layerCount = 1;
        imageBlit.dstSubresource.mipLevel = i;
        imageBlit.dstOffsets[1].x = std::max(width >> i, 1);
        imageBlit.dstOffsets[1].y = std::max(height >> i, 1);
        imageBlit.dstOffsets[1].z = 1;

        // Transition current mip level to image blit destination
        BgiVulkanTexture::TransitionImageBarrier(
            _commandBuffer,
            vkTex,
            oldLayout,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            BgiVulkanTexture::NO_PENDING_WRITES,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            i);

        // Blit from previous level
        vkCmdBlitImage(
            _commandBuffer->GetVulkanCommandBuffer(),
            vkTex->GetImage(),
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            vkTex->GetImage(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &imageBlit,
            VK_FILTER_LINEAR);

        // Prepare current mip level as image blit source for next level
        BgiVulkanTexture::TransitionImageBarrier(
            _commandBuffer,
            vkTex,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_ACCESS_TRANSFER_WRITE_BIT,
            VK_ACCESS_TRANSFER_READ_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            i);
    }

    // Return all mips from TRANSFER_SRC to their default (usually SHADER_READ)
    BgiVulkanTexture::TransitionImageBarrier(
        _commandBuffer,
        vkTex,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        BgiVulkanTexture::GetDefaultImageLayout(desc.usage),
        VK_ACCESS_TRANSFER_READ_BIT,
        BgiVulkanTexture::GetDefaultAccessFlags(desc.usage),
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT);
}

void
BgiVulkanBlitCmds::FillBuffer(BgiBufferHandle const& buffer, uint8_t value)
{
    UTILS_CODING_ERROR("Missing Implementation");
}

void
BgiVulkanBlitCmds::InsertMemoryBarrier(BgiMemoryBarrier barrier)
{
    _CreateCommandBuffer();
    _commandBuffer->InsertMemoryBarrier(barrier);
}

BgiVulkanCommandBuffer*
BgiVulkanBlitCmds::GetCommandBuffer()
{
    return _commandBuffer;
}

bool
BgiVulkanBlitCmds::_Submit(Bgi* bgi, BgiSubmitWaitType wait)
{
    if (!_commandBuffer) {
        return false;
    }

    BgiVulkanDevice* device = _commandBuffer->GetDevice();
    BgiVulkanCommandQueue* queue = device->GetCommandQueue();

    // Submit the GPU work and optionally do CPU - GPU synchronization.
    queue->SubmitToQueue(_commandBuffer, wait);

    return true;
}

void
BgiVulkanBlitCmds::_CreateCommandBuffer()
{
    if (!_commandBuffer) {
        BgiVulkanDevice* device = _bgi->GetPrimaryDevice();
        BgiVulkanCommandQueue* queue = device->GetCommandQueue();
        _commandBuffer = queue->AcquireCommandBuffer();
        UTILS_VERIFY(_commandBuffer);
    }
}

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
