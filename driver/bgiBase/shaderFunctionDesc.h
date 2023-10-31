#ifndef GUNGNIR_DRIVER_BASE_SHADER_FUNCTION_DESC_H
#define GUNGNIR_DRIVER_BASE_SHADER_FUNCTION_DESC_H

#include "core/base.h"

#include "driver/bgiBase/api.h"
#include "driver/bgiBase/enum.h"
#include "driver/bgiBase/types.h"

#include <string>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

/// \struct HgiShaderFunctionTextureDesc
///
/// Describes a texture to be passed into a shader
///
/// <ul>
/// <li>nameInShader:
///   The name written from the codegen into shader file for the texture.</li>
/// <li>dimensions:
///   1d, 2d or 3d texture declaration.</li>
/// <li>format
///   Format of the texture. This is required in APIs where sampler types depend
///   on the texture (e.g., GL) </li>
/// <li>textureType:
///   Type of the texture (e.g. array texture, shadow, etc.).</li>
/// <li>bindIndex:
///   The index of the resource.</li>
/// <li>arraySize:
///   If arraySize > 0, indicates the size of the array. Note that textureType 
///   = HgiTextureTypeTexture and arraySize = 2 would create an array with two 
///   textures whereas textureType  = HgiTextureTypeArrayTexture and arraySize 
///   = 0 would create a single array texture (arrays of array textures are 
///   supported as well).</li>
/// <li>writable
///   Whether the texture is writable.</li>
/// </ul>
///
struct BgiShaderFunctionTextureDesc
{
    BGI_API
    BgiShaderFunctionTextureDesc();

    std::string nameInShader;
    uint32_t dimensions;
    BgiFormat format;
    BgiShaderTextureType textureType;
    uint32_t bindIndex;
    size_t arraySize;
    bool writable;
};

using BgiShaderFunctionTextureDescVector =
    std::vector<BgiShaderFunctionTextureDesc>;

BGI_API
bool operator==(
    const BgiShaderFunctionTextureDesc& lhs,
    const BgiShaderFunctionTextureDesc& rhs);

BGI_API
bool operator!=(
    const BgiShaderFunctionTextureDesc& lhs,
    const BgiShaderFunctionTextureDesc& rhs);

/// \struct HgiShaderFunctionBufferDesc
///
/// Describes a buffer to be passed into a shader
///
/// <ul>
/// <li>nameInShader:
///   The name written from the codegen into shader file for the texture.</li>
/// <li>type:
///   Type of the param within the shader file.</li>
/// <li>bindIndex:
///   The index of the resource.</li>
/// <li>arraySize:
///   The size of the array when binding is a HgiBindingTypeArray.</li>
/// <li>bindingType:
///   The binding model to use to expose the buffer to the shader.</li>
/// <li>writeable:
///   Whether the resource is writable.</li>
/// </ul>
///
struct BgiShaderFunctionBufferDesc
{
    BGI_API
    BgiShaderFunctionBufferDesc();

    std::string nameInShader;
    std::string type;
    uint32_t bindIndex;
    uint32_t arraySize;
    BgiBindingType binding;
    bool writable;
};

using BgiShaderFunctionBufferDescVector =
    std::vector<BgiShaderFunctionBufferDesc>;

BGI_API
bool operator==(
    const BgiShaderFunctionBufferDesc& lhs,
    const BgiShaderFunctionBufferDesc& rhs);

BGI_API
bool operator!=(
    const BgiShaderFunctionBufferDesc& lhs,
    const BgiShaderFunctionBufferDesc& rhs);

/// \struct HgiShaderFunctionParamDesc
///
/// Describes a param passed into a shader or between shader stages.
///
/// <ul>
/// <li>nameInShader:
///   The name written from the codegen into the shader file for the param.</li>
/// <li>type:
///   Type of the param within the shader file.</li>
/// <li>location:
///   For OpenGL, optionally specify the layout location.
///   For Metal, if no role is specified, this generates the role</li>
/// <li>interstageSlot:
///   Optionally specify an index for interstage parameters.</li>
/// <li>interpolation:
///   Optionally specify the interpolation: Default, Flat or NoPerspective.</li>
/// <li>sampling:
///   Optionally specify the sampling: Default, Centroid or Sample.</li>
/// <li>storage:
///   Optionally specify the storage type: Default, Patch.</li>
/// <li>role:
///   Optionally a role can be specified, like position, uv, color.</li>
/// <li>arraySize:
///   If specified, generates an array type parameter with given size.</li>
/// </ul>
///
struct BgiShaderFunctionParamDesc
{
    BGI_API
    BgiShaderFunctionParamDesc();

