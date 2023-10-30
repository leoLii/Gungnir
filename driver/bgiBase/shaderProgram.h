#ifndef GUNGNIR_DRIVER_BASE_SHADER_PROGRAM_H
#define GUNGNIR_DRIVER_BASE_SHADER_PROGRAM_H

#include "core/base.h"

#include "driver/bgiBase/api.h"
#include "driver/bgiBase/enums.h"
#include "driver/bgiBase/handle.h"
// #include "driver/bgiBase/shaderFunction.h"
#include "driver/bgiBase/types.h"

#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

/// \struct HgiShaderProgramDesc
///
/// Describes the properties needed to create a GPU shader program.
///
/// <ul>
/// <li>debugName:
///   This label can be applied as debug label for gpu debugging.</li>
/// <li>shaderFunctions:
///   Holds handles to shader functions for each shader stage.</li>
/// </ul>
///
struct BgiShaderProgramDesc
{
    BGI_API
    BgiShaderProgramDesc();

    std::string debugName;
};

BGI_API
bool operator==(
    const BgiShaderProgramDesc& lhs,
    const BgiShaderProgramDesc& rhs);

BGI_API
bool operator!=(
    const BgiShaderProgramDesc& lhs,
    const BgiShaderProgramDesc& rhs);

///
/// \class HgiShaderProgram
///
/// Represents a collection of shader functions.
/// This object does not take ownership of the shader functions and does not
/// destroy them automatically. The client must destroy the shader functions
/// when the program is detroyed, because only the client knows if the shader
/// functions are used by other shader programs.
///
class BgiShaderProgram
{
public:
    BGI_API
    virtual ~BgiShaderProgram();

    /// The descriptor describes the object.
    BGI_API
    BgiShaderProgramDesc const& GetDescriptor() const;

    /// Returns false if any shader compile errors occured.
    BGI_API
    virtual bool IsValid() const = 0;

    /// Returns shader compile errors.
    BGI_API
    virtual std::string const& GetCompileErrors() = 0;

    /// Returns the shader functions that are part of this program.
    //BGI_API
    //virtual HgiShaderFunctionHandleVector const& GetShaderFunctions() const = 0;

    /// Returns the byte size of the GPU shader program.
    /// APIs that do not have programs can return the combined byte size of the
    /// shader functions used by the program.
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
    /// In Metal, Vulkan this returns 0.
    BGI_API
    virtual uint64_t GetRawResource() const = 0;

protected:
    BGI_API
    BgiShaderProgram(BgiShaderProgramDesc const& desc);

    BgiShaderProgramDesc _descriptor;

private:
    BgiShaderProgram() = delete;
    BgiShaderProgram & operator=(const BgiShaderProgram&) = delete;
    BgiShaderProgram(const BgiShaderProgram&) = delete;
};

using BgiShaderProgramHandle = BgiHandle<class BgiShaderProgram>;
using BgiShaderProgramHandleVector = std::vector<BgiShaderProgramHandle>;

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_BASE_SHADER_PROGRAM_H
