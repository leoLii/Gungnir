#ifndef GUNGNIR_DRIVER_BASE_ENUMS_H
#define GUNGNIR_DRIVER_BASE_ENUMS_H

#include "base.h"

#include <cstdint>

GUNGNIR_NAMESPACE_OPEN_SCOPE

using BgiBits = uint32_t;


/// \enum BgiDeviceCapabilitiesBits
///
/// Describes what capabilities the requested device must have.
///
/// <ul>
/// <li>BgiDeviceCapabilitiesBitsPresentation:
///   The device must be capable of presenting graphics to screen</li>
/// <li>BgiDeviceCapabilitiesBitsBindlessBuffers:
///   THe device can access GPU buffers using bindless handles</li>
/// <li>BgiDeviceCapabilitiesBitsConcurrentDispatch:
///   The device can execute commands concurrently</li>
/// <li>BgiDeviceCapabilitiesBitsUnifiedMemory:
///   The device shares all GPU and CPU memory</li>
/// <li>BgiDeviceCapabilitiesBitsBuiltinBarycentrics:
///   The device can provide built-in barycentric coordinates</li>
/// <li>BgiDeviceCapabilitiesBitsShaderDrawParameters:
///   The device can provide additional built-in shader variables corresponding
///   to draw command parameters</li>
/// <li>BgiDeviceCapabilitiesBitsMultiDrawIndirect:
///   The device supports multiple primitive, indirect drawing</li>
/// <li>BgiDeviceCapabilitiesBitsBindlessTextures:
///   The device can access GPU textures using bindless handles</li>
/// <li>BgiDeviceCapabilitiesBitsShaderDoublePrecision:
///   The device supports double precision types in shaders</li>
/// <li>BgiDeviceCapabilitiesBitsDepthRangeMinusOnetoOne:
///   The device's clip space depth ranges from [-1,1]</li>
/// <li>BgiDeviceCapabilitiesBitsCppShaderPadding:
///   Use CPP padding for shader language structures</li>
/// <li>BgiDeviceCapabilitiesBitsConservativeRaster:
///   The device supports conservative rasterization</li>
/// <li>BgiDeviceCapabilitiesBitsStencilReadback:
///   Supports reading back the stencil buffer from GPU to CPU.</li>
/// <li>BgiDeviceCapabilitiesBitsCustomDepthRange:
///   The device supports setting a custom depth range.</li>
/// <li>BgiDeviceCapabilitiesBitsMetalTessellation:
///   Supports Metal tessellation shaders</li>
/// <li>BgiDeviceCapabilitiesBitsBasePrimitiveOffset:
///   The device requires workaround for base primitive offset</li>
/// <li>BgiDeviceCapabilitiesBitsPrimitiveIdEmulation:
///   The device requires workaround for primitive id</li>
/// <li>BgiDeviceCapabilitiesBitsIndirectCommandBuffers:
///   Indirect command buffers are supported</li>
/// </ul>
///
enum BgiDeviceCapabilitiesBits : BgiBits
{
    BgiDeviceCapabilitiesBitsPresentation            = 1 << 0,
    BgiDeviceCapabilitiesBitsBindlessBuffers         = 1 << 1,
    BgiDeviceCapabilitiesBitsConcurrentDispatch      = 1 << 2,
    BgiDeviceCapabilitiesBitsUnifiedMemory           = 1 << 3,
    BgiDeviceCapabilitiesBitsBuiltinBarycentrics     = 1 << 4,
    BgiDeviceCapabilitiesBitsShaderDrawParameters    = 1 << 5,
    BgiDeviceCapabilitiesBitsMultiDrawIndirect       = 1 << 6,
    BgiDeviceCapabilitiesBitsBindlessTextures        = 1 << 7,
    BgiDeviceCapabilitiesBitsShaderDoublePrecision   = 1 << 8,
    BgiDeviceCapabilitiesBitsDepthRangeMinusOnetoOne = 1 << 9,
    BgiDeviceCapabilitiesBitsCppShaderPadding        = 1 << 10,
    BgiDeviceCapabilitiesBitsConservativeRaster      = 1 << 11,
    BgiDeviceCapabilitiesBitsStencilReadback         = 1 << 12,
    BgiDeviceCapabilitiesBitsCustomDepthRange        = 1 << 13,
    BgiDeviceCapabilitiesBitsMetalTessellation       = 1 << 14,
    BgiDeviceCapabilitiesBitsBasePrimitiveOffset     = 1 << 15,
    BgiDeviceCapabilitiesBitsPrimitiveIdEmulation    = 1 << 16,
    BgiDeviceCapabilitiesBitsIndirectCommandBuffers  = 1 << 17,
};

using BgiDeviceCapabilities = BgiBits;

/// \enum BgiTextureType
///
/// Describes the kind of texture.
///
/// <ul>
/// <li>BgiTextureType1D:
///   A one-dimensional texture.</li>
/// <li>BgiTextureType2D:
///   A two-dimensional texture.</li>
/// <li>BgiTextureType3D:
///   A three-dimensional texture.</li>
/// <li>BgiTextureType1DArray:
///   An array of one-dimensional textures.</li>
/// <li>BgiTextureType2DArray:
///   An array of two-dimensional textures.</li>
/// </ul>
///
enum BgiTextureType
{
    BgiTextureType1D = 0,
    BgiTextureType2D,
    BgiTextureType3D,
    BgiTextureType1DArray,
    BgiTextureType2DArray,

    BgiTextureTypeCount
};

/// \enum BgiTextureUsageBits
///
/// Describes how the texture will be used. If a texture has multiple uses you
/// can combine multiple bits.
///
/// <ul>
/// <li>BgiTextureUsageBitsColorTarget:
///   The texture is a color attachment rendered into via a render pass.</li>
/// <li>BgiTextureUsageBitsDepthTarget:
///   The texture is a depth attachment rendered into via a render pass.</li>
/// <li>BgiTextureUsageBitsStencilTarget:
///   The texture is a stencil attachment rendered into via a render pass.</li>
/// <li>BgiTextureUsageBitsShaderRead:
///   The texture is sampled from in a shader (sampling)</li>
/// <li>BgiTextureUsageBitsShaderWrite:
///   The texture is written into from in a shader (image store)
///   When a texture is used as BgiBindResourceTypeStorageImage you must
///   add this flag (even if you only read from the image).</li>
///
/// <li>BgiTextureUsageCustomBitsBegin:
///   This bit (and any bit after) can be used to attached custom, backend
///   specific  bits to the usage bit. </li>
/// </ul>
///
enum BgiTextureUsageBits : BgiBits
{
    BgiTextureUsageBitsColorTarget   = 1 << 0,
    BgiTextureUsageBitsDepthTarget   = 1 << 1,
    BgiTextureUsageBitsStencilTarget = 1 << 2,
    BgiTextureUsageBitsShaderRead    = 1 << 3,
    BgiTextureUsageBitsShaderWrite   = 1 << 4,

    BgiTextureUsageCustomBitsBegin = 1 << 5,
};

using BgiTextureUsage = BgiBits;

/// \enum BgiSamplerAddressMode
///
/// Various modes used during sampling of a texture.
///
enum BgiSamplerAddressMode
{
    BgiSamplerAddressModeClampToEdge = 0,
    BgiSamplerAddressModeMirrorClampToEdge,
    BgiSamplerAddressModeRepeat,
    BgiSamplerAddressModeMirrorRepeat,
    BgiSamplerAddressModeClampToBorderColor,

    BgiSamplerAddressModeCount
};

/// \enum BgiSamplerFilter
///
/// Sampler filtering modes that determine the pixel value that is returned.
///
/// <ul>
/// <li>BgiSamplerFilterNearest:
///   Returns the value of a single mipmap level.</li>
/// <li>BgiSamplerFilterLinear:
///   Combines the values of multiple mipmap levels.</li>
/// </ul>
///
enum BgiSamplerFilter
{
    BgiSamplerFilterNearest = 0,
    BgiSamplerFilterLinear  = 1,

    BgiSamplerFilterCount
};

