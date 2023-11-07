#pragma once

#include "core/base.h"

#include "driver/bgiBase/api.h"
#include "driver/bgiBase/cmds.h"
#include "driver/bgiBase/resourceBindings.h"
#include "driver/bgiBase/graphicsPipeline.h"

#include <memory>
#include <stdint.h>

GUNGNIR_NAMESPACE_OPEN_SCOPE

class Bgi;
class BgiComputeCmds;
class BgiGraphicsCmds;

struct BgiIndirectCommands
{
    BgiIndirectCommands(uint32_t drawCount,
                        BgiGraphicsPipelineHandle const &graphicsPipeline,
                        BgiResourceBindingsHandle const &resourceBindings)
        : drawCount(drawCount)
        , graphicsPipeline(graphicsPipeline)
        , resourceBindings(resourceBindings)
    {
    }

    virtual ~BgiIndirectCommands() = default;

    uint32_t drawCount;
    BgiGraphicsPipelineHandle graphicsPipeline;
    BgiResourceBindingsHandle resourceBindings;
};

using BgiIndirectCommandsUniquePtr = std::unique_ptr<BgiIndirectCommands>;

/// \class HgiIndirectCommandEncoder
///
/// The indirect command encoder is used to record the drawing primitives for a
/// batch and capture the resource bindings so that it can be executed
/// efficently in a later stage of rendering.
/// The EncodeDraw and EncodeDrawIndexed functions store all the necessary state
/// in the HgiIndirectCommands structure.  This is sub-classed based on the
/// platform implementation to maintain all the custom state.
/// Execute draw takes the HgiIndirectCommands structure and replays it on the
/// device.  Currently this is only implemented on the Metal HGI device.
///
class BgiIndirectCommandEncoder : public BgiCmds
{
public:
    BGI_API
    ~BgiIndirectCommandEncoder() override;

    /// Encodes a batch of draw commands from the drawParameterBuffer.
    /// Returns a HgiIndirectCommands which holds the necessary buffers and
    /// state for replaying the batch.
    BGI_API
    virtual BgiIndirectCommandsUniquePtr EncodeDraw(
        BgiComputeCmds * computeCmds,
        BgiGraphicsPipelineHandle const& pipeline,
        BgiResourceBindingsHandle const& resourceBindings,
        BgiVertexBufferBindingVector const& vertexBindings,
        BgiBufferHandle const& drawParameterBuffer,
        uint32_t drawBufferByteOffset,
        uint32_t drawCount,
        uint32_t stride) = 0;

    /// Encodes a batch of indexed draw commands from the drawParameterBuffer.
    /// Returns a HgiIndirectCommands which holds the necessary buffers and
    /// state for replaying the batch.
    BGI_API
    virtual BgiIndirectCommandsUniquePtr EncodeDrawIndexed(
        BgiComputeCmds * computeCmds,
        BgiGraphicsPipelineHandle const& pipeline,
        BgiResourceBindingsHandle const& resourceBindings,
        BgiVertexBufferBindingVector const& vertexBindings,
        BgiBufferHandle const& indexBuffer,
        BgiBufferHandle const& drawParameterBuffer,
        uint32_t drawBufferByteOffset,
        uint32_t drawCount,
        uint32_t stride,
        uint32_t patchBaseVertexByteOffset) = 0;

    /// Excutes an indirect command batch from the HgiIndirectCommands
    /// structure.
    BGI_API
    virtual void ExecuteDraw(
        BgiGraphicsCmds * gfxCmds,
        BgiIndirectCommands const* commands) = 0;

protected:
    BGI_API
    BgiIndirectCommandEncoder();

private:
    BgiIndirectCommandEncoder & operator=(const BgiIndirectCommandEncoder&) = delete;
    BgiIndirectCommandEncoder(const BgiIndirectCommandEncoder&) = delete;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE
