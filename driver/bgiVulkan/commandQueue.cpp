#include "core/utils/diagnostic.h"

#include "driver/bgiVulkan/commandQueue.h"
#include "driver/bgiVulkan/commandBuffer.h"
#include "driver/bgiVulkan/device.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

static BgiVulkanCommandQueue::BgiVulkan_CommandPool*
_CreateCommandPool(BgiVulkanDevice* device)
{
    VkCommandPoolCreateInfo poolCreateInfo =
        {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    poolCreateInfo.flags =
        VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |           // short lived
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // reset individually

    // If Graphics and Compute were to come from different queue families we
    // would need to use a different commandpool/buffer for gfx vs compute.
    poolCreateInfo.queueFamilyIndex = device->GetGfxQueueFamilyIndex();

    VkCommandPool pool = nullptr;

    UTILS_VERIFY(
        vkCreateCommandPool(
            device->GetVulkanDevice(),
            &poolCreateInfo,
            BgiVulkanAllocator(),
            &pool) == VK_SUCCESS
    );

    BgiVulkanCommandQueue::BgiVulkan_CommandPool* newPool =
        new BgiVulkanCommandQueue::BgiVulkan_CommandPool();

    newPool->vkCommandPool = pool;
    return newPool;
}

static void
_DestroyCommandPool(
    BgiVulkanDevice* device,
    BgiVulkanCommandQueue::BgiVulkan_CommandPool* pool)
{
    for (BgiVulkanCommandBuffer* cb : pool->commandBuffers) {
        delete cb;
    }
    pool->commandBuffers.clear();

    vkDestroyCommandPool(
        device->GetVulkanDevice(),
        pool->vkCommandPool,
        BgiVulkanAllocator());

    pool->vkCommandPool = nullptr;
    delete pool;
}

BgiVulkanCommandQueue::BgiVulkanCommandQueue(BgiVulkanDevice* device)
    : _device(device)
    , _vkGfxQueue(nullptr)
    , _inflightBits(0)
    , _inflightCounter(0)
    , _threadId(std::this_thread::get_id())
    , _resourceCommandBuffer(nullptr)
{
    // Acquire the graphics queue
    const uint32_t firstQueueInFamily = 0;
    vkGetDeviceQueue(
        device->GetVulkanDevice(),
        device->GetGfxQueueFamilyIndex(),
        firstQueueInFamily,
        &_vkGfxQueue);
}

BgiVulkanCommandQueue::~BgiVulkanCommandQueue()
{
    for (auto const& it : _commandPools) {
        _DestroyCommandPool(_device, it.second);
    }
    _commandPools.clear();
}

/* Externally synchronized */
void
BgiVulkanCommandQueue::SubmitToQueue(
    BgiVulkanCommandBuffer* cb,
    BgiSubmitWaitType wait)
{
    VkSemaphore semaphore = nullptr;

    // If we have resource commands submit those before work commands.
    // It would be more performant to submit both command buffers to the queue
    // at the same time, but we have to signal the fence for each since we use
    // the fence to determine when a command buffer can be reused.
    if (_resourceCommandBuffer) {
        _resourceCommandBuffer->EndCommandBuffer();
        VkCommandBuffer rcb = _resourceCommandBuffer->GetVulkanCommandBuffer();
        semaphore = _resourceCommandBuffer->GetVulkanSemaphore();
        VkFence rFence = _resourceCommandBuffer->GetVulkanFence();

        VkSubmitInfo resourceInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
        resourceInfo.commandBufferCount = 1;
        resourceInfo.pCommandBuffers = &rcb;
        resourceInfo.signalSemaphoreCount = 1;
        resourceInfo.pSignalSemaphores = &semaphore;

        UTILS_VERIFY(
            vkQueueSubmit(_vkGfxQueue, 1, &resourceInfo, rFence) == VK_SUCCESS
        );

        _resourceCommandBuffer = nullptr;
    }

    // XXX Ideally EndCommandBuffer is called on the thread that used it since
    // this can be a heavy operation. However, currently Hgi does not provide
    // a 'EndRecording' function on its Hgi*Cmds that clients must call.
    cb->EndCommandBuffer();
    VkCommandBuffer wcb = cb->GetVulkanCommandBuffer();
    VkFence wFence = cb->GetVulkanFence();

    VkSubmitInfo workInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    workInfo.commandBufferCount = 1;
    workInfo.pCommandBuffers = &wcb;
    VkPipelineStageFlags waitMask;
    if (semaphore) {
        workInfo.waitSemaphoreCount = 1;
        workInfo.pWaitSemaphores = &semaphore;
        waitMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        workInfo.pWaitDstStageMask = &waitMask;
    }

    // Submit provided command buffers to GPU queue.
    // Record and submission order does not guarantee execution order.
    // VK docs: "Execution Model" & "Implicit Synchronization Guarantees".
    // The vulkan queue must be externally synchronized.
    UTILS_VERIFY(
        vkQueueSubmit(_vkGfxQueue, 1, &workInfo, wFence) == VK_SUCCESS
    );

    // Optional blocking wait
    if (wait == BgiSubmitWaitTypeWaitUntilCompleted) {
        static const uint64_t timeOut = 100000000000;
        VkDevice vkDevice = _device->GetVulkanDevice();
        UTILS_VERIFY(
            vkWaitForFences(vkDevice, 1, &wFence, VK_TRUE, timeOut)==VK_SUCCESS
        );
        // When the client waits for the cmd buf to finish on GPU they will
        // expect to have the CompletedHandlers run. For example when the
        // client wants to do a GPU->CPU read back (memcpy)
        cb->RunAndClearCompletedHandlers();
    }
}

/* Multi threaded */
BgiVulkanCommandBuffer*
BgiVulkanCommandQueue::AcquireCommandBuffer()
{
    // Find the thread's command pool.
    BgiVulkan_CommandPool* pool =
        _AcquireThreadCommandPool(std::this_thread::get_id());

    // Grab one of the available command buffers.
    BgiVulkanCommandBuffer* cmdBuf = nullptr;
    for (BgiVulkanCommandBuffer* cb : pool->commandBuffers) {
        if (!cb->IsInFlight()) {
            cmdBuf = cb;
            break;
        }
    }

    // If no command buffer was available, create a new one.
    if (!cmdBuf) {
        cmdBuf = new BgiVulkanCommandBuffer(_device, pool->vkCommandPool);
        pool->commandBuffers.push_back(cmdBuf);
    }

    // Acquire an unique id for this cmd buffer amongst inflight cmd buffers.
    uint8_t inflightId = _AcquireInflightIdBit();
    _SetInflightBit(inflightId, /*enabled*/ true);

    // Begin recording to ensure the caller has exclusive access to cmd buffer.
    cmdBuf->BeginCommandBuffer(inflightId);
    return cmdBuf;
}

/* Single threaded */
BgiVulkanCommandBuffer*
BgiVulkanCommandQueue::AcquireResourceCommandBuffer()
{
    // XXX We currently have only one resource command buffer. We can get away
    // with this since Hgi::Create* must currently happen on the main thread.
    // Once we change that, we must support resource command buffers on
    // secondary threads.
    UTILS_VERIFY(std::this_thread::get_id() == _threadId);

    if (!_resourceCommandBuffer) {
        _resourceCommandBuffer = AcquireCommandBuffer();
    }
    return _resourceCommandBuffer;
}

/* Multi threaded */
uint64_t
BgiVulkanCommandQueue::GetInflightCommandBuffersBits()
{
    return _inflightBits.load();
}

/* Multi threaded */
VkQueue
BgiVulkanCommandQueue::GetVulkanGraphicsQueue() const
{
    return _vkGfxQueue;
}

/* Single threaded */
void
BgiVulkanCommandQueue::ResetConsumedCommandBuffers(BgiSubmitWaitType wait)
{
    // Lock the command pool map from concurrent access since we may insert.
    std::lock_guard<std::mutex> guard(_commandPoolsMutex);

    // Loop all pools and reset any command buffers that have been consumed.
    for (auto it : _commandPools) {
        BgiVulkan_CommandPool* pool = it.second;
        for (BgiVulkanCommandBuffer* cb : pool->commandBuffers) {
            if (cb->ResetIfConsumedByGPU(wait)) {
                _SetInflightBit(cb->GetInflightId(), /*enabled*/ false);
            }
        }
    }
}

/* Multi threaded */
BgiVulkanCommandQueue::BgiVulkan_CommandPool*
BgiVulkanCommandQueue::_AcquireThreadCommandPool(
    std::thread::id const& threadId)
{
    // Lock the command pool map from concurrent access since we may insert.
    std::lock_guard<std::mutex> guard(_commandPoolsMutex);

    auto it = _commandPools.find(threadId);
    if (it == _commandPools.end()) {
        BgiVulkan_CommandPool* newPool = _CreateCommandPool(_device);
        _commandPools[threadId] = newPool;
        return newPool;
    } else {
        return it->second;
    }
}

/* Multi threaded */
uint8_t
BgiVulkanCommandQueue::_AcquireInflightIdBit()
{
    // Command buffers can be acquired by threads, so we need to do an
    // increment that is thread safe. We circle back to the first bit after
    // all bits have been used once. These means we can track the in-flight
    // status of up to 64 consecutive command buffer usages.
    // This becomes important in garbage collection and is explained more there.
    return _inflightCounter.fetch_add(1) % 64;
}

/* Multi threaded */
void
BgiVulkanCommandQueue::_SetInflightBit(uint8_t id, bool enabled)
{
    // We need to set the bit atomically since this function can be called by
    // multiple threads. Try to set the value and if it fails (another thread
    // may have updated the `expected` value!), we re-apply our bit and
    // try again.
    uint64_t expect = _inflightBits.load();

    if (enabled) {
        // Spin if bit was already enabled. This means we have reached our max
        // of 64 command buffers and must wait until it becomes available.
        expect &= ~(1ULL<<id);
        while (!_inflightBits.compare_exchange_weak(
            expect, expect | (1ULL<<id))) 
        {
            expect &= ~(1ULL<<id);
        }
    } else {
        while (!_inflightBits.compare_exchange_weak(
            expect, expect & ~(1ULL<<id)));
    }
}

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
