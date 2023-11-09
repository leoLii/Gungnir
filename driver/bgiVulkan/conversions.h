#pragma once

#include "common/base.h"

#include "driver/bgiBase/enums.h"
#include "driver/bgiBase/types.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

///
/// \class HgiVulkanConversions
///
/// Converts from Hgi types to Vulkan types.
///
class BgiVulkanConversions final
{
public:
    BGIVULKAN_API
    static VkFormat GetFormat(BgiFormat inFormat, bool depthFormat = false);

    BGIVULKAN_API
    static BgiFormat GetFormat(VkFormat inFormat);

    BGIVULKAN_API
    static VkImageAspectFlags GetImageAspectFlag(BgiTextureUsage usage);

    BGIVULKAN_API
    static VkImageUsageFlags GetTextureUsage(BgiTextureUsage tu);

    BGIVULKAN_API
    static VkFormatFeatureFlags GetFormatFeature(BgiTextureUsage tu);

    BGIVULKAN_API
    static VkAttachmentLoadOp GetLoadOp(BgiAttachmentLoadOp op);

    BGIVULKAN_API
    static VkAttachmentStoreOp GetStoreOp(BgiAttachmentStoreOp op);

    BGIVULKAN_API
    static VkSampleCountFlagBits GetSampleCount(BgiSampleCount sc);

    BGIVULKAN_API
    static VkShaderStageFlags GetShaderStages(BgiShaderStage ss);

    BGIVULKAN_API
    static VkBufferUsageFlags GetBufferUsage(BgiBufferUsage bu);

    BGIVULKAN_API
    static VkCullModeFlags GetCullMode(BgiCullMode cm);

    BGIVULKAN_API
    static VkPolygonMode GetPolygonMode(BgiPolygonMode pm);

    BGIVULKAN_API
    static VkFrontFace GetWinding(BgiWinding wd);

    BGIVULKAN_API
    static VkDescriptorType GetDescriptorType(BgiBindResourceType rt);

    BGIVULKAN_API
    static VkBlendFactor GetBlendFactor(BgiBlendFactor bf);

    BGIVULKAN_API
    static VkBlendOp GetBlendEquation(BgiBlendOp bo);

    BGIVULKAN_API
    static VkCompareOp GetDepthCompareFunction(BgiCompareFunction cf);

    BGIVULKAN_API
    static VkImageType GetTextureType(BgiTextureType tt);

    BGIVULKAN_API
    static VkImageViewType GetTextureViewType(BgiTextureType tt);

    BGIVULKAN_API
    static VkSamplerAddressMode GetSamplerAddressMode(BgiSamplerAddressMode a);

    BGIVULKAN_API
    static VkFilter GetMinMagFilter(BgiSamplerFilter mf);

    BGIVULKAN_API
    static VkSamplerMipmapMode GetMipFilter(BgiMipFilter mf);
    
    BGIVULKAN_API
    static VkBorderColor GetBorderColor(BgiBorderColor bc);

    BGIVULKAN_API
    static VkComponentSwizzle GetComponentSwizzle(BgiComponentSwizzle cs);

    BGIVULKAN_API
    static VkPrimitiveTopology GetPrimitiveType(BgiPrimitiveType pt);

    BGIVULKAN_API
    static std::string GetImageLayoutFormatQualifier(BgiFormat inFormat);
};

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
