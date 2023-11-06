#include "core/utils/diagnostic.h"

#include "driver/bgiBase/graphicsCmdsDesc.h"
#include "driver/bgiVulkan/graphicsCmds.h"
#include "driver/bgiVulkan/buffer.h"
#include "driver/bgiVulkan/commandBuffer.h"
#include "driver/bgiVulkan/commandQueue.h"
#include "driver/bgiVulkan/conversions.h"
#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/diagnostic.h"
#include "driver/bgiVulkan/bgi.h"
#include "driver/bgiVulkan/graphicsPipeline.h"
#include "driver/bgiVulkan/resourceBindings.h"
#include "driver/bgiVulkan/texture.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiVulkanGraphicsCmds::BgiVulkanGraphicsCmds(
    BgiVulkan* bgi,
    BgiGraphicsCmdsDesc const& desc)
    : _bgi(bgi)
    , _descriptor(desc)
    , _commandBuffer(nullptr)
    , _renderPassStarted(false)
    , _viewportSet(false)
    , _scissorSet(false)
{
    // We do not acquire the command buffer here, because the Cmds object may
    // have been created on the main thread, but used on a secondary thread.
    // We need to acquire a command buffer for the thread that is doing the
    // recording so we postpone acquiring cmd buffer until first use of Cmds.
}

BgiVulkanGraphicsCmds::~BgiVulkanGraphicsCmds()
{
}

void
BgiVulkanGraphicsCmds::PushDebugGroup(const char* label)
{
    _CreateCommandBuffer();
    BgiVulkanBeginLabel(_bgi->GetPrimaryDevice(), _commandBuffer, label);
}

void
BgiVulkanGraphicsCmds::PopDebugGroup()
{
    _CreateCommandBuffer();
    BgiVulkanEndLabel(_bgi->GetPrimaryDevice(), _commandBuffer);
}

void
BgiVulkanGraphicsCmds::SetViewport(Vector4i const& vp)
{
    _viewportSet = true;

    // Delay until the pipeline is set and the render pass has begun.
    _pendingUpdates.push_back(
        [this, vp] {
            float offsetX = (float) vp[0];
            float offsetY = (float) vp[1];
            float width = (float) vp[2];
            float height = (float) vp[3];

            // Flip viewport in Y-axis, because the vertex.y position is flipped
            // between opengl and vulkan. This also moves origin to bottom-left.
            // Requires VK_KHR_maintenance1 extension.

            // Alternatives are:
            // 1. Multiply projection by 'inverted Y and half Z' matrix:
            //    const GfMatrix4d clip(
            //        1.0,  0.0, 0.0, 0.0,
            //        0.0, -1.0, 0.0, 0.0,
            //        0.0,  0.0, 0.5, 0.0,
            //        0.0,  0.0, 0.5, 1.0);
            //    projection = clip * projection;
            //
            // 2. Adjust vertex position:
            //    gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;

            VkViewport viewport;
            viewport.x = offsetX;
            viewport.y = offsetY + height;
            viewport.width = width;
            viewport.height = -height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            vkCmdSetViewport(
                _commandBuffer->GetVulkanCommandBuffer(),
                0,
                1,
                &viewport);
    });
}

void
BgiVulkanGraphicsCmds::SetScissor(Vector4i const& sc)
{
    _scissorSet = true;

    // Delay until the pipeline is set and the render pass has begun.
    _pendingUpdates.push_back(
        [this, sc] {
            uint32_t w(sc[2]);
            uint32_t h(sc[3]);
            VkRect2D scissor = {{sc[0], sc[1]}, {w, h}};
            vkCmdSetScissor(
                _commandBuffer->GetVulkanCommandBuffer(),
                0,
                1,
                &scissor);
    });
}

void
BgiVulkanGraphicsCmds::BindPipeline(BgiGraphicsPipelineHandle pipeline)
{
    _CreateCommandBuffer();

    // End the previous render pass in case we are using the same
    // GfxCmds with multiple pipelines.
    _EndRenderPass();

    _pipeline = pipeline;
    BgiVulkanGraphicsPipeline* pso = 
        static_cast<BgiVulkanGraphicsPipeline*>(_pipeline.Get());

    if (UTILS_VERIFY(pso)) {
        pso->BindPipeline(_commandBuffer->GetVulkanCommandBuffer());
    }
}

void
BgiVulkanGraphicsCmds::BindResources(BgiResourceBindingsHandle res)
{
    // Delay until the pipeline is set and the render pass has begun.
    _pendingUpdates.push_back(
        [this, res] {
            BgiVulkanGraphicsPipeline* pso = 
                static_cast<BgiVulkanGraphicsPipeline*>(_pipeline.Get());

            BgiVulkanResourceBindings * rb =
                static_cast<BgiVulkanResourceBindings*>(res.Get());

            if (pso && rb){
                rb->BindResources(
                    _commandBuffer->GetVulkanCommandBuffer(),
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pso->GetVulkanPipelineLayout());
            }
        }
    );
}

void
BgiVulkanGraphicsCmds::SetConstantValues(
    BgiGraphicsPipelineHandle pipeline,
    BgiShaderStage stages,
    uint32_t bindIndex,
    uint32_t byteSize,
    const void* data)
{
    // The data provided could be local stack memory that goes out of scope
    // before we execute this pending fn. Make a copy to prevent that.
    uint8_t* dataCopy = new uint8_t[byteSize];
    memcpy(dataCopy, data, byteSize);

    // Delay until the pipeline is set and the render pass has begun.
    _pendingUpdates.push_back(
        [this, byteSize, dataCopy, stages] {
            BgiVulkanGraphicsPipeline* pso = 
                static_cast<BgiVulkanGraphicsPipeline*>(_pipeline.Get());

            if (pso) {
                vkCmdPushConstants(
                    _commandBuffer->GetVulkanCommandBuffer(),
                    pso->GetVulkanPipelineLayout(),
                    BgiVulkanConversions::GetShaderStages(stages),
                    0, // offset
                    byteSize,
                    dataCopy);
            }

        delete[] dataCopy;
    });
}

void
BgiVulkanGraphicsCmds::BindVertexBuffers(
    BgiVertexBufferBindingVector const &bindings)
{
    // Delay until the pipeline is set and the render pass has begun.
    _pendingUpdates.push_back(
        [this, bindings] {
        std::vector<VkBuffer> buffers;
        std::vector<VkDeviceSize> bufferOffsets;

        for (BgiVertexBufferBinding const &binding : bindings) {
            BgiVulkanBuffer* buf =
                static_cast<BgiVulkanBuffer*>(binding.buffer.Get());
            VkBuffer vkBuf = buf->GetVulkanBuffer();
            if (vkBuf) {
                buffers.push_back(vkBuf);
                bufferOffsets.push_back(binding.byteOffset);
            }
        }

        vkCmdBindVertexBuffers(
            _commandBuffer->GetVulkanCommandBuffer(),
            bindings[0].index, // first binding
            buffers.size(),
            buffers.data(),
            bufferOffsets.data());
    });
}

void
BgiVulkanGraphicsCmds::Draw(
    uint32_t vertexCount,
    uint32_t baseVertex,
    uint32_t instanceCount,
    uint32_t baseInstance)
{
    // Make sure the render pass has begun and resource are bound
    _ApplyPendingUpdates();

    vkCmdDraw(
        _commandBuffer->GetVulkanCommandBuffer(),
        vertexCount,
        instanceCount,
        baseVertex,
        baseInstance);
}

void
BgiVulkanGraphicsCmds::DrawIndirect(
    BgiBufferHandle const& drawParameterBuffer,
    uint32_t drawBufferByteOffset,
    uint32_t drawCount,
    uint32_t stride)
{
    // Make sure the render pass has begun and resource are bound
    _ApplyPendingUpdates();

    BgiVulkanBuffer* drawBuf =
        static_cast<BgiVulkanBuffer*>(drawParameterBuffer.Get());

    vkCmdDrawIndirect(
        _commandBuffer->GetVulkanCommandBuffer(),
        drawBuf->GetVulkanBuffer(),
        drawBufferByteOffset,
        drawCount,
        stride);
}

