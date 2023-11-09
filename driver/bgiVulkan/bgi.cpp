#include "common/utils/diagnostic.h"

#include "driver/bgiVulkan/bgi.h"
#include "driver/bgiVulkan/blitCmds.h"
#include "driver/bgiVulkan/buffer.h"
#include "driver/bgiVulkan/capabilities.h"
#include "driver/bgiVulkan/commandQueue.h"
#include "driver/bgiVulkan/computeCmds.h"
#include "driver/bgiVulkan/computePipeline.h"
#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/diagnostic.h"
#include "driver/bgiVulkan/garbageCollector.h"
#include "driver/bgiVulkan/graphicsCmds.h"
#include "driver/bgiVulkan/graphicsPipeline.h"
#include "driver/bgiVulkan/instance.h"
#include "driver/bgiVulkan/resourceBindings.h"
#include "driver/bgiVulkan/sampler.h"
#include "driver/bgiVulkan/shaderFunction.h"
#include "driver/bgiVulkan/shaderProgram.h"
#include "driver/bgiVulkan/texture.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

BgiVulkan::BgiVulkan()
    : _instance(new BgiVulkanInstance())
    , _device(new BgiVulkanDevice(_instance))
    , _garbageCollector(new BgiVulkanGarbageCollector(this))
    , _threadId(std::this_thread::get_id())
    , _frameDepth(0)
{
}

BgiVulkan::~BgiVulkan()
{
    BgiVulkanCommandQueue* queue = _device->GetCommandQueue();

    // Wait for command buffers to complete, then reset command buffers for 
    // each device's queue.
    queue->ResetConsumedCommandBuffers(BgiSubmitWaitTypeWaitUntilCompleted);

    // Wait for all devices and perform final garbage collection.
    _device->WaitForIdle();
    _garbageCollector->PerformGarbageCollection(_device);
    delete _garbageCollector;
    delete _device;
    delete _instance;
}

bool
BgiVulkan::IsBackendSupported() const
{
    // Want Vulkan 1.2 or higher.
    const uint32_t apiVersion = GetCapabilities()->GetAPIVersion();
    const uint32_t majorVersion = VK_VERSION_MAJOR(apiVersion);
    const uint32_t minorVersion = VK_VERSION_MINOR(apiVersion);

    return (majorVersion >= 1) && (minorVersion >= 2);
}

/* Multi threaded */
BgiGraphicsCmdsUniquePtr
BgiVulkan::CreateGraphicsCmds(
    BgiGraphicsCmdsDesc const& desc)
{
    BgiVulkanGraphicsCmds* cmds(new BgiVulkanGraphicsCmds(this, desc));
    return BgiGraphicsCmdsUniquePtr(cmds);
}

/* Multi threaded */
BgiBlitCmdsUniquePtr
BgiVulkan::CreateBlitCmds()
{
    return BgiBlitCmdsUniquePtr(new BgiVulkanBlitCmds(this));
}

BgiComputeCmdsUniquePtr
BgiVulkan::CreateComputeCmds(
    BgiComputeCmdsDesc const& desc)
{
    BgiVulkanComputeCmds* cmds(new BgiVulkanComputeCmds(this, desc));
    return BgiComputeCmdsUniquePtr(cmds);
}

/* Multi threaded */
BgiTextureHandle
BgiVulkan::CreateTexture(BgiTextureDesc const & desc)
{
    return BgiTextureHandle(
        new BgiVulkanTexture(this, GetPrimaryDevice(), desc),
        GetUniqueId());
}

/* Multi threaded */
void
BgiVulkan::DestroyTexture(BgiTextureHandle* texHandle)
{
    TrashObject(texHandle, GetGarbageCollector()->GetTextureList());
}

/* Multi threaded */
BgiTextureViewHandle
BgiVulkan::CreateTextureView(BgiTextureViewDesc const & desc)
{
    if (!desc.sourceTexture) {
        UTILS_CODING_ERROR("Source texture is null");
    }

    BgiTextureHandle src = BgiTextureHandle(
        new BgiVulkanTexture(this, GetPrimaryDevice(),desc), GetUniqueId());
    BgiTextureView* view = new BgiTextureView(desc);
    view->SetViewTexture(src);
    return BgiTextureViewHandle(view, GetUniqueId());
}

void
BgiVulkan::DestroyTextureView(BgiTextureViewHandle* viewHandle)
{
    // Trash the texture inside the view and invalidate the view handle.
    BgiTextureHandle texHandle = (*viewHandle)->GetViewTexture();
    TrashObject(&texHandle, GetGarbageCollector()->GetTextureList());
    (*viewHandle)->SetViewTexture(BgiTextureHandle());
    delete viewHandle->Get();
    *viewHandle = BgiTextureViewHandle();
}

/* Multi threaded */
BgiSamplerHandle
BgiVulkan::CreateSampler(BgiSamplerDesc const & desc)
{
    return BgiSamplerHandle(
        new BgiVulkanSampler(GetPrimaryDevice(), desc),
        GetUniqueId());
}

/* Multi threaded */
void
BgiVulkan::DestroySampler(BgiSamplerHandle* smpHandle)
{
    TrashObject(smpHandle, GetGarbageCollector()->GetSamplerList());
}

/* Multi threaded */
BgiBufferHandle
BgiVulkan::CreateBuffer(BgiBufferDesc const & desc)
{
    return BgiBufferHandle(
        new BgiVulkanBuffer(this, GetPrimaryDevice(), desc),
        GetUniqueId());
}

/* Multi threaded */
void
BgiVulkan::DestroyBuffer(BgiBufferHandle* bufHandle)
{
    TrashObject(bufHandle, GetGarbageCollector()->GetBufferList());
}

/* Multi threaded */
BgiShaderFunctionHandle
BgiVulkan::CreateShaderFunction(BgiShaderFunctionDesc const& desc)
{
    return BgiShaderFunctionHandle(
        new BgiVulkanShaderFunction(GetPrimaryDevice(), this, desc,
        GetCapabilities()->GetShaderVersion()), GetUniqueId());
}

/* Multi threaded */
void
BgiVulkan::DestroyShaderFunction(BgiShaderFunctionHandle* shaderFnHandle)
{
    TrashObject(shaderFnHandle, GetGarbageCollector()->GetShaderFunctionList());
}

/* Multi threaded */
BgiShaderProgramHandle
BgiVulkan::CreateShaderProgram(BgiShaderProgramDesc const& desc)
{
    return BgiShaderProgramHandle(
        new BgiVulkanShaderProgram(GetPrimaryDevice(), desc),
        GetUniqueId());
}

/* Multi threaded */
void
BgiVulkan::DestroyShaderProgram(BgiShaderProgramHandle* shaderPrgHandle)
{
    TrashObject(shaderPrgHandle, GetGarbageCollector()->GetShaderProgramList());
}

/* Multi threaded */
BgiResourceBindingsHandle
BgiVulkan::CreateResourceBindings(BgiResourceBindingsDesc const& desc)
{
    return BgiResourceBindingsHandle(
        new BgiVulkanResourceBindings(GetPrimaryDevice(), desc),
        GetUniqueId());
}

/* Multi threaded */
void
BgiVulkan::DestroyResourceBindings(BgiResourceBindingsHandle* resHandle)
{
    TrashObject(resHandle, GetGarbageCollector()->GetResourceBindingsList());
}

BgiGraphicsPipelineHandle
BgiVulkan::CreateGraphicsPipeline(BgiGraphicsPipelineDesc const& desc)
{
    return BgiGraphicsPipelineHandle(
        new BgiVulkanGraphicsPipeline(GetPrimaryDevice(), desc),
        GetUniqueId());
}

void
BgiVulkan::DestroyGraphicsPipeline(BgiGraphicsPipelineHandle* pipeHandle)
{
    TrashObject(pipeHandle, GetGarbageCollector()->GetGraphicsPipelineList());
}

BgiComputePipelineHandle
BgiVulkan::CreateComputePipeline(BgiComputePipelineDesc const& desc)
{
    return BgiComputePipelineHandle(
        new BgiVulkanComputePipeline(GetPrimaryDevice(), desc),
        GetUniqueId());
}

void
BgiVulkan::DestroyComputePipeline(BgiComputePipelineHandle* pipeHandle)
{
    TrashObject(pipeHandle, GetGarbageCollector()->GetComputePipelineList());
}

/* Multi threaded */
tokens::DRIVER const&
BgiVulkan::GetAPIName() const {
    return tokens::DRIVER::Vulkan;
}

/* Multi threaded */
BgiVulkanCapabilities const*
BgiVulkan::GetCapabilities() const
{
    return &_device->GetDeviceCapabilities();
}


BgiIndirectCommandEncoder*
BgiVulkan::GetIndirectCommandEncoder() const
{
    return nullptr;
}

/* Single threaded */
void
BgiVulkan::StartFrame()
{
    // Please read important usage limitations for Hgi::StartFrame

    if (_frameDepth++ == 0) {
        BgiVulkanBeginQueueLabel(GetPrimaryDevice(), "Full Hydra Frame");
    }
}

/* Single threaded */
void
BgiVulkan::EndFrame()
{
    // Please read important usage limitations for Hgi::EndFrame

    if (--_frameDepth == 0) {
        _EndFrameSync();
        BgiVulkanEndQueueLabel(GetPrimaryDevice());
    }
}

/* Multi threaded */
BgiVulkanInstance*
BgiVulkan::GetVulkanInstance() const
{
    return _instance;
}

/* Multi threaded */
BgiVulkanDevice*
BgiVulkan::GetPrimaryDevice() const
{
    return _device;
}

/* Multi threaded */
BgiVulkanGarbageCollector*
BgiVulkan::GetGarbageCollector() const
{
    return _garbageCollector;
}

/* Single threaded */
bool
BgiVulkan::_SubmitCmds(BgiCmds* cmds, BgiSubmitWaitType wait)
{
    // XXX The device queue is externally synchronized so we would at minimum
    // need a mutex here to ensure only one thread submits cmds at a time.
    // However, since we currently call garbage collection here and because
    // we only have one resource command buffer, we cannot support submitting
    // cmds from secondary threads until those issues are resolved.
    if (_threadId != std::this_thread::get_id()) {
        UTILS_CODING_ERROR("Secondary threads should not submit cmds");
        return false;
    }

    // Submit Cmds work
    bool result = false;
    if (cmds) {
        result = Bgi::_SubmitCmds(cmds, wait);
    }

    // XXX If client does not call StartFrame / EndFrame we perform end of frame
    // cleanup after each SubmitCmds. This is more frequent than ideal and also
    // prevents us from making SubmitCmds thread-safe.
    if (_frameDepth==0) {
        _EndFrameSync();
    }

    return result;
}

/* Single threaded */
void
BgiVulkan::_EndFrameSync()
{
    // The garbage collector and command buffer reset must happen on the
    // main-thread when no threads are recording.
    if (_threadId != std::this_thread::get_id()) {
        UTILS_CODING_ERROR("Secondary thread violation");
        return;
    }

    BgiVulkanDevice* device = GetPrimaryDevice();
    BgiVulkanCommandQueue* queue = device->GetCommandQueue();

    // Reset command buffers for each device's queue.
    queue->ResetConsumedCommandBuffers();

    // Perform garbage collection for each device.
    _garbageCollector->PerformGarbageCollection(device);
}

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
