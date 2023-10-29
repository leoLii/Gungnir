#ifndef GUNGNIR_DRIVER_BASE_BGI_H
#define GUNGNIR_DRIVER_BASE_BGI_H

#include "core/base.h"

#include "driver/bgiBase/blitCmds.h"
#include "driver/bgiBase/buffer.h"
#include "driver/bgiBase/computeCmds.h"
#include "driver/bgiBase/computeCmdsDesc.h"
#include "driver/bgiBase/graphicsCmds.h"
#include "driver/bgiBase/graphicsCmdsDesc.h"
#include "driver/bgiBase/resourceBindings.h"
#include "driver/bgiBase/sampler.h"
#include "driver/bgiBase/shaderProgram.h"
#include "driver/bgiBase/texture.h"
#include "driver/bgiBase/types.h"

#include <atomic>
#include <memory>

GUNGNIR_NAMESPACE_OPEN_SCOPE

class BgiCapabilities;
class BgiIndirectCommandEncoder;

using BgiUniquePtr = std::unique_ptr<class Bgi>;

/// \class Bgi
///
/// Bifröst Graphics Interface.
///

class Bgi
{
    Bgi();

    virtual ~Bgi();

    /// Submit one BgiCmds objects.
    /// Once the cmds object is submitted it cannot be re-used to record cmds.
    /// A call to SubmitCmds would usually result in the hgi backend submitting
    /// the cmd buffers of the cmds object(s) to the device queue.
    /// Derived classes can override _SubmitCmds to customize submission.
    /// Thread safety: This call is not thread-safe. Submission must happen on
    /// the main thread so we can continue to support the OpenGL platform. 
    /// See notes above.
    void SubmitCmds(
        BgiCmds* cmds, 
        BgiSubmitWaitType wait = BgiSubmitWaitTypeNoWait);

    /// Helper function to return a Bgi object for the current platform.
    /// For example on Linux this may return BgiGL while on macOS BgiMetal.
    /// Caller, usually the application, owns the lifetime of the Bgi object and
    /// the object is destroyed when the caller drops the unique ptr.
    /// Thread safety: Not thread safe.
    static BgiUniquePtr CreatePlatformDefaultBgi();

    /// Determine if Bgi instance can run on current hardware.
    /// Thread safety: This call is thread safe.
    virtual bool IsBackendSupported() const = 0;

    /// Constructs a temporary Bgi object for the current platform and calls
    /// the object's IsBackendSupported() function.
    /// Thread safety: Not thread safe.
    static bool IsSupported();

    /// Returns a GraphicsCmds object (for temporary use) that is ready to
    /// record draw commands. GraphicsCmds is a lightweight object that
    /// should be re-acquired each frame (don't hold onto it after EndEncoding).
    /// Thread safety: Each Bgi backend must ensure that a Cmds object can be
    /// created on the main thread, recorded into (exclusively) by one secondary
    /// thread and be submitted on the main thread. See notes above.
    virtual BgiGraphicsCmdsUniquePtr CreateGraphicsCmds(
        BgiGraphicsCmdsDesc const& desc) = 0;

    /// Returns a BlitCmds object (for temporary use) that is ready to execute
    /// resource copy commands. BlitCmds is a lightweight object that
    /// should be re-acquired each frame (don't hold onto it after EndEncoding).
    /// Thread safety: Each Bgi backend must ensure that a Cmds object can be
    /// created on the main thread, recorded into (exclusively) by one secondary
    /// thread and be submitted on the main thread. See notes above.
    virtual BgiBlitCmdsUniquePtr CreateBlitCmds() = 0;

    /// Returns a ComputeCmds object (for temporary use) that is ready to
    /// record dispatch commands. ComputeCmds is a lightweight object that
    /// should be re-acquired each frame (don't hold onto it after EndEncoding).
    /// Thread safety: Each Bgi backend must ensure that a Cmds object can be
    /// created on the main thread, recorded into (exclusively) by one secondary
    /// thread and be submitted on the main thread. See notes above.
    virtual BgiComputeCmdsUniquePtr CreateComputeCmds(
        BgiComputeCmdsDesc const& desc) = 0;

    /// Create a texture in rendering backend.
    /// Thread safety: Creation must happen on main thread. See notes above.
    virtual BgiTextureHandle CreateTexture(BgiTextureDesc const & desc) = 0;

    /// Destroy a texture in rendering backend.
    /// Thread safety: Destruction must happen on main thread. See notes above.
    virtual void DestroyTexture(BgiTextureHandle* texHandle) = 0;

    /// Create a texture view in rendering backend.
    /// A texture view aliases another texture's data.
    /// It is the responsibility of the client to ensure that the sourceTexture
    /// is not destroyed while the texture view is in use.
    /// Thread safety: Creation must happen on main thread. See notes above.
    virtual BgiTextureViewHandle CreateTextureView(
        BgiTextureViewDesc const & desc) = 0;

    /// Destroy a texture view in rendering backend.
    /// This will destroy the view's texture, but not the sourceTexture that
    /// was aliased by the view. The sourceTexture data remains unchanged.
    /// Thread safety: Destruction must happen on main thread. See notes above.
    virtual void DestroyTextureView(BgiTextureViewHandle* viewHandle) = 0;

    /// Create a sampler in rendering backend.
    /// Thread safety: Creation must happen on main thread. See notes above.
    virtual BgiSamplerHandle CreateSampler(BgiSamplerDesc const & desc) = 0;

    /// Destroy a sampler in rendering backend.
    /// Thread safety: Destruction must happen on main thread. See notes above.
    virtual void DestroySampler(BgiSamplerHandle* smpHandle) = 0;

    /// Create a buffer in rendering backend.
    /// Thread safety: Creation must happen on main thread. See notes above.
    virtual BgiBufferHandle CreateBuffer(BgiBufferDesc const & desc) = 0;

    /// Destroy a buffer in rendering backend.
    /// Thread safety: Destruction must happen on main thread. See notes above.
    virtual void DestroyBuffer(BgiBufferHandle* bufHandle) = 0;

    /// Create a new shader function.
    /// Thread safety: Creation must happen on main thread. See notes above.
    virtual BgiShaderFunctionHandle CreateShaderFunction(
        BgiShaderFunctionDesc const& desc) = 0;

    /// Destroy a shader function.
    /// Thread safety: Destruction must happen on main thread. See notes above.
    virtual void DestroyShaderFunction(
        BgiShaderFunctionHandle* shaderFunctionHandle) = 0;

    /// Create a new shader program.
    /// Thread safety: Creation must happen on main thread. See notes above.
    virtual BgiShaderProgramHandle CreateShaderProgram(
        BgiShaderProgramDesc const& desc) = 0;

    /// Destroy a shader program.
    /// Note that this does NOT automatically destroy the shader functions in
    /// the program since shader functions may be used by more than one program.
    /// Thread safety: Destruction must happen on main thread. See notes above.
    virtual void DestroyShaderProgram(
        BgiShaderProgramHandle* shaderProgramHandle) = 0;

    /// Create a new resource binding object.
    /// Thread safety: Creation must happen on main thread. See notes above.
    virtual BgiResourceBindingsHandle CreateResourceBindings(
        BgiResourceBindingsDesc const& desc) = 0;

    /// Destroy a resource binding object.
    /// Thread safety: Destruction must happen on main thread. See notes above.
    virtual void DestroyResourceBindings(
        BgiResourceBindingsHandle* resHandle) = 0;

    /// Create a new graphics pipeline state object.
    /// Thread safety: Creation must happen on main thread. See notes above.
    virtual BgiGraphicsPipelineHandle CreateGraphicsPipeline(
        BgiGraphicsPipelineDesc const& pipeDesc) = 0;

    /// Destroy a graphics pipeline state object.
    /// Thread safety: Destruction must happen on main thread. See notes above.
    virtual void DestroyGraphicsPipeline(
        BgiGraphicsPipelineHandle* pipeHandle) = 0;

    /// Create a new compute pipeline state object.
    /// Thread safety: Creation must happen on main thread. See notes above.
    virtual BgiComputePipelineHandle CreateComputePipeline(
        BgiComputePipelineDesc const& pipeDesc) = 0;

    /// Destroy a compute pipeline state object.
    /// Thread safety: Destruction must happen on main thread. See notes above.
    virtual void DestroyComputePipeline(BgiComputePipelineHandle* pipeHandle)=0;

    /// Return the name of the api (e.g. "OpenGL").
    /// Thread safety: This call is thread safe.
    virtual TfToken const& GetAPIName() const = 0;

    /// Returns the device-specific capabilities structure.
    /// Thread safety: This call is thread safe.
    virtual BgiCapabilities const* GetCapabilities() const = 0;

    /// Returns the device-specific indirect command buffer encoder
    /// or nullptr if not supported.
    /// Thread safety: This call is thread safe.
    virtual BgiIndirectCommandEncoder* GetIndirectCommandEncoder() const = 0;

    /// Optionally called by client app at the start of a new rendering frame.
    /// We can't rely on StartFrame for anything important, because it is up to
    /// the external client to (optionally) call this and they may never do.
    /// Hydra doesn't have a clearly defined start or end frame.
    /// This can be helpful to insert GPU frame debug markers.
    /// Thread safety: Not thread safe. Should be called on the main thread.
    virtual void StartFrame() = 0;

    /// Optionally called at the end of a rendering frame.
    /// Please read the comments in StartFrame.
    /// Thread safety: Not thread safe. Should be called on the main thread.
    virtual void EndFrame() = 0;

protected:
    // Returns a unique id for handle creation.
    // Thread safety: Thread-safe atomic increment.
    uint64_t GetUniqueId();

    // Calls Submit on provided Cmds.
    // Derived classes can override this function if they need customize the
    // command submission. The default implementation calls cmds->_Submit().
    virtual bool _SubmitCmds(
        BgiCmds* cmds, BgiSubmitWaitType wait);

private:
    Bgi & operator=(const Bgi&) = delete;
    Bgi(const Bgi&) = delete;

    std::atomic<uint64_t> _uniqueIdCounter;
};

///
/// Bgi factory for plugin system
///
class BgiFactoryBase {
public:
    virtual Bgi* New() const = 0;
    virtual ~BgiFactoryBase();
};

template <class T>
class BgiFactory : public BgiFactoryBase {
public:
    Bgi* New() const {
        return new T;
    }
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_BGI_H