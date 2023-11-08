#pragma once

#include "core/base.h"
#include "core/math/math.h"

#include "driver/bgiBase/graphicsCmdsDesc.h"
#include "driver/bgiBase/graphicsPipeline.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

using namespace math;

namespace driver {

class BgiVulkanDevice;

using VkDescriptorSetLayoutVector = std::vector<VkDescriptorSetLayout>;
using VkClearValueVector = std::vector<VkClearValue>;

/// \class HgiVulkanPipeline
///
/// Vulkan implementation of HgiGraphicsPipeline.
///
class BgiVulkanGraphicsPipeline final : public BgiGraphicsPipeline
{
public:
    BGIVULKAN_API
    ~BgiVulkanGraphicsPipeline() override;

    /// Apply pipeline state
    BGIVULKAN_API
    void BindPipeline(VkCommandBuffer cb);

    /// Returns the device used to create this object.
    BGIVULKAN_API
    BgiVulkanDevice* GetDevice() const;

    /// Returns the vulkan pipeline layout
    BGIVULKAN_API
    VkPipelineLayout GetVulkanPipelineLayout() const;

    /// Returns the vulkan render pass
    BGIVULKAN_API
    VkRenderPass GetVulkanRenderPass() const;

    /// Returns the vulkan frame buffer, creating it if needed.
    BGIVULKAN_API
    VkFramebuffer AcquireVulkanFramebuffer(
        BgiGraphicsCmdsDesc const& gfxDesc,
        Vector2i* dimensions);

    /// Returns the clear values for each color and depth attachment.
    BGIVULKAN_API
    VkClearValueVector const& GetClearValues() const;

    /// Returns the (writable) inflight bits of when this object was trashed.
    BGIVULKAN_API
    uint64_t & GetInflightBits();

protected:
    friend class BgiVulkan;

    BGIVULKAN_API
    BgiVulkanGraphicsPipeline(
        BgiVulkanDevice* device,
        BgiGraphicsPipelineDesc const& desc);

private:
    BgiVulkanGraphicsPipeline() = delete;
    BgiVulkanGraphicsPipeline & operator=(const BgiVulkanGraphicsPipeline&) = delete;
    BgiVulkanGraphicsPipeline(const BgiVulkanGraphicsPipeline&) = delete;

    void _CreateRenderPass();

    struct BgiVulkan_Framebuffer {
        Vector2i dimensions;
        BgiGraphicsCmdsDesc desc;
        VkFramebuffer vkFramebuffer;
    };

    BgiVulkanDevice* _device;
    uint64_t _inflightBits;
    VkPipeline _vkPipeline;
    VkRenderPass _vkRenderPass;
    VkPipelineLayout _vkPipelineLayout;
    VkDescriptorSetLayoutVector _vkDescriptorSetLayouts;
    VkClearValueVector _vkClearValues;

    std::vector<BgiVulkan_Framebuffer> _framebuffers;
};

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
