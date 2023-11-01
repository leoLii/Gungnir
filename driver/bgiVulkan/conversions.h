#ifndef GUNGNIR_DRIVER_VULKAN_CONVERSATIONS_H
#define GUNGNIR_DRIVER_VULKAN_CONVERSATIONS_H

#include "core/base.h"

#include "driver/bgiBase/enums.h"
#include "driver/bgiBase/types.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkan.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

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

    HGIVULKAN_API
    static VkImageUsageFlags GetTextureUsage(BgiTextureUsage tu);

    HGIVULKAN_API
    static VkFormatFeatureFlags GetFormatFeature(BgiTextureUsage tu);

    HGIVULKAN_API
    static VkAttachmentLoadOp GetLoadOp(BgiAttachmentLoadOp op);

    HGIVULKAN_API
    static VkAttachmentStoreOp GetStoreOp(BgiAttachmentStoreOp op);

    HGIVULKAN_API
    static VkSampleCountFlagBits GetSampleCount(BgiSampleCount sc);

    HGIVULKAN_API
    static VkShaderStageFlags GetShaderStages(BgiShaderStage ss);

    HGIVULKAN_API
    static VkBufferUsageFlags GetBufferUsage(BgiBufferUsage bu);

    HGIVULKAN_API
    static VkCullModeFlags GetCullMode(BgiCullMode cm);

    HGIVULKAN_API
    static VkPolygonMode GetPolygonMode(BgiPolygonMode pm);

    HGIVULKAN_API
    static VkFrontFace GetWinding(BgiWinding wd);

    HGIVULKAN_API
    static VkDescriptorType GetDescriptorType(BgiBindResourceType rt);

    HGIVULKAN_API
    static VkBlendFactor GetBlendFactor(BgiBlendFactor bf);

    HGIVULKAN_API
    static VkBlendOp GetBlendEquation(BgiBlendOp bo);

    HGIVULKAN_API
    static VkCompareOp GetDepthCompareFunction(BgiCompareFunction cf);

    HGIVULKAN_API
    static VkImageType GetTextureType(BgiTextureType tt);

    HGIVULKAN_API
    static VkImageViewType GetTextureViewType(BgiTextureType tt);

    HGIVULKAN_API
    static VkSamplerAddressMode GetSamplerAddressMode(BgiSamplerAddressMode a);

    HGIVULKAN_API
    static VkFilter GetMinMagFilter(BgiSamplerFilter mf);

    HGIVULKAN_API
    static VkSamplerMipmapMode GetMipFilter(BgiMipFilter mf);
    
    HGIVULKAN_API
    static VkBorderColor GetBorderColor(BgiBorderColor bc);

    HGIVULKAN_API
    static VkComponentSwizzle GetComponentSwizzle(BgiComponentSwizzle cs);

    HGIVULKAN_API
    static VkPrimitiveTopology GetPrimitiveType(BgiPrimitiveType pt);

    HGIVULKAN_API
    static std::string GetImageLayoutFormatQualifier(BgiFormat inFormat);
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_VULKAN_CONVERSATIONS_H
