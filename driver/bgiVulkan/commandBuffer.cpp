#include "pxr/base/tf/diagnostic.h"

#include "driver/bgiVulkan/commandBuffer.h"
#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/diagnostic.h"
#include "driver/bgiVulkan/vulkan.h"

#include <string>

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiVulkanCommandBuffer::BgiVulkanCommandBuffer(
    BgiVulkanDevice* device,
    VkCommandPool pool)
    : _device(device)
    , _vkCommandPool(pool)
    , _vkCommandBuffer(nullptr)
    , _vkFence(nullptr)
    , _vkSemaphore(nullptr)
    , _isInFlight(false)
    , _isSubmitted(false)
    , _inflightId(0)
{
    VkDevice vkDevice = _device->GetVulkanDevice();

    // Create vulkan command buffer
    VkCommandBufferAllocateInfo allocInfo =
        {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    allocInfo.commandBufferCount = 1;
    allocInfo.commandPool = pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    TF_VERIFY(
        vkAllocateCommandBuffers(
            vkDevice,
            &allocInfo,
            &_vkCommandBuffer) == VK_SUCCESS
    );

    // Assign a debug label to command buffer
    uint64_t cmdBufHandle = (uint64_t)_vkCommandBuffer;
    std::string handleStr = std::to_string(cmdBufHandle);
    std::string cmdBufLbl = "HgiVulkan Command Buffer " + handleStr;

    BgiVulkanSetDebugName(
        _device,
        cmdBufHandle,
        VK_OBJECT_TYPE_COMMAND_BUFFER,
        cmdBufLbl.c_str());

    // CPU synchronization fence. So we known when the cmd buffer can be reused.
    VkFenceCreateInfo fenceInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fenceInfo.flags = 0; // Unsignaled starting state

    TF_VERIFY(
        vkCreateFence(
            vkDevice,
            &fenceInfo,
            BgiVulkanAllocator(),
            &_vkFence) == VK_SUCCESS
    );

    // Create semaphore for GPU-GPU synchronization
    VkSemaphoreCreateInfo semaCreateInfo =
        {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    TF_VERIFY(
        vkCreateSemaphore(
            vkDevice,
            &semaCreateInfo,
            BgiVulkanAllocator(),
            &_vkSemaphore) == VK_SUCCESS
    );

    // Assign a debug label to fence.
    std::string fenceLbl = "HgiVulkan Fence for Command Buffer: " + handleStr;
    BgiVulkanSetDebugName(
        _device,
        (uint64_t)_vkFence,
        VK_OBJECT_TYPE_FENCE,
        fenceLbl.c_str());
}

BgiVulkanCommandBuffer::~BgiVulkanCommandBuffer()
{
    VkDevice vkDevice = _device->GetVulkanDevice();
    vkDestroySemaphore(vkDevice, _vkSemaphore, BgiVulkanAllocator());
    vkDestroyFence(vkDevice, _vkFence, BgiVulkanAllocator());
    vkFreeCommandBuffers(vkDevice, _vkCommandPool, 1, &_vkCommandBuffer);
}

void
BgiVulkanCommandBuffer::BeginCommandBuffer(uint8_t inflightId)
{
    if (!_isInFlight) {

        VkCommandBufferBeginInfo beginInfo =
            {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        TF_VERIFY(
            vkBeginCommandBuffer(_vkCommandBuffer, &beginInfo) == VK_SUCCESS
        );

        _inflightId = inflightId;
        _isInFlight = true;
    }
}

bool
BgiVulkanCommandBuffer::IsInFlight() const
{
    return _isInFlight;
}

void
BgiVulkanCommandBuffer::EndCommandBuffer()
{
    if (_isInFlight) {
        TF_VERIFY(
            vkEndCommandBuffer(_vkCommandBuffer) == VK_SUCCESS
        );

        _isSubmitted = true;
    }
}

bool
BgiVulkanCommandBuffer::ResetIfConsumedByGPU(BgiSubmitWaitType wait)
{
    // Command buffer is already available (previously reset).
    // We do not have to test the fence or reset the cmd buffer.
    if (!_isInFlight) {
        return false;
    }

    // The command buffer is still recording. We should not test its fence until
    // we have submitted the command buffer to the queue (vulkan requirement).
    if (!_isSubmitted) {
        return false;
    }

    VkDevice vkDevice = _device->GetVulkanDevice();

    // Check the fence to see if the GPU has consumed the command buffer.
    // We cannnot reuse a command buffer until the GPU is finished with it.
    if (vkGetFenceStatus(vkDevice, _vkFence) == VK_NOT_READY){
        if (wait == BgiSubmitWaitTypeWaitUntilCompleted) {
            static const uint64_t timeOut = 100000000000;
            TF_VERIFY(vkWaitForFences(
                vkDevice, 1, &_vkFence, VK_TRUE, timeOut) == VK_SUCCESS);
        } else {
            return false;
        }
    }

    // GPU is done with command buffer, execute the custom fns the client wants
    // to see executed when cmd buf is consumed.
    RunAndClearCompletedHandlers();

    // GPU is done with command buffer, reset fence and command buffer.
    TF_VERIFY(
        vkResetFences(vkDevice, 1, &_vkFence)  == VK_SUCCESS
    );

    // It might be more efficient to reset the cmd pool instead of individual
    // command buffers. But we may not have a clear 'StartFrame' / 'EndFrame'
    // sequence in Hydra. If we did, we could reset the command pool(s) during
    // Beginframe. Instead we choose to reset each command buffer when it has
    // been consumed by the GPU.

    VkCommandBufferResetFlags flags = _GetCommandBufferResetFlags();
    TF_VERIFY(
        vkResetCommandBuffer(_vkCommandBuffer, flags) == VK_SUCCESS
    );

    // Command buffer may now be reused for new recordings / resource creation.
    _isInFlight = false;
    _isSubmitted = false;
    return true;
}

VkCommandBuffer
BgiVulkanCommandBuffer::GetVulkanCommandBuffer() const
{
    return _vkCommandBuffer;
}

VkCommandPool
BgiVulkanCommandBuffer::GetVulkanCommandPool() const
{
    return _vkCommandPool;
}

VkFence
BgiVulkanCommandBuffer::GetVulkanFence() const
{
    return _vkFence;
}

VkSemaphore
BgiVulkanCommandBuffer::GetVulkanSemaphore() const
{
    return _vkSemaphore;
}

uint8_t
BgiVulkanCommandBuffer::GetInflightId() const
{
    return _inflightId;
}

BgiVulkanDevice*
BgiVulkanCommandBuffer::GetDevice() const
{
    return _device;
}

void
BgiVulkanCommandBuffer::InsertMemoryBarrier(BgiMemoryBarrier barrier)
{
    if (!_vkCommandBuffer) {
        return;
    }

    VkMemoryBarrier memoryBarrier = {VK_STRUCTURE_TYPE_MEMORY_BARRIER};

    // XXX Flush / stall and invalidate all caches (big hammer!).
    // Ideally we would set more fine-grained barriers, but we
    // currently do not get enough information from Hgi to
    // know what src or dst access there is or what images or
    // buffers might be affected.
    TF_VERIFY(barrier==BgiMemoryBarrierAll, "Unsupported barrier");

    // Who might be generating the data we are interested in reading.
    memoryBarrier.srcAccessMask =
        VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;

    // Who might be consuming the data that was writen.
    memoryBarrier.dstAccessMask =
        VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;

    vkCmdPipelineBarrier(
        _vkCommandBuffer,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, // producer (what we wait for)
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, // consumer (what must wait)
        0,                                  // flags
        1,                                  // memoryBarrierCount
        &memoryBarrier,                     // memory barriers
        0, nullptr,                         // buffer barriers
        0, nullptr);                        // image barriers
}

void
BgiVulkanCommandBuffer::AddCompletedHandler(BgiVulkanCompletedHandler const& fn)
{
    _completedHandlers.push_back(fn);
}

void
BgiVulkanCommandBuffer::RunAndClearCompletedHandlers()
{
    for (BgiVulkanCompletedHandler& fn : _completedHandlers) {
        fn();
    }
    _completedHandlers.clear();
}

VkCommandBufferResetFlags
BgiVulkanCommandBuffer::_GetCommandBufferResetFlags()
{
    // For now we do not use VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT,
    // assuming similar memory requirements will be needed each frame.
    // Releasing resources can come at a performance cost.
    static const VkCommandBufferResetFlags flags = 0;
    return flags;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
