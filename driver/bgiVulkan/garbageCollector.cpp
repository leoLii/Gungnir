#include "pxr/base/arch/hints.h"
#include "pxr/base/tf/diagnostic.h"

#include "driver/bgiVulkan/garbageCollector.h"
#include "driver/bgiVulkan/buffer.h"
#include "driver/bgiVulkan/commandQueue.h"
#include "driver/bgiVulkan/computePipeline.h"
#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/graphicsPipeline.h"
#include "driver/bgiVulkan/bgi.h"
#include "driver/bgiVulkan/resourceBindings.h"
#include "driver/bgiVulkan/sampler.h"
#include "driver/bgiVulkan/shaderFunction.h"
#include "driver/bgiVulkan/shaderProgram.h"
#include "driver/bgiVulkan/texture.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

std::vector<BgiVulkanBufferVector*> 
    BgiVulkanGarbageCollector::_bufferList;
std::vector<BgiVulkanTextureVector*> 
    BgiVulkanGarbageCollector::_textureList;
std::vector<BgiVulkanSamplerVector*> 
    BgiVulkanGarbageCollector::_samplerList;
std::vector<BgiVulkanShaderFunctionVector*> 
    BgiVulkanGarbageCollector::_shaderFunctionList;
std::vector<BgiVulkanShaderProgramVector*> 
    BgiVulkanGarbageCollector::_shaderProgramList;
std::vector<BgiVulkanResourceBindingsVector*> 
    BgiVulkanGarbageCollector::_resourceBindingsList;
std::vector<BgiVulkanGraphicsPipelineVector*> 
    BgiVulkanGarbageCollector::_graphicsPipelineList;
std::vector<BgiVulkanComputePipelineVector*> 
    BgiVulkanGarbageCollector::_computePipelineList;

template<class T>
static void _EmptyTrash(
    std::vector<std::vector<T*>*>* list,
    VkDevice vkDevice,
    uint64_t queueInflightBits)
{
    // Loop the garbage vectors of each thread
    for (auto vec : *list) {
        for (size_t i=vec->size(); i-- > 0;) {
            T* object = (*vec)[i];

            // Each device has its own queue, so its own set of inflight bits.
            // We must only destroy objects that belong to this device & queue.
            // (The garbage collector collects objects from all devices)
            if (vkDevice != object->GetDevice()->GetVulkanDevice()) {
                continue;
            }

            // See comments in PerformGarbageCollection.
            if ((queueInflightBits & object->GetInflightBits()) == 0) {
                delete object;
                std::iter_swap(vec->begin() + i, vec->end() - 1);
                vec->pop_back();
            }
        }
    }
}

BgiVulkanGarbageCollector::BgiVulkanGarbageCollector(BgiVulkan* bgi)
    : _bgi(bgi)
    , _isDestroying(false)
{
}

BgiVulkanGarbageCollector::~BgiVulkanGarbageCollector() = default;

/* Multi threaded */
BgiVulkanBufferVector*
BgiVulkanGarbageCollector::GetBufferList()
{
    return _GetThreadLocalStorageList(&_bufferList);
}

/* Multi threaded */
BgiVulkanTextureVector*
BgiVulkanGarbageCollector::GetTextureList()
{
    return _GetThreadLocalStorageList(&_textureList);
}

/* Multi threaded */
BgiVulkanSamplerVector*
BgiVulkanGarbageCollector::GetSamplerList()
{
    return _GetThreadLocalStorageList(&_samplerList);
}

/* Multi threaded */
BgiVulkanShaderFunctionVector*
BgiVulkanGarbageCollector::GetShaderFunctionList()
{
    return _GetThreadLocalStorageList(&_shaderFunctionList);
}

/* Multi threaded */
BgiVulkanShaderProgramVector*
BgiVulkanGarbageCollector::GetShaderProgramList()
{
    return _GetThreadLocalStorageList(&_shaderProgramList);
}

/* Multi threaded */
BgiVulkanResourceBindingsVector*
BgiVulkanGarbageCollector::GetResourceBindingsList()
{
    return _GetThreadLocalStorageList(&_resourceBindingsList);
}

/* Multi threaded */
BgiVulkanGraphicsPipelineVector*
BgiVulkanGarbageCollector::GetGraphicsPipelineList()
{
    return _GetThreadLocalStorageList(&_graphicsPipelineList);
}

/* Multi threaded */
BgiVulkanComputePipelineVector*
BgiVulkanGarbageCollector::GetComputePipelineList()
{
    return _GetThreadLocalStorageList(&_computePipelineList);
}

/* Single threaded */
void
BgiVulkanGarbageCollector::PerformGarbageCollection(BgiVulkanDevice* device)
{
    // Garbage Collection notes:
    //
    // When the client requests objects to be destroyed (Eg. Hgi::DestroyBuffer)
    // we put objects into this garbage collector. At that time we also store
    // the bits of the command buffers that are 'in-flight'.
    // We have to delay destroying the vulkan resources until there are no
    // command buffers using the resource.
    // Instead of tracking complex dependencies between objects and cmd buffers
    // we simply assume that all in-flight command buffers might be using the
    // destroyed object and wait until those command buffers have been
    // consumed by the GPU.
    //
    // In _EmptyTrash we try to delete objects in the garbage collector.
    // We compare the bits of the queue and the object to decide if we can
    // delete the object. Example:
    //
    //    Each command buffer takes up one bit (where 1 means "in-flight").
    //    Queue currently in-flight cmd buf bits:   01001011101
    //    In-flight bits when obj was trashed:      00100000100
    //    Bitwise & result:                         00000000100
    //
    // Conclusion: object cannot yet be destroyed. One command buffer that was
    // in-flight during the destruction request is still in-flight and might
    // still be using the object on the GPU.

    _isDestroying = true;

    // Check what command buffers are in-flight on the device queue.
    BgiVulkanCommandQueue* queue = device->GetCommandQueue();
    uint64_t queueBits = queue->GetInflightCommandBuffersBits();
    VkDevice vkDevice = device->GetVulkanDevice();

    _EmptyTrash(&_bufferList, vkDevice, queueBits);
    _EmptyTrash(&_textureList, vkDevice, queueBits);
    _EmptyTrash(&_samplerList, vkDevice, queueBits);
    _EmptyTrash(&_shaderFunctionList, vkDevice, queueBits);
    _EmptyTrash(&_shaderProgramList, vkDevice, queueBits);
    _EmptyTrash(&_resourceBindingsList, vkDevice, queueBits);
    _EmptyTrash(&_graphicsPipelineList, vkDevice, queueBits);
    _EmptyTrash(&_computePipelineList, vkDevice, queueBits);

    _isDestroying = false;
}

template<class T>
T* BgiVulkanGarbageCollector::_GetThreadLocalStorageList(std::vector<T*>* collector)
{
    if (ARCH_UNLIKELY(_isDestroying)) {
        TF_CODING_ERROR("Cannot destroy object during garbage collection ");
        while(_isDestroying);
    }

    // Only lock and create a new garbage vector if we dont have one in TLS.
    // Using TLS means this we store per type T, not per T and Hgi instance.
    // So if you call garbage collect on one Hgi, it destroys objects across
    // all Hgi's. This should be ok since we only call the destructor of the
    // garbage object.
    thread_local T* _tls = nullptr;
    static std::mutex garbageMutex;

    if (!_tls) {
        _tls = new T();
        std::lock_guard<std::mutex> guard(garbageMutex);
        collector->push_back(_tls);
    }
    return _tls;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
