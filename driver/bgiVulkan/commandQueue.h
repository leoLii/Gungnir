#pragma once

#include "core/base.h"

#include "driver/bgiBase/enums.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

class BgiVulkanCommandBuffer;
class BgiVulkanDevice;

/// \class HgiVulkanCommandQueue
///
/// The CommandQueue manages command buffers and their submission to the
/// GPU device queue.
///
class BgiVulkanCommandQueue final
{
public:
    // Holds one thread's command pool and list of command buffers .
    struct BgiVulkan_CommandPool
    {
        VkCommandPool vkCommandPool = nullptr;
        std::vector<BgiVulkanCommandBuffer*> commandBuffers;
    };

    using CommandPoolPtrMap =
        std::unordered_map<std::thread::id, BgiVulkan_CommandPool*>;

    /// Construct a new queue for the provided device.
    BGIVULKAN_API
    BgiVulkanCommandQueue(BgiVulkanDevice* device);

    BGIVULKAN_API
    ~BgiVulkanCommandQueue();

    /// Commits the provided command buffer to GPU queue for processing.
    /// After submission the command buffer must not be re-used by client.
    /// Thread safety: Submission must be externally synchronized. Clients
    /// should call HgiVulkan::SubmitToQueue.
    void SubmitToQueue(
        BgiVulkanCommandBuffer* cmdBuffer,
        BgiSubmitWaitType wait = BgiSubmitWaitTypeNoWait);

    /// Returns a command buffer that is ready to record commands.
    /// The ownership of the command buffer (ptr) remains with this queue. The
    /// caller should not delete it. Instead, submit it back to this queue
    /// when command recording into the buffer has finished.
    /// Thread safety: The returned command buffer may only be used by the
    /// calling thread. Calls to acquire a command buffer are thread safe.
    BGIVULKAN_API
    BgiVulkanCommandBuffer* AcquireCommandBuffer();

    /// Returns a resource command buffer that is ready to record commands.
    /// The ownership of the command buffer (ptr) remains with this queue. The
    /// caller should not delete or submit it. Resource command buffers are
    /// automatically submitted before regular command buffers.
    /// Thread safety: XXX Not thread safe. This call may only happen on the
    /// main-thread and only that thread may use this command buffer.
    BGIVULKAN_API
    BgiVulkanCommandBuffer* AcquireResourceCommandBuffer();

    /// Returns a bit key that holds the in-flight status of all cmd buffers.
    /// This is used for garbage collection to delay destruction of objects
    /// until the currently in-flight command buffers have been consumed.
    /// Thread safety: This call is thread safe.
    BGIVULKAN_API
    uint64_t GetInflightCommandBuffersBits();

    /// Returns the vulkan graphics queue.
    /// Thread safety: This call is thread safe.
    BGIVULKAN_API
    VkQueue GetVulkanGraphicsQueue() const;

    /// Loop all pools and reset any command buffers that have been consumed.
    /// Thread safety: This call is not thread safe. This function should be
    /// called once from main thread while no other threads are recording.
    BGIVULKAN_API
    void ResetConsumedCommandBuffers(
        BgiSubmitWaitType wait = BgiSubmitWaitTypeNoWait);

private:
    BgiVulkanCommandQueue() = delete;
    BgiVulkanCommandQueue & operator=(const BgiVulkanCommandQueue&) = delete;
    BgiVulkanCommandQueue(const BgiVulkanCommandQueue&) = delete;

    // Returns the command pool for a thread.
    // Thread safety: This call is thread safe.
    BgiVulkan_CommandPool* _AcquireThreadCommandPool(
        std::thread::id const& threadId);

    // Returns an id-bit that uniquely identifies the cmd buffer amongst all
    // in-flight cmd buffers.
    // Thread safety: This call is thread safe..
    uint8_t _AcquireInflightIdBit();

    // Set if a command buffer is in-flight (enabled=true) or not.
    // Thread safety: This call is thread safe.
    void _SetInflightBit(uint8_t inflightId, bool enabled);

    BgiVulkanDevice* _device;
    VkQueue _vkGfxQueue;
    CommandPoolPtrMap _commandPools;
    std::mutex _commandPoolsMutex;

    std::atomic<uint64_t> _inflightBits;
    std::atomic<uint8_t> _inflightCounter;

    std::thread::id _threadId;
    BgiVulkanCommandBuffer* _resourceCommandBuffer;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE
