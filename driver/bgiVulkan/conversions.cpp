#include "core/utils/diagnostic.h"

#include "driver/bgiVulkan/conversions.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

static const uint32_t
_LoadOpTable[BgiAttachmentLoadOpCount][2] =
{
    {BgiAttachmentLoadOpDontCare, VK_ATTACHMENT_LOAD_OP_DONT_CARE},
    {BgiAttachmentLoadOpClear,    VK_ATTACHMENT_LOAD_OP_CLEAR},
    {BgiAttachmentLoadOpLoad,     VK_ATTACHMENT_LOAD_OP_LOAD}
};
static_assert(BgiAttachmentLoadOpCount==3, "");

static const uint32_t
_StoreOpTable[BgiAttachmentStoreOpCount][2] =
{
    {BgiAttachmentStoreOpDontCare, VK_ATTACHMENT_STORE_OP_DONT_CARE},
    {BgiAttachmentStoreOpStore,    VK_ATTACHMENT_STORE_OP_STORE}
};
static_assert(BgiAttachmentStoreOpCount==2, "");

static const uint32_t
_FormatTable[BgiFormatCount][2] =
{
    // HGI FORMAT              VK FORMAT
    {BgiFormatUNorm8,         VK_FORMAT_R8_UNORM},
    {BgiFormatUNorm8Vec2,     VK_FORMAT_R8G8_UNORM},
    // HgiFormatUNorm8Vec3, VK_FORMAT_R8G8B8_UNORM // not supported by HgiFormat
    {BgiFormatUNorm8Vec4,     VK_FORMAT_R8G8B8A8_UNORM},
    {BgiFormatSNorm8,         VK_FORMAT_R8_SNORM},
    {BgiFormatSNorm8Vec2,     VK_FORMAT_R8G8_SNORM},
    // HgiFormatSNorm8Vec3, VK_FORMAT_R8G8B8_SNORM // not supported by HgiFormat
    {BgiFormatSNorm8Vec4,     VK_FORMAT_R8G8B8A8_SNORM},
    {BgiFormatFloat16,        VK_FORMAT_R16_SFLOAT},
    {BgiFormatFloat16Vec2,    VK_FORMAT_R16G16_SFLOAT},
    {BgiFormatFloat16Vec3,    VK_FORMAT_R16G16B16_SFLOAT},
    {BgiFormatFloat16Vec4,    VK_FORMAT_R16G16B16A16_SFLOAT},
    {BgiFormatFloat32,        VK_FORMAT_R32_SFLOAT},
    {BgiFormatFloat32Vec2,    VK_FORMAT_R32G32_SFLOAT},
    {BgiFormatFloat32Vec3,    VK_FORMAT_R32G32B32_SFLOAT},
    {BgiFormatFloat32Vec4,    VK_FORMAT_R32G32B32A32_SFLOAT},
    {BgiFormatInt16,          VK_FORMAT_R16_SINT},
    {BgiFormatInt16Vec2,      VK_FORMAT_R16G16_SINT},
    {BgiFormatInt16Vec3,      VK_FORMAT_R16G16B16_SINT},
    {BgiFormatInt16Vec4,      VK_FORMAT_R16G16B16A16_SINT},
    {BgiFormatUInt16,         VK_FORMAT_R16_UINT},
    {BgiFormatUInt16Vec2,     VK_FORMAT_R16G16_UINT},
    {BgiFormatUInt16Vec3,     VK_FORMAT_R16G16B16_UINT},
    {BgiFormatUInt16Vec4,     VK_FORMAT_R16G16B16A16_UINT},
    {BgiFormatInt32,          VK_FORMAT_R32_SINT},
    {BgiFormatInt32Vec2,      VK_FORMAT_R32G32_SINT},
    {BgiFormatInt32Vec3,      VK_FORMAT_R32G32B32_SINT},
    {BgiFormatInt32Vec4,      VK_FORMAT_R32G32B32A32_SINT},
    {BgiFormatUNorm8Vec4srgb, VK_FORMAT_R8G8B8A8_SRGB},
    {BgiFormatBC6FloatVec3,   VK_FORMAT_BC6H_SFLOAT_BLOCK},
    {BgiFormatBC6UFloatVec3,  VK_FORMAT_BC6H_UFLOAT_BLOCK},
    {BgiFormatBC7UNorm8Vec4,  VK_FORMAT_BC7_UNORM_BLOCK},
    {BgiFormatBC7UNorm8Vec4srgb, VK_FORMAT_BC7_SRGB_BLOCK},
    {BgiFormatBC1UNorm8Vec4,  VK_FORMAT_BC1_RGBA_UNORM_BLOCK},
    {BgiFormatBC3UNorm8Vec4,  VK_FORMAT_BC3_UNORM_BLOCK},
    {BgiFormatFloat32UInt8,   VK_FORMAT_D32_SFLOAT_S8_UINT},
    {BgiFormatPackedInt1010102, VK_FORMAT_A2B10G10R10_SNORM_PACK32},
};

// A few random format validations to make sure the table above stays in sync
// with the HgiFormat table.
constexpr bool _CompileTimeValidateBgiFormatTable() {
    return (BgiFormatCount==35 &&
            BgiFormatUNorm8 == 0 &&
            BgiFormatFloat16Vec4 == 9 &&
            BgiFormatFloat32Vec4 == 13 &&
            BgiFormatUInt16Vec4 == 21 &&
            BgiFormatUNorm8Vec4srgb == 26 &&
            BgiFormatBC3UNorm8Vec4 == 32) ? true : false;
}

static_assert(_CompileTimeValidateBgiFormatTable(), 
              "_FormatDesc array out of sync with HgiFormat enum");


static const uint32_t
_SampleCountTable[][2] =
{
    {BgiSampleCount1,  VK_SAMPLE_COUNT_1_BIT},
    {BgiSampleCount2,  VK_SAMPLE_COUNT_2_BIT},
    {BgiSampleCount4,  VK_SAMPLE_COUNT_4_BIT},
    {BgiSampleCount8,  VK_SAMPLE_COUNT_8_BIT},
    {BgiSampleCount16, VK_SAMPLE_COUNT_16_BIT}
};
static_assert(BgiSampleCountEnd==17, "");

static const uint32_t
_ShaderStageTable[][2] =
{
    {BgiShaderStageVertex,              VK_SHADER_STAGE_VERTEX_BIT},
    {BgiShaderStageFragment,            VK_SHADER_STAGE_FRAGMENT_BIT},
    {BgiShaderStageCompute,             VK_SHADER_STAGE_COMPUTE_BIT},
    {BgiShaderStageTessellationControl, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT},
    {BgiShaderStageTessellationEval,    VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
    {BgiShaderStageGeometry,            VK_SHADER_STAGE_GEOMETRY_BIT},
};
static_assert(BgiShaderStageCustomBitsBegin == 1 << 8, "");

static const uint32_t
_TextureUsageTable[][2] =
{
    {BgiTextureUsageBitsColorTarget,   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT},
    {BgiTextureUsageBitsDepthTarget,   VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT},
    {BgiTextureUsageBitsStencilTarget, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT},
    {BgiTextureUsageBitsShaderRead,    VK_IMAGE_USAGE_SAMPLED_BIT},
    {BgiTextureUsageBitsShaderWrite,   VK_IMAGE_USAGE_STORAGE_BIT}
};
static_assert(BgiTextureUsageCustomBitsBegin == 1 << 5, "");

static const uint32_t
_FormatFeatureTable[][2] =
{
    {BgiTextureUsageBitsColorTarget,   VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT},
    {BgiTextureUsageBitsDepthTarget,   VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT},
    {BgiTextureUsageBitsStencilTarget, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT},
    {BgiTextureUsageBitsShaderRead,    VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT},
    {BgiTextureUsageBitsShaderWrite,   VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT},
};
static_assert(BgiTextureUsageCustomBitsBegin == 1 << 5, "");

static const uint32_t
_BufferUsageTable[][2] =
{
    {BgiBufferUsageUniform, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},
    {BgiBufferUsageIndex32, VK_BUFFER_USAGE_INDEX_BUFFER_BIT},
    {BgiBufferUsageVertex,  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT},
    {BgiBufferUsageStorage, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT},
};
static_assert(BgiBufferUsageCustomBitsBegin == 1 << 4, "");

static const uint32_t
_CullModeTable[BgiCullModeCount][2] =
{
    {BgiCullModeNone,         VK_CULL_MODE_NONE},
    {BgiCullModeFront,        VK_CULL_MODE_FRONT_BIT},
    {BgiCullModeBack,         VK_CULL_MODE_BACK_BIT},
    {BgiCullModeFrontAndBack, VK_CULL_MODE_FRONT_AND_BACK}
};
static_assert(BgiCullModeCount==4, "");

static const uint32_t
_PolygonModeTable[BgiPolygonModeCount][2] =
{
    {BgiPolygonModeFill,  VK_POLYGON_MODE_FILL},
    {BgiPolygonModeLine,  VK_POLYGON_MODE_LINE},
    {BgiPolygonModePoint, VK_POLYGON_MODE_POINT}
};
static_assert(BgiPolygonModeCount==3, "");

static const uint32_t
_WindingTable[BgiWindingCount][2] =
{
    {BgiWindingClockwise,        VK_FRONT_FACE_CLOCKWISE},
    {BgiWindingCounterClockwise, VK_FRONT_FACE_COUNTER_CLOCKWISE}
};
static_assert(BgiWindingCount==2, "");

static const uint32_t
_BindResourceTypeTable[BgiBindResourceTypeCount][2] =
{
    {BgiBindResourceTypeSampler,              VK_DESCRIPTOR_TYPE_SAMPLER},
    {BgiBindResourceTypeSampledImage,         VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE},
    {BgiBindResourceTypeCombinedSamplerImage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER},
    {BgiBindResourceTypeStorageImage,         VK_DESCRIPTOR_TYPE_STORAGE_IMAGE},
    {BgiBindResourceTypeUniformBuffer,        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER},
    {BgiBindResourceTypeStorageBuffer,        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER},
    {BgiBindResourceTypeTessFactors,          VK_DESCRIPTOR_TYPE_STORAGE_BUFFER},
};
static_assert(BgiBindResourceTypeCount==7, "");

static const uint32_t
_blendEquationTable[BgiBlendOpCount][2] =
{
    {BgiBlendOpAdd,             VK_BLEND_OP_ADD},
    {BgiBlendOpSubtract,        VK_BLEND_OP_SUBTRACT},
    {BgiBlendOpReverseSubtract, VK_BLEND_OP_REVERSE_SUBTRACT},
    {BgiBlendOpMin,             VK_BLEND_OP_MIN},
    {BgiBlendOpMax,             VK_BLEND_OP_MAX},
};
static_assert(BgiBlendOpCount==5, "");

static const uint32_t
_blendFactorTable[BgiBlendFactorCount][2] =
{
    {BgiBlendFactorZero,                  VK_BLEND_FACTOR_ZERO},
    {BgiBlendFactorOne,                   VK_BLEND_FACTOR_ONE},
    {BgiBlendFactorSrcColor,              VK_BLEND_FACTOR_SRC_COLOR},
    {BgiBlendFactorOneMinusSrcColor,      VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR},
    {BgiBlendFactorDstColor,              VK_BLEND_FACTOR_DST_COLOR},
    {BgiBlendFactorOneMinusDstColor,      VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR},
    {BgiBlendFactorSrcAlpha,              VK_BLEND_FACTOR_SRC_ALPHA},
    {BgiBlendFactorOneMinusSrcAlpha,      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA},
    {BgiBlendFactorDstAlpha,              VK_BLEND_FACTOR_DST_ALPHA},
    {BgiBlendFactorOneMinusDstAlpha,      VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA},
    {BgiBlendFactorConstantColor,         VK_BLEND_FACTOR_CONSTANT_COLOR},
    {BgiBlendFactorOneMinusConstantColor, VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR},
    {BgiBlendFactorConstantAlpha,         VK_BLEND_FACTOR_CONSTANT_ALPHA},
    {BgiBlendFactorOneMinusConstantAlpha, VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA},
    {BgiBlendFactorSrcAlphaSaturate,      VK_BLEND_FACTOR_SRC_ALPHA_SATURATE},
    {BgiBlendFactorSrc1Color,             VK_BLEND_FACTOR_SRC1_COLOR},
    {BgiBlendFactorOneMinusSrc1Color,     VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR},
    {BgiBlendFactorSrc1Alpha,             VK_BLEND_FACTOR_SRC1_ALPHA},
    {BgiBlendFactorOneMinusSrc1Alpha,     VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA},
};
static_assert(BgiBlendFactorCount==19, "");

static const uint32_t
_CompareOpTable[BgiCompareFunctionCount][2] =
{
    {BgiCompareFunctionNever,    VK_COMPARE_OP_NEVER},
    {BgiCompareFunctionLess,     VK_COMPARE_OP_LESS},
    {BgiCompareFunctionEqual,    VK_COMPARE_OP_EQUAL},
    {BgiCompareFunctionLEqual,   VK_COMPARE_OP_LESS_OR_EQUAL},
    {BgiCompareFunctionGreater,  VK_COMPARE_OP_GREATER},
    {BgiCompareFunctionNotEqual, VK_COMPARE_OP_NOT_EQUAL},
    {BgiCompareFunctionGEqual,   VK_COMPARE_OP_GREATER_OR_EQUAL},
    {BgiCompareFunctionAlways,   VK_COMPARE_OP_ALWAYS}
};
static_assert(BgiCompareFunctionCount==8, "");

static const uint32_t
_textureTypeTable[BgiTextureTypeCount][2] =
{
    {BgiTextureType1D,      VK_IMAGE_TYPE_1D},
    {BgiTextureType2D,      VK_IMAGE_TYPE_2D},
    {BgiTextureType3D,      VK_IMAGE_TYPE_3D},
    {BgiTextureType1DArray, VK_IMAGE_TYPE_1D},
    {BgiTextureType2DArray, VK_IMAGE_TYPE_2D}
};
static_assert(BgiTextureTypeCount==5, "");

static const uint32_t
_textureViewTypeTable[BgiTextureTypeCount][2] =
{
    {BgiTextureType1D,      VK_IMAGE_VIEW_TYPE_1D},
    {BgiTextureType2D,      VK_IMAGE_VIEW_TYPE_2D},
    {BgiTextureType3D,      VK_IMAGE_VIEW_TYPE_3D},
    {BgiTextureType1DArray, VK_IMAGE_VIEW_TYPE_1D_ARRAY},
    {BgiTextureType2DArray, VK_IMAGE_VIEW_TYPE_2D_ARRAY}
};
static_assert(BgiTextureTypeCount==5, "");

static const uint32_t
_samplerAddressModeTable[BgiSamplerAddressModeCount][2] =
{
    {BgiSamplerAddressModeClampToEdge,        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE},
    {BgiSamplerAddressModeMirrorClampToEdge,  VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE},
    {BgiSamplerAddressModeRepeat,             VK_SAMPLER_ADDRESS_MODE_REPEAT},
    {BgiSamplerAddressModeMirrorRepeat,       VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT},
    {BgiSamplerAddressModeClampToBorderColor, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER}
};
static_assert(BgiSamplerAddressModeCount==5, "");

static const uint32_t
_samplerFilterTable[BgiSamplerFilterCount][2] =
{
    {BgiSamplerFilterNearest, VK_FILTER_NEAREST},
    {BgiSamplerFilterLinear,  VK_FILTER_LINEAR}
};
static_assert(BgiSamplerFilterCount==2, "");

static const uint32_t
_mipFilterTable[BgiMipFilterCount][2] =
{
    {BgiMipFilterNotMipmapped, VK_SAMPLER_MIPMAP_MODE_NEAREST /*unused*/},
    {BgiMipFilterNearest,      VK_SAMPLER_MIPMAP_MODE_NEAREST},
    {BgiMipFilterLinear,       VK_SAMPLER_MIPMAP_MODE_LINEAR}
};
static_assert(BgiMipFilterCount==3, "");

static const uint32_t
_borderColorTable[BgiBorderColorCount][2] =
{
    {BgiBorderColorTransparentBlack, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK},
    {BgiBorderColorOpaqueBlack,      VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK},
    {BgiBorderColorOpaqueWhite,      VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE}
};
static_assert(BgiBorderColorCount==3, "");

static const uint32_t
_componentSwizzleTable[BgiComponentSwizzleCount][2] =
{
    {BgiComponentSwizzleZero, VK_COMPONENT_SWIZZLE_ZERO},
    {BgiComponentSwizzleOne,  VK_COMPONENT_SWIZZLE_ONE},
    {BgiComponentSwizzleR,    VK_COMPONENT_SWIZZLE_R},
    {BgiComponentSwizzleG,    VK_COMPONENT_SWIZZLE_G},
    {BgiComponentSwizzleB,    VK_COMPONENT_SWIZZLE_B},
    {BgiComponentSwizzleA,    VK_COMPONENT_SWIZZLE_A}
};
static_assert(BgiComponentSwizzleCount==6, "");

static const uint32_t
_primitiveTypeTable[BgiPrimitiveTypeCount][2] =
{
    {BgiPrimitiveTypePointList,    VK_PRIMITIVE_TOPOLOGY_POINT_LIST},
    {BgiPrimitiveTypeLineList,     VK_PRIMITIVE_TOPOLOGY_LINE_LIST},
    {BgiPrimitiveTypeLineStrip,    VK_PRIMITIVE_TOPOLOGY_LINE_STRIP},
    {BgiPrimitiveTypeTriangleList, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST},
    {BgiPrimitiveTypePatchList,    VK_PRIMITIVE_TOPOLOGY_PATCH_LIST},
    {BgiPrimitiveTypeLineListWithAdjacency,
                            VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY}
};
static_assert(BgiPrimitiveTypeCount==6, "");

static const std::string
_imageLayoutFormatTable[BgiFormatCount][2] =
{ 
    {"BgiFormatUNorm8",            "r8"},
    {"BgiFormatUNorm8Vec2",        "rg8"},
    {"BgiFormatUNorm8Vec4",        "rgba8"},
    {"BgiFormatSNorm8",            "r8_snorm"},
    {"BgiFormatSNorm8Vec2",        "rg8_snorm"},
    {"BgiFormatSNorm8Vec4",        "rgba8_snorm"},
    {"BgiFormatFloat16",           "r16f"},
    {"BgiFormatFloat16Vec2",       "rg16f"},
    {"BgiFormatFloat16Vec3",       ""},
    {"BgiFormatFloat16Vec4",       "rgba16f"},
    {"BgiFormatFloat32",           "r32f"},
    {"BgiFormatFloat32Vec2",       "rg32f"},
    {"BgiFormatFloat32Vec3",       ""},
    {"BgiFormatFloat32Vec4",       "rgba32f" },
    {"BgiFormatInt16",             "r16i"},
    {"BgiFormatInt16Vec2",         "rg16i"},
    {"BgiFormatInt16Vec3",         ""},
    {"BgiFormatInt16Vec4",         "rgba16i"},
    {"BgiFormatUInt16",            "r16ui"},
    {"BgiFormatUInt16Vec2",        "rg16ui"},
    {"BgiFormatUInt16Vec3",        ""},
    {"BgiFormatUInt16Vec4",        "rgba16ui"},
    {"BgiFormatInt32",             "r32i"},
    {"BgiFormatInt32Vec2",         "rg32i"},
    {"BgiFormatInt32Vec3",         ""},
    {"BgiFormatInt32Vec4",         "rgba32i"},
    {"BgiFormatUNorm8Vec4srgb",    ""},
    {"BgiFormatBC6FloatVec3",      ""},
    {"BgiFormatBC6UFloatVec3",     ""},
    {"BgiFormatBC7UNorm8Vec4",     ""},
    {"BgiFormatBC7UNorm8Vec4srgb", ""},
    {"BgiFormatBC1UNorm8Vec4",     ""},
    {"BgiFormatBC3UNorm8Vec4",     ""},
    {"BgiFormatFloat32UInt8",      ""},
    {"BgiFormatPackedInt1010102",  ""},
};

VkFormat
BgiVulkanConversions::GetFormat(BgiFormat inFormat, bool depthFormat)
{
    if (!UTILS_VERIFY(inFormat!=BgiFormatInvalid)) {
        return VK_FORMAT_UNDEFINED;
    }

    VkFormat vkFormat = VkFormat(_FormatTable[inFormat][1]);

    // Special case for float32 depth format not properly handled by
    // _FormatTable
    if (depthFormat && inFormat == BgiFormatFloat32) {
        vkFormat = VK_FORMAT_D32_SFLOAT;
    }

    return vkFormat;
}

BgiFormat
BgiVulkanConversions::GetFormat(VkFormat inFormat)
{
    if (!UTILS_VERIFY(inFormat!=VK_FORMAT_UNDEFINED)) {
        return BgiFormatInvalid;
    }

    // While HdFormat/HgiFormat do not support BGRA channel ordering it may
    // be used for the native window swapchain on some platforms.
    if (inFormat == VK_FORMAT_B8G8R8A8_UNORM) {
        return BgiFormatUNorm8Vec4;
    }

    for (auto const& f : _FormatTable) {
        if (f[1] == inFormat) return BgiFormat(f[0]);
    }

    UTILS_CODING_ERROR("Missing format table entry");
    return BgiFormatInvalid;
}

VkImageAspectFlags
BgiVulkanConversions::GetImageAspectFlag(BgiTextureUsage usage)
{
    VkImageAspectFlags result = VK_IMAGE_ASPECT_COLOR_BIT;

    if (usage & BgiTextureUsageBitsDepthTarget && 
        usage & BgiTextureUsageBitsStencilTarget) {
        result = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    } else if (usage & BgiTextureUsageBitsDepthTarget) {
        result = VK_IMAGE_ASPECT_DEPTH_BIT;
    } else if (usage & BgiTextureUsageBitsStencilTarget) {
        result = VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    return result;
}

VkImageUsageFlags
BgiVulkanConversions::GetTextureUsage(BgiTextureUsage tu)
{
    VkImageUsageFlags vkFlags = 0;
    for (const auto& f : _TextureUsageTable) {
        if (tu & f[0]) vkFlags |= f[1];
    }

    if (vkFlags==0) {
        UTILS_CODING_ERROR("Missing texture usage table entry");
    }
    return vkFlags;
}

VkFormatFeatureFlags
BgiVulkanConversions::GetFormatFeature(BgiTextureUsage tu)
{
    VkFormatFeatureFlags vkFlags = 0;
    for (const auto& f : _FormatFeatureTable) {
        if (tu & f[0]) vkFlags |= f[1];
    }

    if (vkFlags==0) {
        UTILS_CODING_ERROR("Missing texture usage table entry");
    }
    return vkFlags;
}

VkAttachmentLoadOp
BgiVulkanConversions::GetLoadOp(BgiAttachmentLoadOp op)
{
    return VkAttachmentLoadOp(_LoadOpTable[op][1]);
}

VkAttachmentStoreOp
BgiVulkanConversions::GetStoreOp(BgiAttachmentStoreOp op)
{
    return VkAttachmentStoreOp(_StoreOpTable[op][1]);
}

VkSampleCountFlagBits
BgiVulkanConversions::GetSampleCount(BgiSampleCount sc)
{
    for (auto const& s : _SampleCountTable) {
        if (s[0] == sc) return VkSampleCountFlagBits(s[1]);
    }

    UTILS_CODING_ERROR("Missing Sample table entry");
    return VK_SAMPLE_COUNT_1_BIT;
}

VkShaderStageFlags
BgiVulkanConversions::GetShaderStages(BgiShaderStage ss)
{
    VkShaderStageFlags vkFlags = 0;
    for (const auto& f : _ShaderStageTable) {
        if (ss & f[0]) vkFlags |= f[1];
    }

    if (vkFlags==0) {
        UTILS_CODING_ERROR("Missing shader stage table entry");
    }
    return vkFlags;
}

VkBufferUsageFlags
BgiVulkanConversions::GetBufferUsage(BgiBufferUsage bu)
{
    VkBufferUsageFlags vkFlags = 0;
    for (const auto& f : _BufferUsageTable) {
        if (bu & f[0]) vkFlags |= f[1];
    }

    if (vkFlags==0) {
        UTILS_CODING_ERROR("Missing buffer usage table entry");
    }
    return vkFlags;
}

VkCullModeFlags
BgiVulkanConversions::GetCullMode(BgiCullMode cm)
{
    return VkCullModeFlags(_CullModeTable[cm][1]);
}

VkPolygonMode
BgiVulkanConversions::GetPolygonMode(BgiPolygonMode pm)
{
    return VkPolygonMode(_PolygonModeTable[pm][1]);
}

VkFrontFace
BgiVulkanConversions::GetWinding(BgiWinding wd)
{
    return VkFrontFace(_WindingTable[wd][1]);
}

VkDescriptorType
BgiVulkanConversions::GetDescriptorType(BgiBindResourceType rt)
{
    return VkDescriptorType(_BindResourceTypeTable[rt][1]);
}

VkBlendFactor
BgiVulkanConversions::GetBlendFactor(BgiBlendFactor bf)
{
    return VkBlendFactor(_blendFactorTable[bf][1]);
}

VkBlendOp
BgiVulkanConversions::GetBlendEquation(BgiBlendOp bo)
{
    return VkBlendOp(_blendEquationTable[bo][1]);
}

VkCompareOp
BgiVulkanConversions::GetDepthCompareFunction(BgiCompareFunction cf)
{
    return VkCompareOp(_CompareOpTable[cf][1]);
}

VkImageType
BgiVulkanConversions::GetTextureType(BgiTextureType tt)
{
    return VkImageType(_textureTypeTable[tt][1]);
}

VkImageViewType
BgiVulkanConversions::GetTextureViewType(BgiTextureType tt)
{
    return VkImageViewType(_textureViewTypeTable[tt][1]);
}

VkSamplerAddressMode
BgiVulkanConversions::GetSamplerAddressMode(BgiSamplerAddressMode a)
{
    return VkSamplerAddressMode(_samplerAddressModeTable[a][1]);
}

VkFilter
BgiVulkanConversions::GetMinMagFilter(BgiSamplerFilter mf)
{
    return VkFilter(_samplerFilterTable[mf][1]);
}

VkSamplerMipmapMode
BgiVulkanConversions::GetMipFilter(BgiMipFilter mf)
{
    return VkSamplerMipmapMode(_mipFilterTable[mf][1]);
}

VkBorderColor
BgiVulkanConversions::GetBorderColor(BgiBorderColor bc)
{
    return VkBorderColor(_borderColorTable[bc][1]);
}

VkComponentSwizzle
BgiVulkanConversions::GetComponentSwizzle(BgiComponentSwizzle cs)
{
    return VkComponentSwizzle(_componentSwizzleTable[cs][1]);
}

VkPrimitiveTopology
BgiVulkanConversions::GetPrimitiveType(BgiPrimitiveType pt)
{
    return VkPrimitiveTopology(_primitiveTypeTable[pt][1]);
}

std::string
BgiVulkanConversions::GetImageLayoutFormatQualifier(BgiFormat inFormat)
{
    const std::string layoutQualifier = _imageLayoutFormatTable[inFormat][1];
    if (layoutQualifier.empty()) {
        UTILS_WARN("Given HgiFormat is not a supported image unit format, "
                "defaulting to rgba16f");
        return _imageLayoutFormatTable[9][1];
    }
    return layoutQualifier;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
