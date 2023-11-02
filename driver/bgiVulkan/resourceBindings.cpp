#include "core/utils/diagnostic.h"

#include "driver/bgiVulkan/resourceBindings.h"
#include "driver/bgiVulkan/buffer.h"
#include "driver/bgiVulkan/capabilities.h"
#include "driver/bgiVulkan/conversions.h"
#include "driver/bgiVulkan/device.h"
#include "driver/bgiVulkan/diagnostic.h"
#include "driver/bgiVulkan/sampler.h"
#include "driver/bgiVulkan/texture.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace {
    static const uint8_t _descriptorSetCnt = 1;
}

static VkDescriptorSetLayout
_CreateDescriptorSetLayout(
    BgiVulkanDevice* device,
    std::vector<VkDescriptorSetLayoutBinding> const& bindings,
    std::string const& debugName)
{
    // Create descriptor
    VkDescriptorSetLayoutCreateInfo setCreateInfo =
        {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    setCreateInfo.bindingCount = (uint32_t) bindings.size();
    setCreateInfo.pBindings = bindings.data();
    setCreateInfo.pNext = nullptr;

    VkDescriptorSetLayout layout = nullptr;
    UTILS_VERIFYIFY(
        vkCreateDescriptorSetLayout(
            device->GetVulkanDevice(),
            &setCreateInfo,
            BgiVulkanAllocator(),
            &layout) == VK_SUCCESS
    );

    // Debug label
    if (!debugName.empty()) {
        std::string debugLabel = "DescriptorSetLayout " + debugName;
        BgiVulkanSetDebugName(
            device,
            (uint64_t)layout,
            VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,
            debugLabel.c_str());
    }

    return layout;
}

BgiVulkanResourceBindings::BgiVulkanResourceBindings(
    BgiVulkanDevice* device,
    BgiResourceBindingsDesc const& desc)
    : BgiResourceBindings(desc)
    , _device(device)
    , _inflightBits(0)
    , _vkDescriptorPool(nullptr)
    , _vkDescriptorSetLayout(nullptr)
    , _vkDescriptorSet(nullptr)
{
    // Initialize the pool sizes for each descriptor type we support
    std::vector<VkDescriptorPoolSize> poolSizes;
    poolSizes.resize(BgiBindResourceTypeCount);

    for (size_t i=0; i<BgiBindResourceTypeCount; i++) {
        BgiBindResourceType bt = BgiBindResourceType(i);
        VkDescriptorPoolSize p;
        p.descriptorCount = 0;
        p.type = BgiVulkanConversions::GetDescriptorType(bt);
        poolSizes[i] = p;
    }

    // OpenGL (and Metal) have separate bindings for each buffer and image type.
    // Ubo, ssbo, sampler2D, image all start at bindingIndex 0. So we expect
    // that Hgi clients may specify OpenGL style bindingIndex for each.
    // In Vulkan, bindingIndices are shared (incremented) across all resources.
    // We could split all four into a separate descriptorSet and set the
    // slot=XX in the shader. Instead we keep all resources in one
    // descriptor set and increment all Hgi binding indices here.
    // This assumes that Hgi codeGen does the same for vulkan glsl.

    // For non-bindless buffers in Storm, uniform and storage buffers share a 
    // binding index counter, while textures have their own binding index 
    // counter. Thus for Vulkan, we adjust the texture bind indices to start 
    // after the last buffer bind index.
    // E.g. If HgiResourceBindingDesc indicates the following binding indices:
    // UBO1: 0, SSBO1: 1, SSB02: 2, TEX1: 0, TEX2: 1, here we change that to:
    // UBO1: 0, SSBO1: 1, SSB02: 2, TEX1: 3, TEX2: 4.

    uint32_t textureBindIndexStart = 0;
    
    // XXX We need to overspecify the stage usage here so we can match the 
    // VkDescriptorSetLayout that is created with spirv-reflect for the 
    // graphics and compute pipelines.
    VkShaderStageFlags const bufferShaderStageFlags =
        BgiVulkanConversions::GetShaderStages(
            BgiShaderStageVertex | BgiShaderStageTessellationControl |
            BgiShaderStageTessellationEval | BgiShaderStageGeometry |
            BgiShaderStageFragment);
    VkShaderStageFlags const textureShaderStageFlags =
        BgiVulkanConversions::GetShaderStages(
            BgiShaderStageGeometry | BgiShaderStageFragment);

    // Create DescriptorSetLayout to describe resource bindings.
    //
    // Buffers
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    for (BgiBufferBindDesc const& b : desc.buffers) {
        VkDescriptorSetLayoutBinding d = {};
        d.binding = b.bindingIndex;
        d.descriptorType =
            BgiVulkanConversions::GetDescriptorType(b.resourceType);
        poolSizes[b.resourceType].descriptorCount++;
        d.descriptorCount = (uint32_t) b.buffers.size();
        d.stageFlags = (b.stageUsage == BgiShaderStageCompute) ?
            BgiVulkanConversions::GetShaderStages(b.stageUsage) :
            bufferShaderStageFlags;
        d.pImmutableSamplers = nullptr;
        bindings.push_back(std::move(d));

        textureBindIndexStart =
            std::max(textureBindIndexStart, b.bindingIndex + 1);
    }

    // Textures
    for (BgiTextureBindDesc const& t : desc.textures) {
        VkDescriptorSetLayoutBinding d = {};
        d.binding = textureBindIndexStart + t.bindingIndex;
        d.descriptorType =
            BgiVulkanConversions::GetDescriptorType(t.resourceType);
        poolSizes[t.resourceType].descriptorCount++;
        d.descriptorCount = (uint32_t) t.textures.size();
        d.stageFlags = (t.stageUsage == BgiShaderStageCompute) ?
            BgiVulkanConversions::GetShaderStages(t.stageUsage) :
            textureShaderStageFlags;
        d.pImmutableSamplers = nullptr;
        bindings.push_back(std::move(d));
    }

    // Create descriptor set layout
    _vkDescriptorSetLayout =
        _CreateDescriptorSetLayout(_device, bindings, _descriptor.debugName);

    //
    // Create the descriptor pool.
    //
    // XXX For now each resource bindings gets its own pool to allocate its
    // descriptor sets from to simplify multi-threading support.
    for (size_t i=poolSizes.size(); i-- > 0;) {
        // Vulkan validation will complain if any descriptorCount is 0.
        // Instead of removing them we set a minimum of 1. An empty poolSize
        // will not let us create the pool, which prevents us from creating
        // the descriptorSets.
        poolSizes[i].descriptorCount=std::max(poolSizes[i].descriptorCount, 1u);
    }

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = _descriptorSetCnt;
    pool_info.poolSizeCount = (uint32_t) poolSizes.size();
    pool_info.pPoolSizes = poolSizes.data();

    UTILS_VERIFY(
        vkCreateDescriptorPool(
            _device->GetVulkanDevice(),
            &pool_info,
            BgiVulkanAllocator(),
            &_vkDescriptorPool) == VK_SUCCESS
    );

    // Debug label
    if (!_descriptor.debugName.empty()) {
        std::string debugLabel = "Descriptor Pool " + _descriptor.debugName;
        BgiVulkanSetDebugName(
            device,
            (uint64_t)_vkDescriptorPool,
            VK_OBJECT_TYPE_DESCRIPTOR_POOL,
            debugLabel.c_str());
    }

    //
    // Create Descriptor Set
    //
    VkDescriptorSetAllocateInfo allocateInfo =
        {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};

    allocateInfo.descriptorPool = _vkDescriptorPool;
    allocateInfo.descriptorSetCount = _descriptorSetCnt;
    allocateInfo.pSetLayouts = &_vkDescriptorSetLayout;

    UTILS_VERIFY(
        vkAllocateDescriptorSets(
            _device->GetVulkanDevice(),
            &allocateInfo,
            &_vkDescriptorSet) == VK_SUCCESS
    );

    // Debug label
    if (!_descriptor.debugName.empty()) {
        std::string dbgLbl = "Descriptor Set Buffers " + _descriptor.debugName;
        BgiVulkanSetDebugName(
            _device,
            (uint64_t)_vkDescriptorSet,
            VK_OBJECT_TYPE_DESCRIPTOR_SET,
            dbgLbl.c_str());
    }

    //
    // Setup limits for each resource type
    //
    VkPhysicalDeviceProperties const& devProps =
        _device->GetDeviceCapabilities().vkDeviceProperties;
    VkPhysicalDeviceLimits const& limits = devProps.limits;

    uint32_t bindLimits[BgiBindResourceTypeCount][2] = {
        {BgiBindResourceTypeSampler,
            limits.maxPerStageDescriptorSamplers},
        {BgiBindResourceTypeSampledImage,
            limits.maxPerStageDescriptorSampledImages},
        {BgiBindResourceTypeCombinedSamplerImage,
            limits.maxPerStageDescriptorSampledImages},
        {BgiBindResourceTypeStorageImage,
            limits.maxPerStageDescriptorStorageImages},
        {BgiBindResourceTypeUniformBuffer,
            limits.maxPerStageDescriptorUniformBuffers},
        {BgiBindResourceTypeStorageBuffer,
            limits.maxPerStageDescriptorStorageBuffers},
        {BgiBindResourceTypeTessFactors,
            0} // unsupported
    };
    static_assert(BgiBindResourceTypeCount==7, "");

    //
    // Buffers
    //

    std::vector<VkWriteDescriptorSet> writeSets;

    std::vector<VkDescriptorBufferInfo> bufferInfos;
    bufferInfos.reserve(desc.buffers.size());

    for (BgiBufferBindDesc const& bufDesc : desc.buffers) {
        uint32_t & limit = bindLimits[bufDesc.resourceType][1];
        if (!UTILS_VERIFY(limit>0, "Maximum size array-of-buffers exceeded")) {
            break;
        }
        limit -= 1;

        UTILS_VERIFY(bufDesc.buffers.size() == bufDesc.offsets.size());

        // Each buffer can be an array of buffers (usually one)
        for (size_t i=0; i<bufDesc.buffers.size(); i++) {
            BgiBufferHandle const& bufHandle = bufDesc.buffers[i];
            BgiVulkanBuffer* buf =
                static_cast<BgiVulkanBuffer*>(bufHandle.Get());
            if (!UTILS_VERIFY(buf)) continue;
            VkDescriptorBufferInfo bufferInfo;
            bufferInfo.buffer = buf->GetVulkanBuffer();
            bufferInfo.offset = bufDesc.offsets[i];
            bufferInfo.range = VK_WHOLE_SIZE;
            bufferInfos.push_back(std::move(bufferInfo));
        }
    }

    size_t bufInfoOffset = 0;
    for (BgiBufferBindDesc const& bufDesc : desc.buffers) {
        VkWriteDescriptorSet writeSet= {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        writeSet.dstBinding = bufDesc.bindingIndex;
        writeSet.dstArrayElement = 0;
        writeSet.descriptorCount = (uint32_t) bufDesc.buffers.size(); // 0 ok
        writeSet.dstSet = _vkDescriptorSet;
        writeSet.pBufferInfo = bufferInfos.data() + bufInfoOffset;
        writeSet.pImageInfo = nullptr;
        writeSet.pTexelBufferView = nullptr;
        writeSet.descriptorType =
            BgiVulkanConversions::GetDescriptorType(bufDesc.resourceType);
        writeSets.push_back(std::move(writeSet));
        bufInfoOffset += bufDesc.buffers.size();
    }

    //
    // Textures
    //

    std::vector<VkDescriptorImageInfo> imageInfos;
    imageInfos.reserve(desc.textures.size());

    for (BgiTextureBindDesc const& texDesc : desc.textures) {

        uint32_t & limit = bindLimits[texDesc.resourceType][1];
        if (!UTILS_VERIFY(limit>0, "Maximum array-of-texture/samplers exceeded")) {
            break;
        }
        limit -= 1;

        // Each texture can be an array of textures
        for (size_t i=0; i< texDesc.textures.size(); i++) {
            BgiTextureHandle const& texHandle = texDesc.textures[i];
            BgiVulkanTexture* tex =
                static_cast<BgiVulkanTexture*>(texHandle.Get());
            if (!UTILS_VERIFY(tex)) continue;

            // Not having a sampler is ok only for StorageImage.
            BgiVulkanSampler* smp = nullptr;
            if (i < texDesc.samplers.size()) {
                BgiSamplerHandle const& smpHandle = texDesc.samplers[i];
                smp = static_cast<BgiVulkanSampler*>(smpHandle.Get());
            }

            VkDescriptorImageInfo imageInfo;
            imageInfo.sampler = smp ? smp->GetVulkanSampler() : nullptr;
            imageInfo.imageLayout = tex->GetImageLayout();
            imageInfo.imageView = tex->GetImageView();
            imageInfos.push_back(std::move(imageInfo));
        }
    }

    size_t texInfoOffset = 0;
    for (BgiTextureBindDesc const& texDesc : desc.textures) {
        // For dstBinding we must provided an index in descriptor set.
        // Must be one of the bindings specified in VkDescriptorSetLayoutBinding
        VkWriteDescriptorSet writeSet= {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
        writeSet.dstBinding = textureBindIndexStart + texDesc.bindingIndex;
        writeSet.dstArrayElement = 0;
        writeSet.descriptorCount = (uint32_t) texDesc.textures.size(); // 0 ok
        writeSet.dstSet = _vkDescriptorSet;
        writeSet.pBufferInfo = nullptr;
        writeSet.pImageInfo = imageInfos.data() + texInfoOffset;
        writeSet.pTexelBufferView = nullptr;
        writeSet.descriptorType =
            BgiVulkanConversions::GetDescriptorType(texDesc.resourceType);
        writeSets.push_back(std::move(writeSet));
        texInfoOffset += texDesc.textures.size();
    }

    // Note: this update is immediate. It is not recorded via a command.
    // This means we should only do this if the descriptorSet is not currently
    // in use on GPU. With 'descriptor indexing' extension this has relaxed a
    // little and we are allowed to use vkUpdateDescriptorSets before
    // vkBeginCommandBuffer and after vkEndCommandBuffer, just not during the
    // command buffer recording.
    vkUpdateDescriptorSets(
        _device->GetVulkanDevice(),
        (uint32_t) writeSets.size(),
        writeSets.data(),
        0,        // copy count
        nullptr); // copy_desc
}

BgiVulkanResourceBindings::~BgiVulkanResourceBindings()
{
    vkDestroyDescriptorSetLayout(
        _device->GetVulkanDevice(),
        _vkDescriptorSetLayout,
        BgiVulkanAllocator());

    // Since we have one pool for this resourceBindings we can reset the pool
    // instead of freeing the descriptorSets (vkFreeDescriptorSets).
    vkDestroyDescriptorPool(
        _device->GetVulkanDevice(),
        _vkDescriptorPool,
        BgiVulkanAllocator());
}

void
BgiVulkanResourceBindings::BindResources(
    VkCommandBuffer cb,
    VkPipelineBindPoint bindPoint,
    VkPipelineLayout layout)
{
    // When binding new resources for the currently bound pipeline it may
    // 'disturb' previously bound resources (for a previous pipeline) that
    // are no longer compatible with the layout for the new pipeline.
    // This essentially unbinds the old resources.

    vkCmdBindDescriptorSets(
        cb,
        bindPoint,
        layout,
        0, // firstSet/slot - Hgi does not provide slot index, assume 0.
        _descriptorSetCnt,
        &_vkDescriptorSet,
        0, // dynamicOffset
        nullptr);
}

BgiVulkanDevice*
BgiVulkanResourceBindings::GetDevice() const
{
    return _device;
}

uint64_t &
BgiVulkanResourceBindings::GetInflightBits()
{
    return _inflightBits;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
