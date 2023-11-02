#include "core/utils/diagnostic.h"

#include "driver/bgiVulkan/sampler.h"
#include "driver/bgiVulkan/capabilities.h"
#include "driver/bgiVulkan/conversions.h"
#include "driver/bgiVulkan/device.h"

#include <float.h>

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiVulkanSampler::BgiVulkanSampler(
    BgiVulkanDevice* device,
    BgiSamplerDesc const& desc)
    : BgiSampler(desc)
    , _vkSampler(nullptr)
    , _device(device)
    , _inflightBits(0)
{
    VkSamplerCreateInfo sampler = {VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    sampler.magFilter = BgiVulkanConversions::GetMinMagFilter(desc.magFilter);
    sampler.minFilter = BgiVulkanConversions::GetMinMagFilter(desc.minFilter);
    sampler.addressModeU =
        BgiVulkanConversions::GetSamplerAddressMode(desc.addressModeU);
    sampler.addressModeV =
        BgiVulkanConversions::GetSamplerAddressMode(desc.addressModeV);
    sampler.addressModeW =
        BgiVulkanConversions::GetSamplerAddressMode(desc.addressModeW);

    // Eg. Percentage-closer filtering
    sampler.compareEnable = desc.enableCompare ? VK_TRUE : VK_FALSE;
    sampler.compareOp = 
        BgiVulkanConversions::GetDepthCompareFunction(desc.compareFunction);

    sampler.borderColor = 
        BgiVulkanConversions::GetBorderColor(desc.borderColor);
    sampler.mipLodBias = 0.0f;
    sampler.mipmapMode = BgiVulkanConversions::GetMipFilter(desc.mipFilter);
    sampler.minLod = 0.0f;
    sampler.maxLod = FLT_MAX;

    BgiVulkanCapabilities const& caps = device->GetDeviceCapabilities();
    sampler.anisotropyEnable = caps.vkDeviceFeatures.samplerAnisotropy;
    sampler.maxAnisotropy = sampler.anisotropyEnable ?
        caps.vkDeviceProperties.limits.maxSamplerAnisotropy : 1.0f;

    UTILS_VERIFY(
        vkCreateSampler(
            device->GetVulkanDevice(),
            &sampler,
            BgiVulkanAllocator(),
            &_vkSampler) == VK_SUCCESS
    );
}

BgiVulkanSampler::~BgiVulkanSampler()
{
    vkDestroySampler(
        _device->GetVulkanDevice(),
        _vkSampler,
        BgiVulkanAllocator());
}

uint64_t
BgiVulkanSampler::GetRawResource() const
{
    return (uint64_t) _vkSampler;
}

VkSampler
BgiVulkanSampler::GetVulkanSampler() const
{
    return _vkSampler;
}

BgiVulkanDevice*
BgiVulkanSampler::GetDevice() const
{
    return _device;
}

uint64_t &
BgiVulkanSampler::GetInflightBits()
{
    return _inflightBits;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE