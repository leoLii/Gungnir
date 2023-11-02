#include "core/utils/diagnostic.h"

#include "driver/bgiVulkan/buffer.h"
#include "driver/bgiVulkan/commandBuffer.h"
#include "driver/bgiVulkan/commandQueue.h"
#include "driver/bgiVulkan/conversions.h"
#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/diagnostic.h"
#include "driver/bgiVulkan/garbageCollector.h"
#include "driver/bgiVulkan/bgi.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiVulkanBuffer::BgiVulkanBuffer(
    BgiVulkan* bgi,
    BgiVulkanDevice* device,
    BgiBufferDesc const& desc)
    : BgiBuffer(desc)
    , _device(device)
    , _vkBuffer(nullptr)
    , _vmaAllocation(nullptr)
    , _inflightBits(0)
    , _stagingBuffer(nullptr)
    , _cpuStagingAddress(nullptr)
{
    if (desc.byteSize == 0) {
        TF_CODING_ERROR("The size of buffer [%p] is zero.", this);
        return;
    }

    VmaAllocator vma = device->GetVulkanMemoryAllocator();

    VkBufferCreateInfo bi = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bi.size = desc.byteSize;
    bi.usage = BgiVulkanConversions::GetBufferUsage(desc.usage);
    bi.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | 
                VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bi.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // gfx queue only

    // Create buffer with memory allocated and bound.
    // Equivalent to: vkCreateBuffer, vkAllocateMemory, vkBindBufferMemory
    // XXX On VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU it may be beneficial to
    // skip staging buffers and use DEVICE_LOCAL | HOST_VISIBLE_BIT since all
    // memory is shared between CPU and GPU.
    VmaAllocationCreateInfo ai = {};
    ai.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // GPU efficient

    UTILS_VERIFY(
        vmaCreateBuffer(vma,&bi,&ai,&_vkBuffer,&_vmaAllocation,0) == VK_SUCCESS
    );

    // Debug label
    if (!_descriptor.debugName.empty()) {
        std::string debugLabel = "Buffer " + _descriptor.debugName;
        BgiVulkanSetDebugName(
            device,
            (uint64_t)_vkBuffer,
            VK_OBJECT_TYPE_BUFFER,
            debugLabel.c_str());
    }

    if (desc.initialData) {
        // Use a 'staging buffer' to schedule uploading the 'initialData' to
        // the device-local GPU buffer.
        BgiVulkanBuffer* stagingBuffer = CreateStagingBuffer(_device, desc);
        VkBuffer vkStagingBuf = stagingBuffer->GetVulkanBuffer();

        BgiVulkanCommandQueue* queue = device->GetCommandQueue();
        BgiVulkanCommandBuffer* cb = queue->AcquireResourceCommandBuffer();
        VkCommandBuffer vkCmdBuf = cb->GetVulkanCommandBuffer();

        // Copy data from staging buffer to device-local buffer.
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = desc.byteSize;
        vkCmdCopyBuffer(vkCmdBuf, vkStagingBuf, _vkBuffer, 1, &copyRegion);

        // We don't know if this buffer is a static (immutable) or
        // dynamic (animated) buffer. We assume that most buffers are
        // static and schedule garbage collection of staging resource.
        BgiBufferHandle stagingHandle(stagingBuffer, 0);
        bgi->TrashObject(
            &stagingHandle,
            bgi->GetGarbageCollector()->GetBufferList());
    }

    _descriptor.initialData = nullptr;
}

BgiVulkanBuffer::BgiVulkanBuffer(
    BgiVulkanDevice* device,
    VkBuffer vkBuffer,
    VmaAllocation vmaAllocation,
    BgiBufferDesc const& desc)
    : BgiBuffer(desc)
    , _device(device)
    , _vkBuffer(vkBuffer)
    , _vmaAllocation(vmaAllocation)
    , _inflightBits(0)
    , _stagingBuffer(nullptr)
    , _cpuStagingAddress(nullptr)
{
}

BgiVulkanBuffer::~BgiVulkanBuffer()
{
    if (_cpuStagingAddress && _stagingBuffer) {
        vmaUnmapMemory(
            _device->GetVulkanMemoryAllocator(),
            _stagingBuffer->GetVulkanMemoryAllocation());
        _cpuStagingAddress = nullptr;
    }

    delete _stagingBuffer;
    _stagingBuffer = nullptr;

    vmaDestroyBuffer(
        _device->GetVulkanMemoryAllocator(),
        _vkBuffer,
        _vmaAllocation);
}

size_t
BgiVulkanBuffer::GetByteSizeOfResource() const
{
    return _descriptor.byteSize;
}

uint64_t
BgiVulkanBuffer::GetRawResource() const
{
    return (uint64_t) _vkBuffer;
}

void*
BgiVulkanBuffer::GetCPUStagingAddress()
{
    if (!_stagingBuffer) {
        BgiBufferDesc desc = _descriptor;
        desc.initialData = nullptr;
        _stagingBuffer = CreateStagingBuffer(_device, desc);
    }

    if (!_cpuStagingAddress) {
        UTILS_VERIFY(
            vmaMapMemory(
                _device->GetVulkanMemoryAllocator(), 
                _stagingBuffer->GetVulkanMemoryAllocation(), 
                &_cpuStagingAddress) == VK_SUCCESS
        );
    }

    // This lets the client code memcpy into the staging buffer directly.
    // The staging data must be explicitely copied to the device-local
    // GPU buffer via CopyBufferCpuToGpu cmd by the client.
    return _cpuStagingAddress;
}

bool
BgiVulkanBuffer::IsCPUStagingAddress(const void* address) const
{
    return (address == _cpuStagingAddress);
}

VkBuffer
BgiVulkanBuffer::GetVulkanBuffer() const
{
    return _vkBuffer;
}

VmaAllocation
BgiVulkanBuffer::GetVulkanMemoryAllocation() const
{
    return _vmaAllocation;
}

BgiVulkanBuffer*
BgiVulkanBuffer::GetStagingBuffer() const
{
    return _stagingBuffer;
}

BgiVulkanDevice*
BgiVulkanBuffer::GetDevice() const
{
    return _device;
}

uint64_t &
BgiVulkanBuffer::GetInflightBits()
{
    return _inflightBits;
}

BgiVulkanBuffer*
BgiVulkanBuffer::CreateStagingBuffer(
    BgiVulkanDevice* device,
    BgiBufferDesc const& desc)
{
    VmaAllocator vma = device->GetVulkanMemoryAllocator();

    VkBufferCreateInfo bi = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bi.size = desc.byteSize;
    bi.usage = BgiVulkanConversions::GetBufferUsage(desc.usage);
    bi.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | 
                VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bi.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // gfx queue only

    VmaAllocationCreateInfo ai = {};
    ai.requiredFlags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | // CPU access (mem map)
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT; // Dont have to manually flush

    VkBuffer buffer = 0;
    VmaAllocation alloc = 0;
    UTILS_VERIFY(
        vmaCreateBuffer(vma, &bi, &ai, &buffer, &alloc, 0) == VK_SUCCESS
    );

    // Map the (HOST_VISIBLE) buffer and upload data
    if (desc.initialData) {
        void* map;
        UTILS_VERIFY(vmaMapMemory(vma, alloc, &map) == VK_SUCCESS);
        memcpy(map, desc.initialData, desc.byteSize);
        vmaUnmapMemory(vma, alloc);
    }

    // Return new staging buffer (caller manages lifetime)
    return new BgiVulkanBuffer(device, buffer, alloc, desc);
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
