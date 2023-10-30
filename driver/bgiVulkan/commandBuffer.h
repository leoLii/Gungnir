#ifndef GUNGNIR_DRIVER_VULKAN_COMMAND_BUFFER_H
#define GUNGNIR_DRIVER_VULKAN_COMMAND_BUFFER_H

#include "core/base.h"

#include "driver/bgiBase/enums.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkan.h"

#include <functional>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

class BgiVulkanDevice;

using BgiVulkanCompletedHandler = std::function<void(void)>;
using BgiVulkanCompletedHandlerVector = std::vector<BgiVulkanCompletedHandler>;


/// \class HgiVulkanCommandBuffer
///
/// Represents a primary command buffer in Vulkan.
/// Command buffers are managed by the CommandQueue.
///
class BgiVulkanCommandBuffer final
{
public:
    BGIVULKAN_API
    BgiVulkanCommandBuffer(BgiVulkanDevice* device, VkCommandPool pool);

    BGIVULKAN_API
    ~BgiVulkanCommandBuffer();

    /// Ensures that the command buffer is ready to receive commands.
    /// When recording is finished, submit the command buffer to CommandQueue.
    /// The 'inflightId' must uniquely identify the command buffer amongst all
    /// in-flight command buffers.
    BGIVULKAN_API
    void BeginCommandBuffer(uint8_t inflightId);

    /// End the ability to record commands. This should be called before
    /// submitting the command buffer to the queue.
    BGIVULKAN_API
    void EndCommandBuffer();

    /// Returns true if the command buffer is recording commands or being
    /// consumed by the GPU. Returns false if command buffer is available.
    BGIVULKAN_API
    bool IsInFlight() const;

    /// Returns the vulkan command buffer.
    BGIVULKAN_API
    VkCommandBuffer GetVulkanCommandBuffer() const;

    /// Returns the vulkan command pool that allocated this command buffer.
    BGIVULKAN_API
    VkCommandPool GetVulkanCommandPool() const;

    /// Returns the fence for this command buffer that is used to track when
    /// the command buffer has been consumed by the gpu.
    BGIVULKAN_API
    VkFence GetVulkanFence() const;

    /// Returns the semaphore that will be signaled when this command buffer
    /// has been completed in the queue.
    BGIVULKAN_API
    VkSemaphore GetVulkanSemaphore() const;

    /// Resets the cmd buffer if it has been consumed by the GPU.
    /// Returns true if the command buffer was reset, false if it was not reset.
    /// 'not reset' means it is still inflight or that it was previously reset.
    /// When the command buffer was reset the 'CompletedHandler' fns will have
    /// been executed in case of GPU->CPU read back cmds.
    // If wait = HgiSubmitWaitTypeWaitUntilCompleted, the function will wait 
    // for the command buffer to be consumed before continuing.
    BGIVULKAN_API
    bool ResetIfConsumedByGPU(HgiSubmitWaitType wait);

    /// Inserts a barrier so that data written to memory by commands before
    /// the barrier is available to commands after the barrier.
    BGIVULKAN_API
    void InsertMemoryBarrier(HgiMemoryBarrier barrier);

    /// Returns the id that uniquely identifies this command buffer amongst
    /// all in-flight command buffers.
    BgIVULKAN_API
    uint8_t GetInflightId() const;

    /// Returns the device that was used to create the command buffer.
    BGIVULKAN_API
    BgiVulkanDevice* GetDevice() const;

    /// Insert a function that gets run when the command buffer has been
    /// consumed ("completed") on the GPU and the cmd buf is reset.
    BGIVULKAN_API
    void AddCompletedHandler(BgiVulkanCompletedHandler const& fn);

    /// Executes any CompleteHandler functions and clears the list.
    BGIVULKAN_API
    void RunAndClearCompletedHandlers();

private:
    BgiVulkanCommandBuffer() = delete;
    BgiVulkanCommandBuffer & operator= (const BgiVulkanCommandBuffer&) = delete;
    BgiVulkanCommandBuffer(const BgiVulkanCommandBuffer&) = delete;

    // Returns the flags that will be used to reset command buffers.
    static VkCommandBufferResetFlags _GetCommandBufferResetFlags();

    BgiVulkanDevice* _device;
    VkCommandPool _vkCommandPool;
    VkCommandBuffer _vkCommandBuffer;
    VkFence _vkFence;
    VkSemaphore _vkSemaphore;

    BgiVulkanCompletedHandlerVector _completedHandlers;

    bool _isInFlight;
    bool _isSubmitted;
    uint8_t _inflightId;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_VULKAN_COMMAND_BUFFER_H
