#ifndef GUNGNIR_DRIVER_VULKAN_BGI_H
#define GUNGNIR_DRIVER_VULKAN_BGI_H

#include "core/base.h"

#include "driver/bgiBase/bgi.h"
#include "driver/bgiBase/tokens.h"
#include "driver/bgiVulkan/capabilities.h"
#include "driver/bgiVulkan/commandQueue.h"
#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/vulkan.h"

#include <thread>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

class BgiVulkanGarbageCollector;
class BgiVulkanInstance;

/// \class HgiVulkan
///
/// Vulkan implementation of the Hydra Graphics Interface.
///
class BgiVulkan final : public Bgi
{
public:
    BGIVULKAN_API
    BgiVulkan();

    BGIVULKAN_API
    ~BgiVulkan() override;

    BGIVULKAN_API
    bool IsBackendSupported() const override;

    BGIVULKAN_API
    BgiGraphicsCmdsUniquePtr CreateGraphicsCmds(
        BgiGraphicsCmdsDesc const& desc) override;

    BGIVULKAN_API
    BgiBlitCmdsUniquePtr CreateBlitCmds() override;

    BGIVULKAN_API
    BgiComputeCmdsUniquePtr CreateComputeCmds(
        BgiComputeCmdsDesc const& desc) override;

    BGIVULKAN_API
    BgiTextureHandle CreateTexture(BgiTextureDesc const & desc) override;

    BGIVULKAN_API
    void DestroyTexture(BgiTextureHandle* texHandle) override;

    BGIVULKAN_API
    BgiTextureViewHandle CreateTextureView(
        BgiTextureViewDesc const& desc) override;

    BGIVULKAN_API
    void DestroyTextureView(BgiTextureViewHandle* viewHandle) override;

    BGIVULKAN_API
    BgiSamplerHandle CreateSampler(BgiSamplerDesc const & desc) override;

    BGIVULKAN_API
    void DestroySampler(BgiSamplerHandle* smpHandle) override;

    BGIVULKAN_API
    BgiBufferHandle CreateBuffer(BgiBufferDesc const & desc) override;

    BGIVULKAN_API
    void DestroyBuffer(BgiBufferHandle* bufHandle) override;

    BGIVULKAN_API
    BgiShaderFunctionHandle CreateShaderFunction(
        BgiShaderFunctionDesc const& desc) override;

    BGIVULKAN_API
    void DestroyShaderFunction(
        BgiShaderFunctionHandle* shaderFunctionHandle) override;

    BGIVULKAN_API
    BgiShaderProgramHandle CreateShaderProgram(
        BgiShaderProgramDesc const& desc) override;

    BGIVULKAN_API
    void DestroyShaderProgram(
        BgiShaderProgramHandle* shaderProgramHandle) override;

    BGIVULKAN_API
    BgiResourceBindingsHandle CreateResourceBindings(
        BgiResourceBindingsDesc const& desc) override;

    BGIVULKAN_API
    void DestroyResourceBindings(BgiResourceBindingsHandle* resHandle) override;

    BGIVULKAN_API
    BgiGraphicsPipelineHandle CreateGraphicsPipeline(
        BgiGraphicsPipelineDesc const& pipeDesc) override;

    BGIVULKAN_API
    void DestroyGraphicsPipeline(
        BgiGraphicsPipelineHandle* pipeHandle) override;

    BGIVULKAN_API
    BgiComputePipelineHandle CreateComputePipeline(
        BgiComputePipelineDesc const& pipeDesc) override;

    BGIVULKAN_API
    void DestroyComputePipeline(BgiComputePipelineHandle* pipeHandle) override;

    BGIVULKAN_API
    TfToken const& GetAPIName() const override;

    BGIVULKAN_API
    BgiVulkanCapabilities const* GetCapabilities() const override;

    BGIVULKAN_API
    BgiIndirectCommandEncoder* GetIndirectCommandEncoder() const override;

    BGIVULKAN_API
    void StartFrame() override;

    BGIVULKAN_API
    void EndFrame() override;

    //
    // HgiVulkan specific
    //

    /// Returns the Hgi vulkan instance.
    /// Thread safety: Yes.
    BGIVULKAN_API
    BgiVulkanInstance* GetVulkanInstance() const;

    /// Returns the primary (presentation) vulkan device.
    /// Thread safety: Yes.
    BGIVULKAN_API
    BgiVulkanDevice* GetPrimaryDevice() const;

    /// Returns the garbage collector.
    /// Thread safety: Yes.
    BGIVULKAN_API
    BgiVulkanGarbageCollector* GetGarbageCollector() const;

    /// Invalidates the resource handle and places the object in the garbage
    /// collector vector for future destruction.
    /// This is helpful to avoid destroying GPU resources still in-flight.
    template<class T, class H>
    void TrashObject(H* handle, std::vector<T*>* collector)
    {
        T* object = static_cast<T*>(handle->Get());
        if (object) {
            BgiVulkanDevice* device = object->GetDevice();
            BgiVulkanCommandQueue* queue = device->GetCommandQueue();
            object->GetInflightBits() = queue->GetInflightCommandBuffersBits();
            collector->push_back(object);
        }

        *handle = H();
    }

protected:
    BGIVULKAN_API
    bool _SubmitCmds(BgiCmds* cmds, BgiSubmitWaitType wait) override;

private:
    BgiVulkan & operator=(const BgiVulkan&) = delete;
    BgiVulkan(const BgiVulkan&) = delete;

    // Perform low frequency actions, such as garbage collection.
    // Thread safety: No. Must be called from main thread.
    void _EndFrameSync();

    BgiVulkanInstance* _instance;
    BgiVulkanDevice* _device;
    BgiVulkanGarbageCollector* _garbageCollector;
    std::thread::id _threadId;
    int _frameDepth;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_VULKAN_BGI_H
