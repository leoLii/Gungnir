#ifndef GUNGNIR_DRIVER_BASE_BLITCMDS_H
#define GUNGNIR_DRIVER_BASE_BLITCMDS_H

#include "cmds.h"
#include "buffer.h"
#include "texture.h"
#include "types.h"
#include "enums.h"

#include <memory>

GUNGNIR_NAMESPACE_OPEN_SCOPE

struct BgiTextureGpuToCpuOp;
struct BgiTextureCpuToGpuOp;
struct BgiBufferGpuToGpuOp;
struct BgiBufferCpuToGpuOp;
struct BgiBufferGpuToCpuOp;
struct BgiTextureToBufferOp;
struct BgiBufferToTextureOp;

using BgiBlitCmdsUniquePtr = std::unique_ptr<class BgiBlitCmds>;

/// \class HgiBlitCmds
///
/// A graphics API independent abstraction of resource copy commands.
/// HgiBlitCmds is a lightweight object that cannot be re-used after it has
/// been submitted. A new cmds object should be acquired for each frame.
///
class BgiBlitCmds : public BgiCmds
{
public:
    ~BgiBlitCmds() override;

    /// Push a debug marker.
    virtual void PushDebugGroup(const char* label) = 0;

    /// Pop the lastest debug.
    virtual void PopDebugGroup() = 0;

    /// Copy a texture resource from GPU to CPU.
    /// Synchronization between GPU writes and CPU reads must be managed by
    /// the client by supplying the correct 'wait' flags in SubmitCmds.
    virtual void CopyTextureGpuToCpu(HgiTextureGpuToCpuOp const& copyOp) = 0;

    /// Copy new data from the CPU into a GPU texture.
    virtual void CopyTextureCpuToGpu(HgiTextureCpuToGpuOp const& copyOp) = 0;

    /// Copy a buffer resource from GPU to GPU.
    virtual void CopyBufferGpuToGpu(HgiBufferGpuToGpuOp const& copyOp) = 0;

    /// Copy new data from CPU into GPU buffer.
    /// For example copy new data into a uniform block or storage buffer.
    virtual void CopyBufferCpuToGpu(HgiBufferCpuToGpuOp const& copyOp) = 0;

    /// Copy new data from GPU into CPU buffer.
    /// Synchronization between GPU writes and CPU reads must be managed by
    /// the client by supplying the correct 'wait' flags in SubmitCmds.
    virtual void CopyBufferGpuToCpu(HgiBufferGpuToCpuOp const& copyOp) = 0;

    /// Copy a texture resource into a buffer resource from GPU to GPU.
    virtual void CopyTextureToBuffer(HgiTextureToBufferOp const& copyOp) = 0;

    /// Copy a buffer resource into a texture resource from GPU to GPU.
    virtual void CopyBufferToTexture(HgiBufferToTextureOp const& copyOp) = 0;

    /// Generate mip maps for a texture
    virtual void GenerateMipMaps(HgiTextureHandle const& texture) = 0;
    
    /// Fill a buffer with a constant value.
    virtual void FillBuffer(HgiBufferHandle const& buffer, uint8_t value) = 0;

    /// Inserts a barrier so that data written to memory by commands before
    /// the barrier is available to commands after the barrier.
    virtual void InsertMemoryBarrier(HgiMemoryBarrier barrier) = 0;

protected:
    HgiBlitCmds();

private:
    HgiBlitCmds & operator=(const HgiBlitCmds&) = delete;
    HgiBlitCmds(const HgiBlitCmds&) = delete;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif
