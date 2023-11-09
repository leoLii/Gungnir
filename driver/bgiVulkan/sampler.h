#pragma once

#include "common/base.h"

#include "driver/bgiBase/sampler.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

class BgiVulkanDevice;

///
/// \class HgiVulkanSampler
///
/// Vulkan implementation of HgiSampler
///
class BgiVulkanSampler final : public BgiSampler
{
public:
    BGIVULKAN_API
    ~BgiVulkanSampler() override;

    BGIVULKAN_API
    uint64_t GetRawResource() const override;

    /// Returns the vulkan sampler object.
    BGIVULKAN_API
    VkSampler GetVulkanSampler() const;

    /// Returns the device used to create this object.
    BGIVULKAN_API
    BgiVulkanDevice* GetDevice() const;

    /// Returns the (writable) inflight bits of when this object was trashed.
    BGIVULKAN_API
    uint64_t & GetInflightBits();

protected:
    friend class BgiVulkan;

    BGIVULKAN_API
    BgiVulkanSampler(
        BgiVulkanDevice* device,
        BgiSamplerDesc const& desc);

private:
    BgiVulkanSampler() = delete;
    BgiVulkanSampler & operator=(const BgiVulkanSampler&) = delete;
    BgiVulkanSampler(const BgiVulkanSampler&) = delete;

    VkSampler _vkSampler;

    BgiVulkanDevice* _device;
    uint64_t _inflightBits;
};

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
