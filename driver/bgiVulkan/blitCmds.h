#pragma once

#include "core/base.h"

#include "driver/bgiBase/blitCmds.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

class BgiVulkan;
class BgiVulkanCommandBuffer;

/// \class HgiVulkanBlitCmds
///
/// Vulkan implementation of HgiBlitCmds.
///
class BgiVulkanBlitCmds final : public BgiBlitCmds
{
public:
    BGIVULKAN_API
    ~BgiVulkanBlitCmds() override;

    BGIVULKAN_API
    void PushDebugGroup(const char* label) override;

    BGIVULKAN_API
    void PopDebugGroup() override;

    BGIVULKAN_API
    void CopyTextureGpuToCpu(BgiTextureGpuToCpuOp const& copyOp) override;

    BGIVULKAN_API
    void CopyTextureCpuToGpu(BgiTextureCpuToGpuOp const& copyOp) override;

    BGIVULKAN_API
    void CopyBufferGpuToGpu(BgiBufferGpuToGpuOp const& copyOp) override;

    BGIVULKAN_API
    void CopyBufferCpuToGpu(BgiBufferCpuToGpuOp const& copyOp) override;

    BGIVULKAN_API
    void CopyBufferGpuToCpu(BgiBufferGpuToCpuOp const& copyOp) override;

    BGIVULKAN_API
    void CopyTextureToBuffer(BgiTextureToBufferOp const& copyOp) override;
    
    BGIVULKAN_API
    void CopyBufferToTexture(BgiBufferToTextureOp const& copyOp) override;

    BGIVULKAN_API
    void GenerateMipMaps(BgiTextureHandle const& texture) override;

    BGIVULKAN_API
    void FillBuffer(BgiBufferHandle const& buffer, uint8_t value) override;

    BGIVULKAN_API
    void InsertMemoryBarrier(BgiMemoryBarrier barrier) override;
        
    /// Returns the command buffer used inside this cmds.
    BGIVULKAN_API
    BgiVulkanCommandBuffer* GetCommandBuffer();

protected:
    friend class BgiVulkan;

    BGIVULKAN_API
    BgiVulkanBlitCmds(BgiVulkan* hgi);

    BGIVULKAN_API
    bool _Submit(Bgi* bgi, BgiSubmitWaitType wait) override;

private:
    BgiVulkanBlitCmds & operator=(const BgiVulkanBlitCmds&) = delete;
    BgiVulkanBlitCmds(const BgiVulkanBlitCmds&) = delete;

    void _CreateCommandBuffer();

    BgiVulkan* _bgi;
    BgiVulkanCommandBuffer* _commandBuffer;

    // BlitCmds is used only one frame so storing multi-frame state on BlitCmds
    // will not survive.
};

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
