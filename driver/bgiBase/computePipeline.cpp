#include "driver/bgiBase/computePipeline.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

BgiComputeShaderConstantsDesc::BgiComputeShaderConstantsDesc()
    : byteSize(0)
{
}

bool operator==(
    const BgiComputeShaderConstantsDesc& lhs,
    const BgiComputeShaderConstantsDesc& rhs)
{
    return lhs.byteSize == rhs.byteSize;
}

bool operator!=(
    const BgiComputeShaderConstantsDesc& lhs,
    const BgiComputeShaderConstantsDesc& rhs)
{
    return !(lhs == rhs);
}

BgiComputePipelineDesc::BgiComputePipelineDesc()
    : shaderProgram()
{
}

bool operator==(
    const BgiComputePipelineDesc& lhs,
    const BgiComputePipelineDesc& rhs)
{
    return lhs.debugName == rhs.debugName &&
           lhs.shaderProgram == rhs.shaderProgram &&
           lhs.shaderConstantsDesc == rhs.shaderConstantsDesc;
}

bool operator!=(
    const BgiComputePipelineDesc& lhs,
    const BgiComputePipelineDesc& rhs)
{
    return !(lhs == rhs);
}

BgiComputePipeline::BgiComputePipeline(BgiComputePipelineDesc const& desc)
    : _descriptor(desc)
{
}

BgiComputePipeline::~BgiComputePipeline() = default;

BgiComputePipelineDesc const&
BgiComputePipeline::GetDescriptor() const
{
    return _descriptor;
}

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
