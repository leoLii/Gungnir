#ifndef GUNGNIR_DRIVER_VULKAN_RESOURCE_BINDINGS_H
#define GUNGNIR_DRIVER_VULKAN_RESOURCE_BINDINGS_H

#include "core/base.h"

#include "driver/bgiBase/resourceBindings.h"

#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkan.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

class BgiVulkanDevice;

///
/// \class HgiVulkanResourceBindings
///
/// Vulkan implementation of HgiResourceBindings.
///
///
class BgiVulkanResourceBindings final : public BgiResourceBindings
{
public:
    BGIVULKAN_API
    ~BgiVulkanResourceBindings() override;

    /// Binds the resources to GPU.
    BGIVULKAN_API
    void BindResources(
        VkCommandBuffer cb,
        VkPipelineBindPoint bindPoint,
        VkPipelineLayout layout);

    /// Returns the device used to create this object.
    BGIVULKAN_API
    BgiVulkanDevice* GetDevice() const;

    /// Returns the (writable) inflight bits of when this object was trashed.
    BGIVULKAN_API
    uint64_t & GetInflightBits();

protected:
    friend class BgiVulkan;

    BGIVULKAN_API
    BgiVulkanResourceBindings(
        BgiVulkanDevice* device,
        BgiResourceBindingsDesc const& desc);

private:
    BgiVulkanResourceBindings() = delete;
    BgiVulkanResourceBindings & operator=(const BgiVulkanResourceBindings&) = delete;
    BgiVulkanResourceBindings(const BgiVulkanResourceBindings&) = delete;

    BgiVulkanDevice* _device;
    uint64_t _inflightBits;

    VkDescriptorPool _vkDescriptorPool;
    VkDescriptorSetLayout _vkDescriptorSetLayout;
    VkDescriptorSet _vkDescriptorSet;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_VULKAN_RESOURCE_BINDINGS_H
