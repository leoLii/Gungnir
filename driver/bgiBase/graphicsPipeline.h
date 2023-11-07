#pragma once

#include "core/base.h"
#include "core/math/math.h"

#include "driver/bgiBase/api.h"
#include "driver/bgiBase/attachmentDesc.h"
#include "driver/bgiBase/enums.h"
#include "driver/bgiBase/handle.h"
#include "driver/bgiBase/resourceBindings.h"
#include "driver/bgiBase/shaderProgram.h"
#include "driver/bgiBase/types.h"

#include <string>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

/// \struct HgiVertexAttributeDesc
///
/// Describes one attribute of a vertex.
///
/// <ul>
/// <li>format:
///   Format of the vertex attribute.</li>
/// <li>offset:
///    The byte offset of the attribute in vertex buffer</li>
/// <li>shaderBindLocation:
///    The location of the attribute in the shader. layout(location = X)</li>
/// </ul>
///
struct BgiVertexAttributeDesc
{
    BGI_API
    BgiVertexAttributeDesc();

    BgiFormat format;
    uint32_t offset;
    uint32_t shaderBindLocation;
};
using BgiVertexAttributeDescVector = std::vector<BgiVertexAttributeDesc>;

BGI_API
bool operator==(
    const BgiVertexAttributeDesc& lhs,
    const BgiVertexAttributeDesc& rhs);

BGI_API
bool operator!=(
    const BgiVertexAttributeDesc& lhs,
    const BgiVertexAttributeDesc& rhs);

/// \struct HgiVertexBufferDesc
///
/// Describes the attributes of a vertex buffer.
///
/// <ul>
/// <li>bindingIndex:
///    Binding location for this vertex buffer.</li>
/// <li>vertexAttributes:
///   List of vertex attributes (in vertex buffer).</li>
/// <li>vertexStepFunction:
///   The rate at which data is pulled for this vertex buffer.</li>
/// <li>vertexStride:
///   The byte size of a vertex (distance between two vertices).</li>
/// </ul>
///
struct BgiVertexBufferDesc
{
    BGI_API
    BgiVertexBufferDesc();

    uint32_t bindingIndex;
    BgiVertexAttributeDescVector vertexAttributes;
    BgiVertexBufferStepFunction vertexStepFunction;
    uint32_t vertexStride;
};
using BgiVertexBufferDescVector = std::vector<BgiVertexBufferDesc>;

BGI_API
bool operator==(
    const BgiVertexBufferDesc& lhs,
    const BgiVertexBufferDesc& rhs);

BGI_API
bool operator!=(
    const BgiVertexBufferDesc& lhs,
    const BgiVertexBufferDesc& rhs);

/// \struct HgiMultiSampleState
///
/// Properties to configure multi sampling.
///
/// <ul>
/// <li>multiSampleEnable:
///   When enabled and sampleCount and attachments match and allow for it, use 
///   multi-sampling.</li>
/// <li>alphaToCoverageEnable:
///   Fragment's color.a determines coverage (screen door transparency).</li>
/// <li>alphaToOneEnable:
///   Fragment's color.a is replaced by the maximum representable alpha
///   value for fixed-point color attachments, or by 1.0 for floating-point
///   attachments.</li>
/// <li>sampleCount:
///   The number of samples for each fragment. Must match attachments</li>
/// </ul>
///
struct BgiMultiSampleState
{
    BGI_API
    BgiMultiSampleState();

    bool multiSampleEnable;
    bool alphaToCoverageEnable;
    bool alphaToOneEnable;
    BgiSampleCount sampleCount;
};

BGI_API
bool operator==(
    const BgiMultiSampleState& lhs,
    const BgiMultiSampleState& rhs);

BGI_API
bool operator!=(
    const BgiMultiSampleState& lhs,
    const BgiMultiSampleState& rhs);


/// \struct HgiRasterizationState
///
/// Properties to configure the rasterization state.
///
/// <ul>
/// <li>polygonMode:
///   Determines the rasterization draw mode of primitve (triangles).</li>
/// <li>lineWidth:
///   The width of lines when polygonMode is set to line drawing.</li>
/// <li>cullMode:
///   Determines the culling rules for primitives (triangles).</li>
/// <li>winding:
///   The rule that determines what makes a front-facing primitive.</li>
/// <li>rasterizationEnabled:
///   When false all primitives are discarded before rasterization stage.</li>
/// <li>depthClampEnabled:
///   When enabled clamps the clip space depth to the view volume, rather than
///   clipping the depth to the near and far planes.</li>
/// <li>depthRange:
///   The mapping of NDC depth values to window depth values.</li>
/// <li>conservativeRaster:
///   When enabled, any pixel at least partially covered by a rendered primitive
///   will be rasterized.</li>
/// <li>numClipDistances:
///   The number of user-defined clip distances.</li>
/// </ul>
///
struct BgiRasterizationState
{
    BGI_API
    BgiRasterizationState();

    BgiPolygonMode polygonMode;
    float lineWidth;
    BgiCullMode cullMode;
    BgiWinding winding;
    bool rasterizerEnabled;
    bool depthClampEnabled;
    Vector2f depthRange;
    bool conservativeRaster;
    size_t numClipDistances;
};

BGI_API
bool operator==(
    const BgiRasterizationState& lhs,
    const BgiRasterizationState& rhs);

BGI_API
bool operator!=(
    const BgiRasterizationState& lhs,
    const BgiRasterizationState& rhs);

/// \struct HgiStencilState
///
/// Properties controlling the operation of the stencil test.
///
/// <ul>
/// <li>compareFn:
///   The function used to test the reference value with the masked
///   value read from the stencil buffer.</li>
/// <li>referenceValue:
//.   The reference value used by the stencil test function.</li>
/// <li>stencilFailOp:
///   The operation executed when the stencil test fails.</li>
/// <li>depthFailOp:
///   The operation executed when the stencil test passes but the
///   depth test fails.</li>
/// <li>depthStencilPassOp:
///   The operation executed when both stencil and depth tests pass.</li>
/// <li>readMask:
///   The mask applied to values before the stencil test function.</li>
/// <li>writeMask:
///   The mask applied when writing to the stencil buffer.</li>
/// </ul>
///
struct BgiStencilState
{
    BGI_API
    BgiStencilState();

    BgiCompareFunction compareFn;
    uint32_t referenceValue;
    BgiStencilOp stencilFailOp;
    BgiStencilOp depthFailOp;
    BgiStencilOp depthStencilPassOp;
    uint32_t readMask;
    uint32_t writeMask;
};

BGI_API
bool operator==(
    const BgiStencilState& lhs,
    const BgiStencilState& rhs);

BGI_API
bool operator!=(
    const BgiStencilState& lhs,
    const BgiStencilState& rhs);

/// \struct HgiDepthStencilState
///
/// Properties to configure depth and stencil test.
///
/// <ul>
/// <li>depthTestEnabled:
///   When enabled uses `depthCompareFn` to test if a fragment passes the
///   depth test. Note that depth writes are automatically disabled when
///   depthTestEnabled is false.</li>
/// <li>depthWriteEnabled:
///   When enabled uses `depthCompareFn` to test if a fragment passes the
///   depth test. Note that depth writes are automatically disabled when
///   depthTestEnabled is false.</li>
/// <li>depthCompareFn:
///   The function used to test depth values.</li>
/// <li>depthBiasEnabled:
///   When enabled applies a bias to depth values before the depth test.
/// <li>depthBiasConstantFactor:
///   The constant depth bias.</li>
/// <li>depthBiasSlopeFactor:
///   The depth bias that scales with the gradient of the primitive.</li>
/// <li>stencilTestEnabled:
///   Enables the stencil test.</li>
/// <li>stencilFront:
///   Stencil operation for front faces.</li>
/// <li>stencilBack:
///   Stencil operation for back faces.</li>
/// </ul>
///
struct BgiDepthStencilState
{
    BGI_API
    BgiDepthStencilState();

    bool depthTestEnabled;
    bool depthWriteEnabled;
    BgiCompareFunction depthCompareFn;

    bool depthBiasEnabled;
    float depthBiasConstantFactor;
    float depthBiasSlopeFactor;

    bool stencilTestEnabled;
    BgiStencilState stencilFront;
    BgiStencilState stencilBack;
};

BGI_API
bool operator==(
    const BgiDepthStencilState& lhs,
    const BgiDepthStencilState& rhs);

BGI_API
bool operator!=(
    const BgiDepthStencilState& lhs,
    const BgiDepthStencilState& rhs);

/// \struct HgiGraphicsShaderConstantsDesc
///
/// A small, but fast buffer of uniform data for shaders.
///
/// <ul>
/// <li>byteSize:
///    Size of the constants in bytes. (max 256 bytes)</li>
/// <li>stageUsage:
///    What shader stage(s) the constants will be used in.</li>
/// </ul>
///
struct BgiGraphicsShaderConstantsDesc {
    BGI_API
    BgiGraphicsShaderConstantsDesc();

    uint32_t byteSize;
    BgiShaderStage stageUsage;
};

BGI_API
bool operator==(
    const BgiGraphicsShaderConstantsDesc& lhs,
    const BgiGraphicsShaderConstantsDesc& rhs);

BGI_API
bool operator!=(
    const BgiGraphicsShaderConstantsDesc& lhs,
    const BgiGraphicsShaderConstantsDesc& rhs);

struct BgiTessellationLevel
{
    BGI_API
    BgiTessellationLevel();

    float innerTessLevel[2];
    float outerTessLevel[4];
};

/// \struct HgiTessellationState
///
/// Properties to configure tessellation.
///
/// <ul>
/// <li>patchType:
///   The type of tessellation patch.</li>
/// <li>primitiveIndexSize:
///   The number of control indices per patch.</li>
/// <li>tessellationLevel:
///   The fallback tessellation levels.</li>
/// </ul>
///
struct BgiTessellationState
{
    enum PatchType {
        Triangle,
        Quad,
        Isoline
    };

    enum TessFactorMode {
        Constant,
        TessControl,
        TessVertex
    };

    BGI_API
    BgiTessellationState();

    PatchType patchType;
    int primitiveIndexSize;
    TessFactorMode tessFactorMode = TessFactorMode::Constant;
    BgiTessellationLevel tessellationLevel;
};

/// \struct HgiGraphicsPipelineDesc
///
/// Describes the properties needed to create a GPU pipeline.
///
/// <ul>
/// <li>primitiveType:
///   Describes the stream of vertices (primitive topology).</li>
/// <li>shaderProgram:
///   Shader functions/stages used in this pipeline.</li>
/// <li>depthState:
///   Describes depth state for a pipeline.</li>
/// <li>multiSampleState:
///   Various settings to control multi-sampling.</li>
/// <li>rasterizationState:
///   Various settings to control rasterization.</li>
/// <li>vertexBuffers:
///   Description of the vertex buffers (per-vertex attributes).
///   The actual VBOs are bound via GraphicsCmds.</li>
/// <li>colorAttachmentDescs:
///   Describes each of the color attachments.</li>
/// <li>colorResolveAttachmentDescs:
///   Describes each of the color resolve attachments (optional).</li>
/// <li>depthAttachmentDesc:
///   Describes the depth attachment (optional)
///   Use HgiFormatInvalid to indicate no depth attachment.</li>
/// <li>depthResolveAttachmentDesc:
///   Describes the depth resolve attachment (optional).
///   Use HgiFormatInvalid to indicate no depth resolve attachment.</li>
/// <li>shaderConstantsDesc:
///   Describes the shader uniforms.</li>
/// <li>tessellationState:
///   Describes the tessellation state.</li>
/// </ul>
///
struct BgiGraphicsPipelineDesc
{
    BGI_API
    BgiGraphicsPipelineDesc();

    std::string debugName;
    BgiPrimitiveType primitiveType;
    BgiShaderProgramHandle shaderProgram;
    BgiDepthStencilState depthState;
    BgiMultiSampleState multiSampleState;
    BgiRasterizationState rasterizationState;
    BgiVertexBufferDescVector vertexBuffers;
    BgiAttachmentDescVector colorAttachmentDescs;
    BgiAttachmentDescVector colorResolveAttachmentDescs;
    BgiAttachmentDesc depthAttachmentDesc;
    BgiAttachmentDesc depthResolveAttachmentDesc;
    BgiGraphicsShaderConstantsDesc shaderConstantsDesc;
    BgiTessellationState tessellationState;
};

BGI_API
bool operator==(
    const BgiGraphicsPipelineDesc& lhs,
    const BgiGraphicsPipelineDesc& rhs);

BGI_API
bool operator!=(
    const BgiGraphicsPipelineDesc& lhs,
    const BgiGraphicsPipelineDesc& rhs);


///
/// \class HgiGraphicsPipeline
///
/// Represents a graphics platform independent GPU graphics pipeline
/// resource.
///
/// Base class for Hgi pipelines.
/// To the client (HdSt) pipeline resources are referred to via
/// opaque, stateless handles (HgiPipelineHandle).
///
class BgiGraphicsPipeline
{
public:
    BGI_API
    virtual ~BgiGraphicsPipeline();

    /// The descriptor describes the object.
    BGI_API
    BgiGraphicsPipelineDesc const& GetDescriptor() const;

protected:
    BGI_API
    BgiGraphicsPipeline(BgiGraphicsPipelineDesc const& desc);

    BgiGraphicsPipelineDesc _descriptor;

private:
    BgiGraphicsPipeline() = delete;
    BgiGraphicsPipeline & operator=(const BgiGraphicsPipeline&) = delete;
    BgiGraphicsPipeline(const BgiGraphicsPipeline&) = delete;
};

using BgiGraphicsPipelineHandle = BgiHandle<BgiGraphicsPipeline>;
using BgiGraphicsPipelineHandleVector = std::vector<BgiGraphicsPipelineHandle>;

GUNGNIR_NAMESPACE_CLOSE_SCOPE