    std::string nameInShader;
    std::string type;
    int32_t location;
    int32_t interstageSlot;
    BgiInterpolationType interpolation;
    BgiSamplingType sampling;
    BgiStorageType storage;
    std::string role;
    std::string arraySize;
};

using BgiShaderFunctionParamDescVector =
    std::vector<BgiShaderFunctionParamDesc>;

BGI_API
bool operator==(
    const BgiShaderFunctionParamDesc& lhs,
    const BgiShaderFunctionParamDesc& rhs);

BGI_API
bool operator!=(
    const BgiShaderFunctionParamDesc& lhs,
    const BgiShaderFunctionParamDesc& rhs);

/// \struct HgiShaderFunctionParamBlockDesc
///
/// Describes an interstage param block between shader stages
///
/// <ul>
/// <li>blockName:
///   The name used to match blocks between shader stages.</li>
/// <li>instanceName:
///   The name used to scope access to block members.</li>
/// <li>members:
///   The members of the block.</li>
/// <li>arraySize:
///   If specified, generates a block with given size.</li>
/// <li>interstageSlot:
///   The interstage slot index of the first member of the block,
///   subsequent members are assigned sequential interstage slot indices.</li>
/// </ul>
///
struct BgiShaderFunctionParamBlockDesc
{
    BGI_API
    BgiShaderFunctionParamBlockDesc();

    struct Member {
        std::string name;
        std::string type;
    };
    using MemberVector = std::vector<Member>;

    std::string blockName;
    std::string instanceName;
    MemberVector members;
    std::string arraySize;
    int32_t interstageSlot;
};

using BgiShaderFunctionParamBlockDescVector =
    std::vector<BgiShaderFunctionParamBlockDesc>;

BGI_API
bool operator==(
    const BgiShaderFunctionParamBlockDesc& lhs,
    const BgiShaderFunctionParamBlockDesc& rhs);

BGI_API
bool operator!=(
    const BgiShaderFunctionParamBlockDesc& lhs,
    const BgiShaderFunctionParamBlockDesc& rhs);

BGI_API
bool operator==(
    const HBgiShaderFunctionParamBlockDesc::Member& lhs,
    const HBgiShaderFunctionParamBlockDesc::Member& rhs);

BGI_API
bool operator!=(
    const BgiShaderFunctionParamBlockDesc::Member& lhs,
    const BgiShaderFunctionParamBlockDesc::Member& rhs);

/// \struct HgiShaderFunctionComputeDesc
///
/// Describes a compute function's description
///
/// <ul>
/// <li>localSize:
///   Optional. Specifices the 3D size of the local thread grouping. Defaults to
///   0, meaning it is not set. When x > 0, y and z must also be set > 0. 
///   When localSize is set to > 0, the following source is generated:
///   GLSL: layout(local_size_x = localSize[0],
///      local_size_y = localSize[1], local_size_z = localSize[2]) in;
///   MSL: [[max_total_threads_per_threadgroup(localSize[0] * 
///      localSize[1] * localSize[w])]]
/// </li>
/// </ul>
///
struct BgiShaderFunctionComputeDesc
{
    BGI_API
    BgiShaderFunctionComputeDesc();

    GfVec3i localSize;
};

BGI_API
bool operator==(
        const BgiShaderFunctionComputeDesc& lhs,
        const BgiShaderFunctionComputeDesc& rhs);

BGI_API
bool operator!=(
        const BgiShaderFunctionComputeDesc& lhs,
        const BgiShaderFunctionComputeDesc& rhs);

/// \struct HgiShaderFunctionTessellationDesc
///
/// Describes a tessellation function's description
///
/// <ul>
/// <li>patchType:
///   The type of patch</li>
/// <li>spacing
///   The spacing used by the tessellation primitive generator</li>
/// <li>ordering
///   The ordering used by the tessellation primitive generator</li>
/// <li>numVertsInPerPatch:
///   The number of vertices in per patch</li>
/// <li>numVertsOutPerPatch:
///   The number of vertices out per patch</li>
/// </ul>
///
struct BgiShaderFunctionTessellationDesc
{
    enum class PatchType { Triangles, Quads, Isolines };
    enum class Spacing { Equal, FractionalEven, FractionalOdd };
    enum class Ordering { CW, CCW };
    BGI_API
    BgiShaderFunctionTessellationDesc();

    PatchType patchType;
    Spacing spacing;
    Ordering ordering;
    std::string numVertsPerPatchIn;
    std::string numVertsPerPatchOut;
};

BGI_API
bool operator==(
        const BgiShaderFunctionTessellationDesc& lhs,
        const BgiShaderFunctionTessellationDesc& rhs);

BGI_API
bool operator!=(
        const BgiShaderFunctionTessellationDesc& lhs,
        const BgiShaderFunctionTessellationDesc& rhs);

/// \struct HgiShaderFunctionGeometryDesc
///
/// Describes a geometry function's description
///
/// <ul>
/// <li>inPrimitiveType:
///   The input primitive type.</li>
/// <li>outPrimitiveType:
///   The output primitive type.</li>
/// <li>outMaxVertices:
///   The maximum number of vertices written by a single invovation of the 
///   geometry shader.</li>
/// </ul>
///
struct BgiShaderFunctionGeometryDesc
{
    enum class InPrimitiveType { 
        Points, Lines, LinesAdjacency, Triangles, TrianglesAdjacency };
    enum class OutPrimitiveType { 
        Points, LineStrip, TriangleStrip };

    BGI_API
    BgiShaderFunctionGeometryDesc();

    InPrimitiveType inPrimitiveType;
    OutPrimitiveType outPrimitiveType;
    std::string outMaxVertices;
};

BGI_API
bool operator==(
        const BgiShaderFunctionGeometryDesc& lhs,
        const BgiShaderFunctionGeometryDesc& rhs);

BGI_API
bool operator!=(
        const BgiShaderFunctionGeometryDesc& lhs,
        const BgiShaderFunctionGeometryDesc& rhs);

///
/// Describes a fragment function's description
///
/// <ul>
/// <li>earlyFragmentTests:
///   Fragment shader tests will be performed before fragment
///   shader execution when enabled.</li>
/// </ul>
///
struct BgiShaderFunctionFragmentDesc
{
    BGI_API
    BgiShaderFunctionFragmentDesc();

    bool earlyFragmentTests;

};

BGI_API
bool operator==(
        const BgiShaderFunctionFragmentDesc& lhs,
        const BgiShaderFunctionFragmentDesc& rhs);

BGI_API
bool operator!=(
        const BgiShaderFunctionFragmentDesc& lhs,
        const BgiShaderFunctionFragmentDesc& rhs);

/// \struct HgiShaderFunctionDesc
///
/// Describes the properties needed to create a GPU shader function.
///
/// <ul>
/// <li>debugName:
///   This label can be applied as debug label for gpu debugging.</li>
/// <li>shaderStage:
///   The shader stage this function represents.</li>
/// <li>shaderCodeDeclarations:
///   Optional ascii shader code containing defines and type declarations
///   which need to be emitted before generated resource bindings.</li>
/// <li>shaderCode:
///   The ascii shader code used to compile the shader.</li>
/// <li>generatedShaderCodeOut:
///   Optional pointer to a string that will be filled in with the
///   ascii shader code after shader generation is complete.</li>
/// <li>textures:
///   List of texture descriptions to be passed into a shader.</li>
/// <li>buffers:
///   List of buffer descriptions to be passed into a shader.</li>
/// <li>constantParams:
///   List of descriptions of constant params passed into a shader.</li>
/// <li>stageGlobalMembers:
///   List of descriptions of params declared at global scope.</li>
/// <li>stageInputs:
///   List of descriptions of the inputs of the shader.</li>
/// <li>stageOutputs:
///   List of descriptions of the outputs of the shader.</li>
/// <li>stageInputBlocks:
///   List of descriptions of the input blocks of the shader.</li>
/// <li>stageOutputsBlocks:
///   List of descriptions of the output blocks of the shader.</li>
/// <li>computeDescriptor:
///   Description of compute shader function.</li>
/// <li>tessellationDescriptor:
///   Description of tessellation shader function.</li>
/// <li>geometryDescriptor:
///   Description of geometry shader function.</li>
/// <li>fragmentDescriptor:
///   Description of fragment shader function.</li>
/// </ul>
///
struct BgiShaderFunctionDesc
{
    BGI_API
    BgiShaderFunctionDesc();
    std::string debugName;
    BgiShaderStage shaderStage;
    const char *shaderCodeDeclarations;
    const char *shaderCode;
    std::string *generatedShaderCodeOut;
    std::vector<BgiShaderFunctionTextureDesc> textures;
    std::vector<BgiShaderFunctionBufferDesc> buffers;
    std::vector<BgiShaderFunctionParamDesc> constantParams;
    std::vector<BgiShaderFunctionParamDesc> stageGlobalMembers;
    std::vector<BgiShaderFunctionParamDesc> stageInputs;
    std::vector<BgiShaderFunctionParamDesc> stageOutputs;
    std::vector<BgiShaderFunctionParamBlockDesc> stageInputBlocks;
    std::vector<BgiShaderFunctionParamBlockDesc> stageOutputBlocks;
    BgiShaderFunctionComputeDesc computeDescriptor;
    BgiShaderFunctionTessellationDesc tessellationDescriptor;
    BgiShaderFunctionGeometryDesc geometryDescriptor;
    BgiShaderFunctionFragmentDesc fragmentDescriptor;
};

using BgiShaderFunctionDescVector =
    std::vector<BgiShaderFunctionDesc>;

BGI_API
bool operator==(
    const BgiShaderFunctionDesc& lhs,
    const BgiShaderFunctionDesc& rhs);

BGI_API
bool operator!=(
    const BgiShaderFunctionDesc& lhs,
    const BgiShaderFunctionDesc& rhs);

/// Adds texture descriptor to given shader function descriptor.
BGI_API
void
BgiShaderFunctionAddTexture(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const uint32_t bindIndex = 0,
    uint32_t dimensions = 2,
    const BgiFormat &format = BgiFormatFloat32Vec4,
    const BgiShaderTextureType textureType = BgiShaderTextureTypeTexture);

/// Adds array of textures descriptor to given shader function descriptor.
BGI_API
void
BgiShaderFunctionAddArrayOfTextures(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const uint32_t arraySize,
    const uint32_t bindIndex = 0,
    const uint32_t dimensions = 2,
    const BgiFormat &format = BgiFormatFloat32Vec4,
    const BgiShaderTextureType textureType = BgiShaderTextureTypeTexture);

/// Adds writable texture descriptor to given shader function descriptor.
BGI_API
void
BgiShaderFunctionAddWritableTexture(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const uint32_t bindIndex = 0,
    const uint32_t dimensions = 2,
    const BgiFormat &format = BgiFormatFloat32Vec4,
    const BgiShaderTextureType textureType = BgiShaderTextureTypeTexture);

/// Adds buffer descriptor to given shader function descriptor.
BGI_API
void
BgiShaderFunctionAddBuffer(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const std::string &type,
    const uint32_t bindIndex,
    BgiBindingType binding,
    const uint32_t arraySize = 0);

/// Adds buffer descriptor to given shader function descriptor.
BGI_API
void
BgiShaderFunctionAddWritableBuffer(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const std::string &type,
    const uint32_t bindIndex);

/// Adds constant function param descriptor to given shader function
/// descriptor.
BGI_API
void
BgiShaderFunctionAddConstantParam(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const std::string &type,
    const std::string &role = std::string());

/// Adds stage input function param descriptor to given shader function
/// descriptor.
/// The location is will be set to the next available.
BGI_API
void
BgiShaderFunctionAddStageInput(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const std::string &type,
    const std::string &role = std::string());

/// Adds stage input function param descriptor to given shader function
/// descriptor given param descriptor.
BGI_API
void
BgiShaderFunctionAddStageInput(
        BgiShaderFunctionDesc *functionDesc,
        BgiShaderFunctionParamDesc const &paramDesc);

/// Interstage input.
BGI_API
void
BgiShaderFunctionAddGlobalVariable(
   BgiShaderFunctionDesc *desc,
   const std::string &nameInShader,
   const std::string &type,
   const std::string &arraySize);

/// Adds stage output function param descriptor to given shader function
/// descriptor.
BGI_API
void
BgiShaderFunctionAddStageOutput(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const std::string &type,
    const std::string &role = std::string());

/// Adds stage output function param descriptor to given shader function
/// descriptor.
BGI_API
void
BgiShaderFunctionAddStageOutput(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const std::string &type,
    const uint32_t location);

/// Adds stage output function param descriptor to given shader function
/// descriptor given param descriptor.
BGI_API
void
BgiShaderFunctionAddStageOutput(
        BgiShaderFunctionDesc *functionDesc,
        BgiShaderFunctionParamDesc const &paramDesc);

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_BASE_SHADER_FUNCTION_DESC_H