void
BgiVulkanGraphicsCmds::DrawIndexed(
    BgiBufferHandle const& indexBuffer,
    uint32_t indexCount,
    uint32_t indexBufferByteOffset,
    uint32_t baseVertex,
    uint32_t instanceCount,
    uint32_t baseInstance)
{
    // Make sure the render pass has begun and resource are bound
    _ApplyPendingUpdates();

    BgiVulkanBuffer* ibo = static_cast<BgiVulkanBuffer*>(indexBuffer.Get());

    vkCmdBindIndexBuffer(
        _commandBuffer->GetVulkanCommandBuffer(),
        ibo->GetVulkanBuffer(),
        indexBufferByteOffset,
        VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(
        _commandBuffer->GetVulkanCommandBuffer(),
        indexCount,
        instanceCount,
        static_cast<uint32_t>(indexBufferByteOffset / sizeof(uint32_t)),
        baseVertex,
        baseInstance);
}

void
BgiVulkanGraphicsCmds::DrawIndexedIndirect(
    BgiBufferHandle const& indexBuffer,
    BgiBufferHandle const& drawParameterBuffer,
    uint32_t drawBufferByteOffset,
    uint32_t drawCount,
    uint32_t stride,
    std::vector<uint32_t> const& /*drawParameterBufferUInt32*/,
    uint32_t /*patchBaseVertexByteOffset*/)
{
    // Make sure the render pass has begun and resource are bound
    _ApplyPendingUpdates();

    BgiVulkanBuffer* ibo = static_cast<BgiVulkanBuffer*>(indexBuffer.Get());

    vkCmdBindIndexBuffer(
        _commandBuffer->GetVulkanCommandBuffer(),
        ibo->GetVulkanBuffer(),
        0, // indexBufferByteOffset
        VK_INDEX_TYPE_UINT32);

    BgiVulkanBuffer* drawBuf =
        static_cast<BgiVulkanBuffer*>(drawParameterBuffer.Get());

    vkCmdDrawIndexedIndirect(
        _commandBuffer->GetVulkanCommandBuffer(),
        drawBuf->GetVulkanBuffer(),
        drawBufferByteOffset,
        drawCount,
        stride);

}

void
BgiVulkanGraphicsCmds::InsertMemoryBarrier(BgiMemoryBarrier barrier)
{
    _CreateCommandBuffer();
    _commandBuffer->InsertMemoryBarrier(barrier);
}

BgiVulkanCommandBuffer*
BgiVulkanGraphicsCmds::GetCommandBuffer()
{
    return _commandBuffer;
}

bool
BgiVulkanGraphicsCmds::_Submit(Bgi* bgi, BgiSubmitWaitType wait)
{
    if (!_commandBuffer) {
        return false;
    }

    // End render pass
    _EndRenderPass();

    BgiVulkanDevice* device = _commandBuffer->GetDevice();
    BgiVulkanCommandQueue* queue = device->GetCommandQueue();

    // Submit the GPU work and optionally do CPU - GPU synchronization.
    queue->SubmitToQueue(_commandBuffer, wait);

    return true;
}

void
BgiVulkanGraphicsCmds::_ApplyPendingUpdates()
{
    if (!_pipeline) {
        UTILS_CODING_ERROR("No pipeline bound");
        return;
    }

    // Ensure the cmd buf is created on the thread that does the recording.
    _CreateCommandBuffer();

    // Begin render pass
    if (!_renderPassStarted && !_pendingUpdates.empty()) {
        _renderPassStarted = true;

        BgiVulkanGraphicsPipeline* pso = 
            static_cast<BgiVulkanGraphicsPipeline*>(_pipeline.Get());

        Vector2i size(0);
        VkClearValueVector const& clearValues = pso->GetClearValues();

        VkRenderPassBeginInfo beginInfo =
            {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
        beginInfo.renderPass = pso->GetVulkanRenderPass();
        beginInfo.framebuffer= pso->AcquireVulkanFramebuffer(_descriptor,&size);
        beginInfo.renderArea.extent.width = size[0];
        beginInfo.renderArea.extent.height = size[1];
        beginInfo.clearValueCount = (uint32_t) clearValues.size();
        beginInfo.pClearValues = clearValues.data();

        VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE;

        vkCmdBeginRenderPass(
            _commandBuffer->GetVulkanCommandBuffer(),
            &beginInfo,
            contents);

        // Make sure viewport and scissor are set since our HgiVulkanPipeline
        // hardcodes one dynamic viewport and scissor.
        if (!_viewportSet) {
            SetViewport(Vector4i(0, 0, size[0], size[1]));
        }
        if (!_scissorSet) {
            SetScissor(Vector4i(0, 0, size[0], size[1]));
        }
    }

    // Now that the render pass has begun we can execute any cmds that
    // require a render pass to be active.
    for (BgiVulkanGfxFunction const& fn : _pendingUpdates) {
        fn();
    }
    _pendingUpdates.clear();
}

void
BgiVulkanGraphicsCmds::_EndRenderPass()
{
    if (_renderPassStarted) {
        vkCmdEndRenderPass(_commandBuffer->GetVulkanCommandBuffer());
        _renderPassStarted = false;
        _viewportSet = false;
        _scissorSet = false;
    }
}

void
BgiVulkanGraphicsCmds::_CreateCommandBuffer()
{
    if (!_commandBuffer) {
        BgiVulkanDevice* device = _bgi->GetPrimaryDevice();
        BgiVulkanCommandQueue* queue = device->GetCommandQueue();
        _commandBuffer = queue->AcquireCommandBuffer();
        UTILS_VERIFY(_commandBuffer);
    }
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE