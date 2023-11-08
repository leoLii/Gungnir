#include "driver/bgiBase/graphicsPipeline.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

BgiVertexAttributeDesc::BgiVertexAttributeDesc()
    : format(BgiFormatFloat32Vec4)
    , offset(0)
    , shaderBindLocation(0)
{
}

bool operator==(
    const BgiVertexAttributeDesc& lhs,
    const BgiVertexAttributeDesc& rhs)
{
    return lhs.format == rhs.format &&
           lhs.offset == rhs.offset &&
           lhs.shaderBindLocation == rhs.shaderBindLocation;
}

bool operator!=(
    const BgiVertexAttributeDesc& lhs,
    const BgiVertexAttributeDesc& rhs)
{
    return !(lhs == rhs);
}

BgiVertexBufferDesc::BgiVertexBufferDesc()
    : bindingIndex(0)
    , vertexStepFunction(BgiVertexBufferStepFunctionPerVertex)
    , vertexStride(0)
{
}

bool operator==(
    const BgiVertexBufferDesc& lhs,
    const BgiVertexBufferDesc& rhs)
{
    return lhs.bindingIndex == rhs.bindingIndex &&
           lhs.vertexAttributes == rhs.vertexAttributes &&
           lhs.vertexStepFunction == rhs.vertexStepFunction &&
           lhs.vertexStride == rhs.vertexStride;
}

bool operator!=(
    const BgiVertexBufferDesc& lhs,
    const BgiVertexBufferDesc& rhs)
{
    return !(lhs == rhs);
}

BgiMultiSampleState::BgiMultiSampleState()
    : multiSampleEnable(true)
    , alphaToCoverageEnable(false)
    , alphaToOneEnable(false)
    , sampleCount(BgiSampleCount1)
{
}

bool operator==(
    const BgiMultiSampleState& lhs,
    const BgiMultiSampleState& rhs)
{
    return lhs.multiSampleEnable == rhs.multiSampleEnable &&
           lhs.alphaToCoverageEnable == rhs.alphaToCoverageEnable &&
           lhs.alphaToOneEnable == rhs.alphaToOneEnable &&
           lhs.sampleCount == rhs.sampleCount;
}

bool operator!=(
    const BgiMultiSampleState& lhs,
    const BgiMultiSampleState& rhs)
{
    return !(lhs == rhs);
}

BgiRasterizationState::BgiRasterizationState()
    : polygonMode(BgiPolygonModeFill)
    , lineWidth(1.0f)
    , cullMode(BgiCullModeBack)
    , winding(BgiWindingCounterClockwise)
    , rasterizerEnabled(true)
    , depthClampEnabled(false)
    , depthRange(0.f, 1.f)
    , conservativeRaster(false)
    , numClipDistances(0)
{
}

bool operator==(
    const BgiRasterizationState& lhs,
    const BgiRasterizationState& rhs)
{
    return lhs.polygonMode == rhs.polygonMode &&
           lhs.lineWidth == rhs.lineWidth &&
           lhs.cullMode == rhs.cullMode &&
           lhs.winding == rhs.winding &&
           lhs.rasterizerEnabled == rhs.rasterizerEnabled &&
           lhs.depthClampEnabled == rhs.depthClampEnabled &&
           lhs.depthRange == rhs.depthRange &&
           lhs.conservativeRaster == rhs.conservativeRaster &&
           lhs.numClipDistances == rhs.numClipDistances;
}

bool operator!=(
    const BgiRasterizationState& lhs,
    const BgiRasterizationState& rhs)
{
    return !(lhs == rhs);
}

BgiDepthStencilState::BgiDepthStencilState()
    : depthTestEnabled(true)
    , depthWriteEnabled(true)
    , depthCompareFn(BgiCompareFunctionLess)
    , depthBiasEnabled(false)
    , depthBiasConstantFactor(0.0f)
    , depthBiasSlopeFactor(0.0f)
    , stencilTestEnabled(false)
    , stencilFront()
    , stencilBack()
{
}

