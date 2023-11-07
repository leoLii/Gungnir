#pragma once

#include "core/base.h"
#include "core/math/math.h"

#include "driver/bgiBase/graphicsCmds.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

#include <cstdint>
#include <functional>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

struct BgiGraphicsCmdsDesc;
class BgiVulkan;
class BgiVulkanCommandBuffer;

using BgiVulkanGfxFunction = std::function<void(void)>;
using BgiVulkanGfxFunctionVector = std::vector<BgiVulkanGfxFunction>;

/// \class HgiVulkanGraphicsCmds
///
/// Vulkan implementation of HgiGraphicsEncoder.
///
class BgiVulkanGraphicsCmds final : public BgiGraphicsCmds
{
public:
    BGIVULKAN_API
    ~BgiVulkanGraphicsCmds() override;

    BGIVULKAN_API
    void PushDebugGroup(const char* label) override;

    BGIVULKAN_API
    void PopDebugGroup() override;

    BGIVULKAN_API
    void SetViewport(Vector4i const& vp) override;

    BGIVULKAN_API
    void SetScissor(Vector4i const& sc) override;

    BGIVULKAN_API
    void BindPipeline(BgiGraphicsPipelineHandle pipeline) override;

    BGIVULKAN_API
    void BindResources(BgiResourceBindingsHandle resources) override;

    BGIVULKAN_API
    void SetConstantValues(
        BgiGraphicsPipelineHandle pipeline,
        BgiShaderStage stages,
        uint32_t bindIndex,
        uint32_t byteSize,
        const void* data) override;

    BGIVULKAN_API
    void BindVertexBuffers(
        BgiVertexBufferBindingVector const &bindings) override;

    BGIVULKAN_API
    void Draw(
        uint32_t vertexCount,
        uint32_t baseVertex,
        uint32_t instanceCount,
        uint32_t baseInstance) override;

    BGIVULKAN_API
    void DrawIndirect(
        BgiBufferHandle const& drawParameterBuffer,
        uint32_t drawBufferByteOffset,
        uint32_t drawCount,
        uint32_t stride) override;

    BGIVULKAN_API
    void DrawIndexed(
        BgiBufferHandle const& indexBuffer,
        uint32_t indexCount,
        uint32_t indexBufferByteOffset,
        uint32_t baseVertex,
        uint32_t instanceCount,
        uint32_t baseInstance) override;

    BGIVULKAN_API
    void DrawIndexedIndirect(
        BgiBufferHandle const& indexBuffer,
        BgiBufferHandle const& drawParameterBuffer,
        uint32_t drawBufferByteOffset,
        uint32_t drawCount,
        uint32_t stride,
        std::vector<uint32_t> const& drawParameterBufferUInt32,
        uint32_t patchBaseVertexByteOffset) override;

    BGIVULKAN_API
    void InsertMemoryBarrier(BgiMemoryBarrier barrier) override;

    /// Returns the command buffer used inside this cmds.
    BGIVULKAN_API
    BgiVulkanCommandBuffer* GetCommandBuffer();

protected:
    friend class BgiVulkan;

    BGIVULKAN_API
    BgiVulkanGraphicsCmds(BgiVulkan* bgi, BgiGraphicsCmdsDesc const& desc);

    BGIVULKAN_API
    bool _Submit(Bgi* bgi, BgiSubmitWaitType wait) override;

private:
    BgiVulkanGraphicsCmds() = delete;
    BgiVulkanGraphicsCmds & operator=(const BgiVulkanGraphicsCmds&) = delete;
    BgiVulkanGraphicsCmds(const BgiVulkanGraphicsCmds&) = delete;

    void _ApplyPendingUpdates();
    void _EndRenderPass();
    void _CreateCommandBuffer();

    BgiVulkan* _bgi;
    BgiGraphicsCmdsDesc _descriptor;
    BgiVulkanCommandBuffer* _commandBuffer;
    BgiGraphicsPipelineHandle _pipeline;
    bool _renderPassStarted;
    bool _viewportSet;
    bool _scissorSet;
    BgiVulkanGfxFunctionVector _pendingUpdates;

    // GraphicsCmds is used only one frame so storing multi-frame state on
    // GraphicsCmds will not survive.
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE
