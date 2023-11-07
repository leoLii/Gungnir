#pragma once

#include "core/base.h"

#include "driver/bgiBase/api.h"
#include "driver/bgiBase/cmds.h"
#include "driver/bgiBase/buffer.h"
#include "driver/bgiBase/texture.h"

#include <memory>

GUNGNIR_NAMESPACE_OPEN_SCOPE

struct BgiTextureGpuToCpuOp;
struct BgiTextureCpuToGpuOp;
struct BgiBufferGpuToGpuOp;
struct BgiBufferCpuToGpuOp;
struct BgiBufferGpuToCpuOp;
struct BgiTextureToBufferOp;
struct BgiBufferToTextureOp;
struct BgiResolveImageOp;

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
    BGI_API
    ~BgiBlitCmds() override;

    /// Push a debug marker.
    BGI_API
    virtual void PushDebugGroup(const char* label) = 0;

    /// Pop the lastest debug.
    BGI_API
    virtual void PopDebugGroup() = 0;

    /// Copy a texture resource from GPU to CPU.
    /// Synchronization between GPU writes and CPU reads must be managed by
    /// the client by supplying the correct 'wait' flags in SubmitCmds.
    BGI_API
    virtual void CopyTextureGpuToCpu(BgiTextureGpuToCpuOp const& copyOp) = 0;

    /// Copy new data from the CPU into a GPU texture.
    BGI_API
    virtual void CopyTextureCpuToGpu(BgiTextureCpuToGpuOp const& copyOp) = 0;

    /// Copy a buffer resource from GPU to GPU.
    BGI_API
    virtual void CopyBufferGpuToGpu(BgiBufferGpuToGpuOp const& copyOp) = 0;

    /// Copy new data from CPU into GPU buffer.
    /// For example copy new data into a uniform block or storage buffer.
    BGI_API
    virtual void CopyBufferCpuToGpu(BgiBufferCpuToGpuOp const& copyOp) = 0;

    /// Copy new data from GPU into CPU buffer.
    /// Synchronization between GPU writes and CPU reads must be managed by
    /// the client by supplying the correct 'wait' flags in SubmitCmds.
    BGI_API
    virtual void CopyBufferGpuToCpu(BgiBufferGpuToCpuOp const& copyOp) = 0;

    /// Copy a texture resource into a buffer resource from GPU to GPU.
    BGI_API
    virtual void CopyTextureToBuffer(BgiTextureToBufferOp const& copyOp) = 0;

    /// Copy a buffer resource into a texture resource from GPU to GPU.
    BGI_API
    virtual void CopyBufferToTexture(BgiBufferToTextureOp const& copyOp) = 0;

    /// Generate mip maps for a texture
    BGI_API
    virtual void GenerateMipMaps(BgiTextureHandle const& texture) = 0;
    
    /// Fill a buffer with a constant value.
    BGI_API
    virtual void FillBuffer(BgiBufferHandle const& buffer, uint8_t value) = 0;

    /// Inserts a barrier so that data written to memory by commands before
    /// the barrier is available to commands after the barrier.
    BGI_API
    virtual void InsertMemoryBarrier(BgiMemoryBarrier barrier) = 0;

protected:
    BGI_API
    BgiBlitCmds();

private:
    BgiBlitCmds & operator=(const BgiBlitCmds&) = delete;
    BgiBlitCmds(const BgiBlitCmds&) = delete;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE
