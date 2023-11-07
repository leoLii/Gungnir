#pragma once

#include "core/base.h"

#include "driver/bgiBase/api.h"
#include "driver/bgiBase/enums.h"
#include "driver/bgiBase/handle.h"
#include "driver/bgiBase/types.h"
#include "driver/bgiBase/shaderFunctionDesc.h"

#include <vector>
#include <string>

GUNGNIR_NAMESPACE_OPEN_SCOPE

///
/// \class HgiShaderFunction
///
/// Represents one shader stage function (code snippet).
///
/// ShaderFunctions are usually passed to a ShaderProgram, however be careful
/// not to destroy the ShaderFunction after giving it to the program.
/// While this may be safe for OpenGL after the program is created, it does not
/// apply to other graphics backends, such as Vulkan, where the shader functions
/// are used during rendering.
///
class BgiShaderFunction
{
public:
    BGI_API
    virtual ~BgiShaderFunction();

    /// The descriptor describes the object.
    BGI_API
    BgiShaderFunctionDesc const& GetDescriptor() const;

    /// Returns false if any shader compile errors occured.
    BGI_API
    virtual bool IsValid() const = 0;

    /// Returns shader compile errors.
    BGI_API
    virtual std::string const& GetCompileErrors() = 0;

    /// Returns the byte size of the GPU shader function.
    /// This can be helpful if the application wishes to tally up memory usage.
    BGI_API
    virtual size_t GetByteSizeOfResource() const = 0;

    /// This function returns the handle to the Hgi backend's gpu resource, cast
    /// to a uint64_t. Clients should avoid using this function and instead
    /// use Hgi base classes so that client code works with any Hgi platform.
    /// For transitioning code to Hgi, it can however we useful to directly
    /// access a platform's internal resource handles.
    /// There is no safety provided in using this. If you by accident pass a
    /// HgiMetal resource into an OpenGL call, bad things may happen.
    /// In OpenGL this returns the GLuint resource name.
    /// In Metal this returns the id<MTLFunction> as uint64_t.
    /// In Vulkan this returns the VkShaderModule as uint64_t.
    /// In DX12 this returns the ID3D12Resource pointer as uint64_t.
    BGI_API
    virtual uint64_t GetRawResource() const = 0;

protected:
    BGI_API
    BgiShaderFunction(BgiShaderFunctionDesc const& desc);

    BgiShaderFunctionDesc _descriptor;

private:
    BgiShaderFunction() = delete;
    BgiShaderFunction & operator=(const BgiShaderFunction&) = delete;
    BgiShaderFunction(const BgiShaderFunction&) = delete;
};

using BgiShaderFunctionHandle = BgiHandle<class BgiShaderFunction>;
using BgiShaderFunctionHandleVector = std::vector<BgiShaderFunctionHandle>;

GUNGNIR_NAMESPACE_CLOSE_SCOPE