/// \enum BgiMipFilter
///
/// Sampler filtering modes that determine the pixel value that is returned.
///
/// <ul>
/// <li>BgiMipFilterNotMipmapped:
///   Texture is always sampled at mipmap level 0. (ie. max lod=0)</li>
/// <li>BgiMipFilterNearest:
///   Returns the value of a single mipmap level.</li>
/// <li>BgiMipFilterLinear:
///   Linear interpolates the values of up to two mipmap levels.</li>
/// </ul>
///
enum BgiMipFilter
{
    BgiMipFilterNotMipmapped = 0,
    BgiMipFilterNearest      = 1,
    BgiMipFilterLinear       = 2,

    BgiMipFilterCount
};

/// \enum BgiBorderColor
///
/// Border color to use for clamped texture values.
///
/// <ul>
/// <li>BgiBorderColorTransparentBlack</li>
/// <li>BgiBorderColorOpaqueBlack</li>
/// <li>BgiBorderColorOpaqueWhite</li>
/// </ul>
///
enum BgiBorderColor
{
    BgiBorderColorTransparentBlack = 0,
    BgiBorderColorOpaqueBlack      = 1,
    BgiBorderColorOpaqueWhite      = 2,

    BgiBorderColorCount
};

/// \enum BgiSampleCount
///
/// Sample count for multi-sampling
///
enum BgiSampleCount
{
    BgiSampleCount1  = 1,
    BgiSampleCount2  = 2,
    BgiSampleCount4  = 4,
    BgiSampleCount8  = 8,
    BgiSampleCount16 = 16,

    BgiSampleCountEnd
};

/// \enum BgiAttachmentLoadOp
///
/// Describes what will happen to the attachment pixel data prior to rendering.
///
/// <ul>
/// <li>BgiAttachmentLoadOpDontCare:
///   All pixels are rendered to. Pixel data in render target starts undefined.</li>
/// <li>BgiAttachmentLoadOpClear:
///   The attachment  pixel data is cleared to a specified color value.</li>
/// <li>BgiAttachmentLoadOpLoad:
///   Previous pixel data is loaded into attachment prior to rendering.</li>
/// </ul>
///
enum BgiAttachmentLoadOp
{
    BgiAttachmentLoadOpDontCare = 0,
    BgiAttachmentLoadOpClear,
    BgiAttachmentLoadOpLoad,
    
    BgiAttachmentLoadOpCount
};

/// \enum BgiAttachmentStoreOp
///
/// Describes what will happen to the attachment pixel data after rendering.
///
/// <ul>
/// <li>BgiAttachmentStoreOpDontCare:
///   Pixel data is undefined after rendering has completed (no store cost)</li>
/// <li>BgiAttachmentStoreOpStore:
///   The attachment pixel data is stored in memory.</li>
/// </ul>
///
enum BgiAttachmentStoreOp
{
    BgiAttachmentStoreOpDontCare = 0,
    BgiAttachmentStoreOpStore,
    
    BgiAttachmentStoreOpCount
};

/// \enum BgiBufferUsageBits
///
/// Describes the properties and usage of the buffer.
///
/// <ul>
/// <li>BgiBufferUsageUniform:
///   Shader uniform buffer </li>
/// <li>BgiBufferUsageIndex32:
///   Topology 32 bit indices.</li>
/// <li>BgiBufferUsageVertex:
///   Vertex attributes.</li>
/// <li>BgiBufferUsageStorage:
///   Shader storage buffer / Argument buffer.</li>
///
/// <li>BgiBufferUsageCustomBitsBegin:
///   This bit (and any bit after) can be used to attached custom, backend
///   specific  bits to the usage bit. </li>
/// </ul>
///
enum BgiBufferUsageBits : BgiBits
{
    BgiBufferUsageUniform = 1 << 0,
    BgiBufferUsageIndex32 = 1 << 1,
    BgiBufferUsageVertex  = 1 << 2,
    BgiBufferUsageStorage = 1 << 3,

    BgiBufferUsageCustomBitsBegin = 1 << 4,
};
using BgiBufferUsage = BgiBits;

/// \enum BgiShaderStage
///
/// Describes the stage a shader function operates in.
///
/// <ul>
/// <li>BgiShaderStageVertex:
///   Vertex Shader.</li>
/// <li>BgiShaderStageFragment:
///   Fragment Shader.</li>
/// <li>BgiShaderStageCompute:
///   Compute Shader.</li>
/// <li>BgiShaderStageTessellationControl:
///   Transforms the control points of the low order surface (patch).
///   This runs before the tessellator fixed function stage.</li>
/// <li>BgiShaderStageTessellationEval:
///   Generates the surface geometry (the points) from the transformed control
///   points for every coordinate coming out of the tessellator fixed function
///  stage.</li>
/// <li>BgiShaderStageGeometry:
///   Governs the processing of Primitives.</li>
/// <li>BgiShaderStagePostTessellationControl:
///   Metal specific stage which computes tess factors
///   and modifies user post tess vertex data.</li>
/// <li>BgiShaderStagePostTessellationVertex:
///   Metal specific stage which performs tessellation and 
///   vertex processing.</li>
/// </ul>
///
enum BgiShaderStageBits : BgiBits
{
    BgiShaderStageVertex                 = 1 << 0,
    BgiShaderStageFragment               = 1 << 1,
    BgiShaderStageCompute                = 1 << 2,
    BgiShaderStageTessellationControl    = 1 << 3,
    BgiShaderStageTessellationEval       = 1 << 4,
    BgiShaderStageGeometry               = 1 << 5,
    BgiShaderStagePostTessellationControl = 1 << 6,
    BgiShaderStagePostTessellationVertex = 1 << 7,
    BgiShaderStageCustomBitsBegin        = 1 << 8,
};
using BgiShaderStage = BgiBits;

/// \enum BgiBindResourceType
///
/// Describes the type of the resource to be bound.
///
/// <ul>
/// <li>BgiBindResourceTypeSampler:
///   Sampler.
///   Glsl example: uniform sampler samplerOnly</li>
/// <li>BgiBindResourceTypeSampledImage:
///   Image for use with sampling ops.
///   Glsl example: uniform texture2D textureOnly
///   texture(sampler2D(textureOnly, samplerOnly), ...)</li>
/// <li>BgiBindResourceTypeCombinedSamplerImage:
///   Image and sampler combined into one.
///   Glsl example: uniform sampler2D texSmp;
///   texture(texSmp, ...)</li>
/// <li>BgiBindResourceTypeStorageImage:
///   Storage image used for image store/load ops (Unordered Access View).</li>
/// <li>BgiBindResourceTypeUniformBuffer:
///   Uniform buffer (UBO).</li>
/// <li>BgiBindResourceTypeStorageBuffer:
///   Shader storage buffer (SSBO).</li>
/// <li>BgiBindResourceTypeTessFactors:
///   Tessellation factors for Metal tessellation.</li>
/// </ul>
///
enum BgiBindResourceType
{
    BgiBindResourceTypeSampler = 0,
    BgiBindResourceTypeSampledImage,
    BgiBindResourceTypeCombinedSamplerImage,
    BgiBindResourceTypeStorageImage,
    BgiBindResourceTypeUniformBuffer,
    BgiBindResourceTypeStorageBuffer,
    BgiBindResourceTypeTessFactors,

    BgiBindResourceTypeCount
};

/// \enum BgiPolygonMode
///
/// Controls polygon mode during rasterization
///
/// <ul>
/// <li>BgiPolygonModeFill:
///   Polygons are filled.</li>
/// <li>BgiPolygonModeLine:
///   Polygon edges are drawn as line segments.</li>
/// <li>BgiPolygonModePoint:
///   Polygon vertices are drawn as points.</li>
/// </ul>
///
enum BgiPolygonMode
{
    BgiPolygonModeFill = 0,
    BgiPolygonModeLine,
    BgiPolygonModePoint,

    BgiPolygonModeCount
};

/// \enum BgiCullMode
///
/// Controls primitive (faces) culling.
///
/// <ul>
/// <li>BgiPolygonModeNone:
///   No primitive are discarded.</li>
/// <li>BgiPolygonModeFront:
///   Front-facing primitive are discarded.</li>
/// <li>BgiPolygonModeBack:
///   Back-facing primitive are discarded.</li>
/// <li>BgiPolygonModeFrontAndBack:
///   All primitive are discarded.</li>
/// </ul>
///
enum BgiCullMode
{
    BgiCullModeNone = 0,
    BgiCullModeFront,
    BgiCullModeBack,
    BgiCullModeFrontAndBack,

    BgiCullModeCount
};

/// \enum BgiWinding
///
/// Determines the front-facing orientation of a primitive (face).
///
/// <ul>
/// <li>BgiWindingClockwise:
///   Primitives with clockwise vertex-order are front facing.</li>
/// <li>BgiWindingCounterClockwise:
///   Primitives with counter-clockwise vertex-order are front facing.</li>
/// </ul>
///
enum BgiWinding
{
    BgiWindingClockwise = 0,
    BgiWindingCounterClockwise,

    BgiWindingCount
};


/// \enum BgiBlendOp
///
/// Blend operations
///
enum BgiBlendOp
{
    BgiBlendOpAdd = 0,
    BgiBlendOpSubtract,
    BgiBlendOpReverseSubtract,
    BgiBlendOpMin,
    BgiBlendOpMax,

    BgiBlendOpCount
};

/// \enum BgiBlendFactor
///
/// Blend factors
///
enum BgiBlendFactor
{
    BgiBlendFactorZero = 0,
    BgiBlendFactorOne,
    BgiBlendFactorSrcColor,
    BgiBlendFactorOneMinusSrcColor,
    BgiBlendFactorDstColor,
    BgiBlendFactorOneMinusDstColor,
    BgiBlendFactorSrcAlpha,
    BgiBlendFactorOneMinusSrcAlpha,
    BgiBlendFactorDstAlpha,
    BgiBlendFactorOneMinusDstAlpha,
    BgiBlendFactorConstantColor,
    BgiBlendFactorOneMinusConstantColor,
    BgiBlendFactorConstantAlpha,
    BgiBlendFactorOneMinusConstantAlpha,
    BgiBlendFactorSrcAlphaSaturate,
    BgiBlendFactorSrc1Color,
    BgiBlendFactorOneMinusSrc1Color,
    BgiBlendFactorSrc1Alpha,
    BgiBlendFactorOneMinusSrc1Alpha,

    BgiBlendFactorCount
};

/// \enum BgiColorMaskBits
///
/// Describes whether to permit or restrict writing to color components
/// of a color attachment.
///
enum BgiColorMaskBits : BgiBits
{
    BgiColorMaskRed             = 1 << 0,
    BgiColorMaskGreen           = 1 << 1,
    BgiColorMaskBlue            = 1 << 2,
    BgiColorMaskAlpha           = 1 << 3,
};
using BgiColorMask = BgiBits;

/// \enum BgiCompareFunction
///
/// Compare functions.
///
enum BgiCompareFunction
{
    BgiCompareFunctionNever = 0,
    BgiCompareFunctionLess,
    BgiCompareFunctionEqual,
    BgiCompareFunctionLEqual,
    BgiCompareFunctionGreater,
    BgiCompareFunctionNotEqual,
    BgiCompareFunctionGEqual,
    BgiCompareFunctionAlways,

    BgiCompareFunctionCount
};

/// \enum BgiStencilOp
///
/// Stencil operations.
///
enum BgiStencilOp
{
    BgiStencilOpKeep = 0,
    BgiStencilOpZero,
    BgiStencilOpReplace,
    BgiStencilOpIncrementClamp,
    BgiStencilOpDecrementClamp,
    BgiStencilOpInvert,
    BgiStencilOpIncrementWrap,
    BgiStencilOpDecrementWrap,

    BgiStencilOpCount
};

/// \enum BgiComponentSwizzle
///
/// Swizzle for a component.
///
enum BgiComponentSwizzle
{
    BgiComponentSwizzleZero = 0,
    BgiComponentSwizzleOne,
    BgiComponentSwizzleR,
    BgiComponentSwizzleG,
    BgiComponentSwizzleB,
    BgiComponentSwizzleA,

    BgiComponentSwizzleCount
};

/// \enum BgiPrimitiveType
///
/// What the stream of vertices being rendered represents
///
/// <ul>
/// <li>BgiPrimitiveTypePointList:
///   Rasterize a point at each vertex.</li>
/// <li>BgiPrimitiveTypeLineList:
///   Rasterize a line between each separate pair of vertices.</li>
/// <li>BgiPrimitiveTypeLineStrip:
///   Rasterize a line between each pair of adjacent vertices.</li>
/// <li>BgiPrimitiveTypeTriangleList:
///   Rasterize a triangle for every separate set of three vertices.</li>
/// <li>BgiPrimitiveTypePatchList:
///   A user-defined number of vertices, which is tessellated into
///   points, lines, or triangles.</li>
/// <li>BgiPrimitiveTypeLineListWithAdjacency:
///   A four-vertex encoding used to draw untriangulated quads.
///   Rasterize two triangles for every separate set of four vertices.</li>
/// </ul>
///
enum BgiPrimitiveType
{
    BgiPrimitiveTypePointList = 0,
    BgiPrimitiveTypeLineList,
    BgiPrimitiveTypeLineStrip,
    BgiPrimitiveTypeTriangleList,
    BgiPrimitiveTypePatchList,
    BgiPrimitiveTypeLineListWithAdjacency,

    BgiPrimitiveTypeCount
};

/// \enum BgiVertexBufferStepFunction
///
/// Describes the rate at which vertex attributes are pulled from buffers.
///
/// <ul>
/// <li>BgiVertexBufferStepFunctionConstant:
///   The same attribute data is used for every vertex.</li>
/// <li>BgiVertexBufferStepFunctionPerVertex:
///   New attribute data is fetched for each vertex.</li>
/// <li>BgiVertexBufferStepFunctionPerInstance:
///   New attribute data is fetched for each instance.</li>
/// <li>BgiVertexBufferStepFunctionPerPatch:
///   New attribute data is fetched for each patch.</li>
/// <li>BgiVertexBufferStepFunctionPerPatchControlPoint:
///   New attribute data is fetched for each patch control point.</li>
/// <li>BgiVertexBufferStepFunctionPerDrawCommand:
///   New attribute data is fetched for each draw in a multi-draw command.</li>
/// </ul>
///
enum BgiVertexBufferStepFunction
{
    BgiVertexBufferStepFunctionConstant = 0,
    BgiVertexBufferStepFunctionPerVertex,
    BgiVertexBufferStepFunctionPerInstance,
    BgiVertexBufferStepFunctionPerPatch,
    BgiVertexBufferStepFunctionPerPatchControlPoint,
    BgiVertexBufferStepFunctionPerDrawCommand,

    BgiVertexBufferStepFunctionCount
};

/// \enum BgiSubmitWaitType
///
/// Describes command submission wait behavior.
///
/// <ul>
/// <li>BgiSubmitWaitTypeNoWait:
///   CPU should not wait for the GPU to finish processing the cmds.</li>
/// <li>BgiSubmitWaitTypeWaitUntilCompleted:
///   The CPU waits ("blocked") until the GPU has consumed the cmds.</li>
/// </ul>
///
enum BgiSubmitWaitType
{
    BgiSubmitWaitTypeNoWait = 0,
    BgiSubmitWaitTypeWaitUntilCompleted,
};

/// \enum BgiMemoryBarrier
///
/// Describes what objects the memory barrier affects.
///
/// <ul>
/// <li>BgiMemoryBarrierNone:
///   No barrier (no-op).</li>
/// <li>BgiMemoryBarrierAll:
///   The barrier affects all memory writes and reads.</li>
/// </ul>
///
enum BgiMemoryBarrierBits
{
    BgiMemoryBarrierNone = 0,
    BgiMemoryBarrierAll  = 1 << 0
};
using BgiMemoryBarrier = BgiBits;

/// \enum BgiBindingType
///
/// Describes the type of shader resource binding model to use.
///
/// <ul>
/// <li>BgiBindingTypeValue:
///   Shader declares binding as a value.
///   Glsl example: buffer { int parameter; };
///   Msl example: int parameter;</li>
/// <li>BgiBindingTypeUniformValue:
///   Shader declares binding as a uniform block value.
///   Glsl example: uniform { int parameter; };
///   Msl example: int parameter;</li>
/// <li>BgiBindingTypeArray:
///   Shader declares binding as array value.
///   Glsl example: buffer { int parameter[n]; };
///   Msl example: int parameter[n];</li>
/// <li>BgiBindingTypeUniformArray:
///   Shader declares binding as uniform block array value.
///   Glsl example: uniform { int parameter[n]; };
///   Msl example: int parameter[n];</li>
/// <li>BgiBindingTypePointer:
///   Shader declares binding as pointer value.
///   Glsl example: buffer { int parameter[] };
///   Msl example: int *parameter;</li>
/// </ul>
///
enum BgiBindingType
{
    BgiBindingTypeValue = 0,
    BgiBindingTypeUniformValue,
    BgiBindingTypeArray,
    BgiBindingTypeUniformArray,
    BgiBindingTypePointer,
};

/// \enum BgiInterpolationType
///
/// Describes the type of parameter interpolation.
///
/// <ul>
/// <li>BgiInterpolationDefault:
///   The shader input will have default interpolation.
///   Glsl example: vec2 parameter;
///   Msl example: vec2 parameter;</li>
/// <li>BgiInterpolationFlat:
///   The shader input will have no interpolation.
///   Glsl example: flat vec2 parameter;
///   Msl example: vec2 parameter[[flat]];</li>
/// <li>BgiBindingTypeNoPerspective:
///   The shader input will be linearly interpolated in screen-space
///   Glsl example: noperspective vec2 parameter;
///   Msl example: vec2 parameter[[center_no_perspective]];</li>
/// </ul>
///
enum BgiInterpolationType
{
    BgiInterpolationDefault = 0,
    BgiInterpolationFlat,
    BgiInterpolationNoPerspective,
};

/// \enum BgiSamplingType
///
/// Describes the type of parameter sampling.
///
/// <ul>
/// <li>BgiSamplingDefault:
///   The shader input will have default sampling.
///   Glsl example: vec2 parameter;
///   Msl example: vec2 parameter;</li>
/// <li>BgiSamplingCentroid:
///   The shader input will have centroid sampling.
///   Glsl example: centroid vec2 parameter;
///   Msl example: vec2 parameter[[centroid_perspective]];</li>
/// <li>BgiSamplingSample:
///   The shader input will have per-sample sampling.
///   Glsl example: sample vec2 parameter;
///   Msl example: vec2 parameter[[sample_perspective]];</li>
/// </ul>
///
enum BgiSamplingType
{
    BgiSamplingDefault = 0,
    BgiSamplingCentroid,
    BgiSamplingSample,
};

/// \enum BgiStorageType
///
/// Describes the type of parameter storage.
///
/// <ul>
/// <li>BgiStorageDefault:
///   The shader input will have default storage.
///   Glsl example: vec2 parameter;</li>
/// <li>BgiStoragePatch:
///   The shader input will have per-patch storage.
///   Glsl example: patch vec2 parameter;</li>
/// </ul>
///
enum BgiStorageType
{
    BgiStorageDefault = 0,
    BgiStoragePatch,
};

/// \enum BgiShaderTextureType
///
/// Describes the type of texture to be used in shader gen.
///
/// <ul>
/// <li>BgiShaderTextureTypeTexture:
///   Indicates a regular texture.</li>
/// <li>BgiShaderTextureTypeShadowTexture:
///   Indicates a shadow texture.</li>
/// <li>BgiShaderTextureTypeArrayTexture:
///   Indicates an array texture.</li>
/// </ul>
///
enum BgiShaderTextureType
{
    BgiShaderTextureTypeTexture = 0,
    BgiShaderTextureTypeShadowTexture,
    BgiShaderTextureTypeArrayTexture
};

/// \enum BgiComputeDispatch
///
/// Specifies the dispatch method for compute encoders.
///
/// <ul>
/// <li>BgiComputeDispatchSerial:
///   Kernels are dispatched serially.</li>
/// <li>BgiComputeDispatchConcurrent:
///   Kernels are dispatched concurrently, if supported by the API</li>
/// </ul>
///
enum BgiComputeDispatch
{
    BgiComputeDispatchSerial = 0,
    BgiComputeDispatchConcurrent
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif GUNGNIR_DRIVER_ENUMS_H