bool operator==(
    const BgiDepthStencilState& lhs,
    const BgiDepthStencilState& rhs)
{
    return lhs.depthTestEnabled == rhs.depthTestEnabled &&
           lhs.depthWriteEnabled == rhs.depthWriteEnabled &&
           lhs.depthCompareFn == rhs.depthCompareFn &&
           lhs.depthBiasEnabled == rhs.depthBiasEnabled &&
           lhs.depthBiasConstantFactor == rhs.depthBiasConstantFactor &&
           lhs.depthBiasSlopeFactor == rhs.depthBiasSlopeFactor &&
           lhs.stencilTestEnabled == rhs.stencilTestEnabled &&
           lhs.stencilFront == rhs.stencilFront &&
           lhs.stencilBack == rhs.stencilBack;
}

bool operator!=(
    const BgiDepthStencilState& lhs,
    const BgiDepthStencilState& rhs)
{
    return !(lhs == rhs);
}

BgiStencilState::BgiStencilState()
    : compareFn(BgiCompareFunctionAlways)
    , referenceValue(0)
    , stencilFailOp(BgiStencilOpKeep)
    , depthFailOp(BgiStencilOpKeep)
    , depthStencilPassOp(BgiStencilOpKeep)
    , readMask(0xffffffff)
    , writeMask(0xffffffff)
{
}

bool operator==(
    const BgiStencilState& lhs,
    const BgiStencilState& rhs)
{
    return lhs.compareFn == rhs.compareFn &&
           lhs.referenceValue == rhs.referenceValue &&
           lhs.stencilFailOp == rhs.stencilFailOp &&
           lhs.depthFailOp == rhs.depthFailOp &&
           lhs.depthStencilPassOp == rhs.depthStencilPassOp &&
           lhs.readMask == rhs.readMask &&
           lhs.writeMask == rhs.writeMask;
}

bool operator!=(
    const BgiStencilState& lhs,
    const BgiStencilState& rhs)
{
    return !(lhs == rhs);
}

BgiGraphicsShaderConstantsDesc::BgiGraphicsShaderConstantsDesc()
    : byteSize(0)
    , stageUsage(BgiShaderStageFragment)
{
}

bool operator==(
    const BgiGraphicsShaderConstantsDesc& lhs,
    const BgiGraphicsShaderConstantsDesc& rhs)
{
    return lhs.byteSize == rhs.byteSize &&
           lhs.stageUsage == rhs.stageUsage;
}

bool operator!=(
    const BgiGraphicsShaderConstantsDesc& lhs,
    const BgiGraphicsShaderConstantsDesc& rhs)
{
    return !(lhs == rhs);
}

BgiTessellationLevel::BgiTessellationLevel()
    : innerTessLevel{0, 0}
    , outerTessLevel{0, 0, 0, 0}
{
}

BgiTessellationState::BgiTessellationState()
    : patchType(Triangle)
    , primitiveIndexSize(0)
    , tessellationLevel()
{
}

BgiGraphicsPipelineDesc::BgiGraphicsPipelineDesc()
    : primitiveType(BgiPrimitiveTypeTriangleList)
{
}

BgiGraphicsPipelineDesc const&
BgiGraphicsPipeline::GetDescriptor() const
{
    return _descriptor;
}

bool operator==(
    const BgiGraphicsPipelineDesc& lhs,
    const BgiGraphicsPipelineDesc& rhs)
{
    return lhs.debugName == rhs.debugName &&
           lhs.primitiveType == rhs.primitiveType &&
           lhs.shaderProgram == rhs.shaderProgram &&
           lhs.depthState == rhs.depthState &&
           lhs.multiSampleState == rhs.multiSampleState &&
           lhs.rasterizationState == rhs.rasterizationState &&
           lhs.vertexBuffers == rhs.vertexBuffers &&
           lhs.colorAttachmentDescs == rhs.colorAttachmentDescs &&
           lhs.colorResolveAttachmentDescs == rhs.colorResolveAttachmentDescs &&
           lhs.depthAttachmentDesc == rhs.depthAttachmentDesc &&
           lhs.depthResolveAttachmentDesc == rhs.depthResolveAttachmentDesc &&
           lhs.shaderConstantsDesc == rhs.shaderConstantsDesc;
}

bool operator!=(
    const BgiGraphicsPipelineDesc& lhs,
    const BgiGraphicsPipelineDesc& rhs)
{
    return !(lhs == rhs);
}

BgiGraphicsPipeline::BgiGraphicsPipeline(BgiGraphicsPipelineDesc const& desc)
    : _descriptor(desc)
{
}

BgiGraphicsPipeline::~BgiGraphicsPipeline() = default;

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
