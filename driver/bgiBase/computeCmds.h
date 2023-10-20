#ifndef GUNGNIR_DRIVER_BASE_COMPUTE_CMDS_H
#define GUNGNIR_DRIVER_BASE_COMPUTE_CMDS_H

#include "cmds.h"
#include "computeCmds.h"
#include "resourceBindings.h"

#include <memory.h>

GUNGNIR_NAMESPACE_OPEN_SCOPE

using BgiComputeCmdsUniquePtr = std::unique_ptr<class BgiComputeCmds>;

/// \class HgiComputeCmds
///
/// A graphics API independent abstraction of compute commands.
/// HgiComputeCmds is a lightweight object that cannot be re-used after it has
/// been submitted. A new cmds object should be acquired for each frame.
///
class BgiComputeCmds : public BgiCmds
{
public:
    ~BgiComputeCmds() override;

    /// Push a debug marker.
    virtual void PushDebugGroup(const char* label) = 0;

    /// Pop the last debug marker.
    virtual void PopDebugGroup() = 0;

    /// Bind a pipeline state object. Usually you call this right after calling
    /// CreateGraphicsCmds to set the graphics pipeline state.
    /// The resource bindings used when creating the pipeline must be compatible
    /// with the resources bound via BindResources().
    virtual void BindPipeline(BgiComputePipelineHandle pipeline) = 0;

    /// Bind resources such as textures and uniform buffers.
    /// Usually you call this right after BindPipeline() and the resources bound
    /// must be compatible with the bound pipeline.
    virtual void BindResources(BgiResourceBindingsHandle resources) = 0;

    /// Set Push / Function constants.
    /// `pipeline` is the compute pipeline that you are binding before the
    /// draw call. It contains the program used for the uniform buffer
    /// constant values for.
    /// `bindIndex` is the binding point index in the pipeline's shader
    /// to bind the data to.
    /// `byteSize` is the size of the data you are updating.
    /// `data` is the data you are copying into the push constants block.
    virtual void SetConstantValues(
        BgiComputePipelineHandle pipeline,
        uint32_t bindIndex,
        uint32_t byteSize,
        const void* data) = 0;

    /// Execute a compute shader with provided thread group count in each
    /// dimension.
    virtual void Dispatch(int dimX, int dimY) = 0;

    /// Inserts a barrier so that data written to memory by commands before
    /// the barrier is available to commands after the barrier.
    virtual void InsertMemoryBarrier(BgiMemoryBarrier barrier) = 0;

    /// Returns the dispatch method for this encoder.
    virtual BgiComputeDispatch GetDispatchMethod() const = 0;

protected:
    BgiComputeCmds();

private:
    BgiComputeCmds & operator=(const BgiComputeCmds&) = delete;
    BgiComputeCmds(const BgiComputeCmds&) = delete;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif
