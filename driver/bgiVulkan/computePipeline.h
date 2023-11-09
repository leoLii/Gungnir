#pragma once

#include "common/base.h"

#include "driver/bgiBase/computePipeline.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

class BgiVulkanDevice;

using VkDescriptorSetLayoutVector = std::vector<VkDescriptorSetLayout>;

/// \class HgiVulkanComputePipeline
///
/// Vulkan implementation of HgiComputePipeline.
///
class BgiVulkanComputePipeline final : public BgiComputePipeline
{
public:
    BGIVULKAN_API
    ~BgiVulkanComputePipeline() override;

    /// Apply pipeline state
    BGIVULKAN_API
    void BindPipeline(VkCommandBuffer cb);

    /// Returns the vulkan pipeline layout
    BGIVULKAN_API
    VkPipelineLayout GetVulkanPipelineLayout() const;

    /// Returns the device used to create this object.
    BGIVULKAN_API
    BgiVulkanDevice* GetDevice() const;

    /// Returns the (writable) inflight bits of when this object was trashed.
    BGIVULKAN_API
    uint64_t & GetInflightBits();

protected:
    friend class BgiVulkan;

    BGIVULKAN_API
    BgiVulkanComputePipeline(
        BgiVulkanDevice* device,
        BgiComputePipelineDesc const& desc);

private:
    BgiVulkanComputePipeline() = delete;
    BgiVulkanComputePipeline & operator=(const BgiVulkanComputePipeline&) = delete;
    BgiVulkanComputePipeline(const BgiVulkanComputePipeline&) = delete;

    BgiVulkanDevice* _device;
    uint64_t _inflightBits;
    VkPipeline _vkPipeline;
    VkPipelineLayout _vkPipelineLayout;
    VkDescriptorSetLayoutVector _vkDescriptorSetLayouts;
};

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
