#ifndef GUNGNIR_DRIVER_BASE_COMPUTE_PIPELINE_H
#define GUNGNIR_DRIVER_BASE_COMPUTE_PIPELINE_H

#include "attachmentDesc.h"
#include "enums.h"
#include "handle.h"
#include "resourceBindings.h"
#include "shaderProgram.h"
#include "types.h"

#include <string>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

/// \struct HgiComputeShaderConstantsDesc
///
/// A small, but fast buffer of uniform data for shaders.
///
/// <ul>
/// <li>byteSize:
///    Size of the constants in bytes. (max 256 bytes)</li>
/// </ul>
///
struct HgiComputeShaderConstantsDesc {
    HgiComputeShaderConstantsDesc();

    uint32_t byteSize;
};

bool operator==(
    const BgiComputeShaderConstantsDesc& lhs,
    const BgiComputeShaderConstantsDesc& rhs);

bool operator!=(
    const BgiComputeShaderConstantsDesc& lhs,
    const BgiComputeShaderConstantsDesc& rhs);

/// \struct HgiComputePipelineDesc
///
/// Describes the properties needed to create a GPU compute pipeline.
///
/// <ul>
/// <li>shaderProgram:
///   Shader function used in this pipeline.</li>
/// <li>shaderConstantsDesc:
///   Describes the shader uniforms.</li>
/// </ul>
///
struct BgiComputePipelineDesc
{
    BgiComputePipelineDesc();

    std::string debugName;
    BgiShaderProgramHandle shaderProgram;
    BgiComputeShaderConstantsDesc shaderConstantsDesc;
};

bool operator==(
    const BgiComputePipelineDesc& lhs,
    const BgiComputePipelineDesc& rhs);

bool operator!=(
    const BgiComputePipelineDesc& lhs,
    const BgiComputePipelineDesc& rhs);

///
/// \class HgiComputePipeline
///
/// Represents a graphics platform independent GPU compute pipeline resource.
///
/// Base class for Hgi compute pipelines.
/// To the client (HdSt) compute pipeline resources are referred to via
/// opaque, stateless handles (HgiComputePipelineHandle).
///
class BgiComputePipeline
{
public:
    virtual ~BgiComputePipeline();

    /// The descriptor describes the object.
    BgiComputePipelineDesc const& GetDescriptor() const;

protected:
    BgiComputePipeline(BgiComputePipelineDesc const& desc);

    BgiComputePipelineDesc _descriptor;

private:
    BgiComputePipeline() = delete;
    BgiComputePipeline & operator=(const BgiComputePipeline&) = delete;
    BgiComputePipeline(const BgiComputePipeline&) = delete;
};

using BgiComputePipelineHandle = BgiHandle<class BgiComputePipeline>;
using BgiComputePipelineHandleVector = std::vector<BgiComputePipelineHandle>;

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif
