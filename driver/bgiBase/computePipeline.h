#pragma once

#include "common/base.h"

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

namespace driver {

/// \struct HgiComputeShaderConstantsDesc
///
/// A small, but fast buffer of uniform data for shaders.
///
/// <ul>
/// <li>byteSize:
///    Size of the constants in bytes. (max 256 bytes)</li>
/// </ul>
///
struct BgiComputeShaderConstantsDesc {
    BGI_API
    BgiComputeShaderConstantsDesc();

    uint32_t byteSize;
};

BGI_API
bool operator==(
    const BgiComputeShaderConstantsDesc& lhs,
    const BgiComputeShaderConstantsDesc& rhs);

BGI_API
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
    BGI_API
    BgiComputePipelineDesc();

    std::string debugName;
    BgiShaderProgramHandle shaderProgram;
    BgiComputeShaderConstantsDesc shaderConstantsDesc;
};

BGI_API
bool operator==(
    const BgiComputePipelineDesc& lhs,
    const BgiComputePipelineDesc& rhs);

BGI_API
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
    BGI_API
    virtual ~BgiComputePipeline();

    /// The descriptor describes the object.
    BGI_API
    BgiComputePipelineDesc const& GetDescriptor() const;

protected:
    BGI_API
    BgiComputePipeline(BgiComputePipelineDesc const& desc);

    BgiComputePipelineDesc _descriptor;

private:
    BgiComputePipeline() = delete;
    BgiComputePipeline & operator=(const BgiComputePipeline&) = delete;
    BgiComputePipeline(const BgiComputePipeline&) = delete;
};

using BgiComputePipelineHandle = BgiHandle<class BgiComputePipeline>;
using BgiComputePipelineHandleVector = std::vector<BgiComputePipelineHandle>;

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
