#ifndef GUNGNIR_DRIVER_BASE_BLIT_CMDS_H
#define GUNGNIR_DRIVER_BASE_BLIT_CMDS_H

#include "driver/bgiBase/cmds.h"
#include "driver/bgiBase/buffer.h"
#include "driver/bgiBase/texture.h"
#include "driver/bgiBase/types.h"
#include "driver/bgiBase/enums.h"

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
    virtual void CopyTextureGpuToCpu(BgiTextureGpuToCpuOp const& copyOp) = 0;

    /// Copy new data from the CPU into a GPU texture.
    virtual void CopyTextureCpuToGpu(BgiTextureCpuToGpuOp const& copyOp) = 0;

    /// Copy a buffer resource from GPU to GPU.
    virtual void CopyBufferGpuToGpu(BgiBufferGpuToGpuOp const& copyOp) = 0;

    /// Copy new data from CPU into GPU buffer.
    /// For example copy new data into a uniform block or storage buffer.
    virtual void CopyBufferCpuToGpu(BgiBufferCpuToGpuOp const& copyOp) = 0;

    /// Copy new data from GPU into CPU buffer.
    /// Synchronization between GPU writes and CPU reads must be managed by
    /// the client by supplying the correct 'wait' flags in SubmitCmds.
    virtual void CopyBufferGpuToCpu(BgiBufferGpuToCpuOp const& copyOp) = 0;

    /// Copy a texture resource into a buffer resource from GPU to GPU.
    virtual void CopyTextureToBuffer(BgiTextureToBufferOp const& copyOp) = 0;

    /// Copy a buffer resource into a texture resource from GPU to GPU.
    virtual void CopyBufferToTexture(BgiBufferToTextureOp const& copyOp) = 0;

    /// Generate mip maps for a texture
    virtual void GenerateMipMaps(BgiTextureHandle const& texture) = 0;
    
    /// Fill a buffer with a constant value.
    virtual void FillBuffer(BgiBufferHandle const& buffer, uint8_t value) = 0;

    /// Inserts a barrier so that data written to memory by commands before
    /// the barrier is available to commands after the barrier.
    virtual void InsertMemoryBarrier(BgiMemoryBarrier barrier) = 0;

protected:
    BgiBlitCmds();

private:
    BgiBlitCmds & operator=(const BgiBlitCmds&) = delete;
    BgiBlitCmds(const BgiBlitCmds&) = delete;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif
