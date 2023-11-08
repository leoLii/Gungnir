#pragma once

#include "core/base.h"

#include "driver/bgiBase/bgi.h"
#include "driver/bgiVulkan/api.h"

#include <mutex>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

class BgiVulkan;
class BgiVulkanDevice;

using BgiVulkanBufferVector =
    std::vector<class BgiVulkanBuffer*>;
using BgiVulkanTextureVector =
    std::vector<class BgiVulkanTexture*>;
using BgiVulkanSamplerVector =
    std::vector<class BgiVulkanSampler*>;
using BgiVulkanShaderFunctionVector =
    std::vector<class BgiVulkanShaderFunction*>;
using BgiVulkanShaderProgramVector =
    std::vector<class BgiVulkanShaderProgram*>;
using BgiVulkanResourceBindingsVector =
    std::vector<class BgiVulkanResourceBindings*>;
using BgiVulkanGraphicsPipelineVector =
    std::vector<class BgiVulkanGraphicsPipeline*>;
using BgiVulkanComputePipelineVector =
    std::vector<class BgiVulkanComputePipeline*>;


/// \class HgiVulkanGarbageCollector
///
/// Handles garbage collection of vulkan objects by delaying their destruction
/// until those objects are no longer used.
///
class BgiVulkanGarbageCollector final
{
public:
    BGIVULKAN_API
    BgiVulkanGarbageCollector(BgiVulkan* bgi);

    BGIVULKAN_API
    ~BgiVulkanGarbageCollector();

    /// Destroys the objects inside the garbage collector.
    /// Thread safety: This call is not thread safe and should only be called
    /// while no other threads are destroying objects (e.g. during EndFrame).
    BGIVULKAN_API
    void PerformGarbageCollection(BgiVulkanDevice* device);

    /// Returns a garbage collection vector for a type of handle.
    /// Thread safety: The returned vector is a thread_local vector so this call
    /// is thread safe as long as the vector is only used by the calling thread.
    BgiVulkanBufferVector* GetBufferList();
    BgiVulkanTextureVector* GetTextureList();
    BgiVulkanSamplerVector* GetSamplerList();
    BgiVulkanShaderFunctionVector* GetShaderFunctionList();
    BgiVulkanShaderProgramVector* GetShaderProgramList();
    BgiVulkanResourceBindingsVector* GetResourceBindingsList();
    BgiVulkanGraphicsPipelineVector* GetGraphicsPipelineList();
    BgiVulkanComputePipelineVector* GetComputePipelineList();

private:
    BgiVulkanGarbageCollector & operator =
        (const BgiVulkanGarbageCollector&) = delete;
    BgiVulkanGarbageCollector(const BgiVulkanGarbageCollector&) = delete;

    /// Returns a thread_local vector in which to store a object handle.
    /// Thread safety: The returned vector is a thread_local vector so this call
    /// is thread safe as long as the vector is only used by the calling thread.
    template<class T>
    T* _GetThreadLocalStorageList(std::vector<T*>* collector);

    BgiVulkan* _bgi;

    // List of all the per-thread-vectors of objects that need to be destroyed.
    // The vectors are static (shared across HGIs), because we use thread_local
    // in _GetThreadLocalStorageList which makes us share the garbage collector
    // vectors across Hgi instances.
    static std::vector<BgiVulkanBufferVector*> _bufferList;
    static std::vector<BgiVulkanTextureVector*> _textureList;
    static std::vector<BgiVulkanSamplerVector*> _samplerList;
    static std::vector<BgiVulkanShaderFunctionVector*> _shaderFunctionList;
    static std::vector<BgiVulkanShaderProgramVector*> _shaderProgramList;
    static std::vector<BgiVulkanResourceBindingsVector*> _resourceBindingsList;
    static std::vector<BgiVulkanGraphicsPipelineVector*> _graphicsPipelineList;
    static std::vector<BgiVulkanComputePipelineVector*> _computePipelineList;

    bool _isDestroying;
};

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
