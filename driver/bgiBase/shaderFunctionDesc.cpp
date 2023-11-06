#include "driver/bgiBase/shaderFunctionDesc.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiShaderFunctionTextureDesc::BgiShaderFunctionTextureDesc()
  : dimensions(2)
  , format(BgiFormatInvalid)
  , textureType(BgiShaderTextureTypeTexture)
  , bindIndex(0)
  , arraySize(0)
  , writable(false)
{
}

BgiShaderFunctionBufferDesc::BgiShaderFunctionBufferDesc()
  : bindIndex(0)
  , arraySize(0)
  , binding(BgiBindingTypeValue)
  , writable(false)
{
}

BgiShaderFunctionComputeDesc::BgiShaderFunctionComputeDesc()
    : localSize(Vector3i(0, 0, 0))
{ 
}

BgiShaderFunctionParamDesc::BgiShaderFunctionParamDesc()
  : location(-1)
  , interstageSlot(-1)
  , interpolation(BgiInterpolationDefault)
  , sampling(BgiSamplingDefault)
  , storage(BgiStorageDefault)
{
}

BgiShaderFunctionParamBlockDesc::BgiShaderFunctionParamBlockDesc()
  : interstageSlot(-1)
{
}

BgiShaderFunctionTessellationDesc::BgiShaderFunctionTessellationDesc()
  : patchType(PatchType::Triangles)
  , spacing(Spacing::Equal)
  , ordering(Ordering::CCW)
  , numVertsPerPatchIn("3")
  , numVertsPerPatchOut("3")
{
}

BgiShaderFunctionDesc::BgiShaderFunctionDesc() 
  : shaderStage(0)
  , shaderCodeDeclarations(nullptr)
  , shaderCode(nullptr)
  , generatedShaderCodeOut(nullptr)
  , textures()
  , constantParams()
  , stageInputs()
  , stageOutputs()
  , computeDescriptor()
  , tessellationDescriptor()
  , geometryDescriptor()
  , fragmentDescriptor()
{
}

bool operator==(
    const BgiShaderFunctionTextureDesc& lhs,
    const BgiShaderFunctionTextureDesc& rhs)
{
    return lhs.nameInShader == rhs.nameInShader &&
           lhs.dimensions == rhs.dimensions &&
           lhs.format == rhs.format &&
           lhs.textureType == rhs.textureType &&
           lhs.arraySize == rhs.arraySize &&
           lhs.writable == rhs.writable;
}

bool operator!=(
    const BgiShaderFunctionTextureDesc& lhs,
    const BgiShaderFunctionTextureDesc& rhs)
{
    return !(lhs == rhs);
}

BgiShaderFunctionGeometryDesc::BgiShaderFunctionGeometryDesc() 
  : inPrimitiveType(InPrimitiveType::Triangles)
  , outPrimitiveType(OutPrimitiveType::TriangleStrip)
  , outMaxVertices("3")
{
}

bool operator==(
    const BgiShaderFunctionGeometryDesc& lhs,
    const BgiShaderFunctionGeometryDesc& rhs)
{
    return lhs.inPrimitiveType == rhs.inPrimitiveType &&
           lhs.outPrimitiveType == rhs.outPrimitiveType &&
           lhs.outMaxVertices == rhs.outMaxVertices;
}

bool operator!=(
    const BgiShaderFunctionGeometryDesc& lhs,
    const BgiShaderFunctionGeometryDesc& rhs)
{
    return !(lhs == rhs);
}

BgiShaderFunctionFragmentDesc::BgiShaderFunctionFragmentDesc()
    : earlyFragmentTests(false)
{
}

bool operator==(
    const BgiShaderFunctionFragmentDesc& lhs,
    const BgiShaderFunctionFragmentDesc& rhs)
{
    return lhs.earlyFragmentTests == rhs.earlyFragmentTests;
}

bool operator!=(
    const BgiShaderFunctionFragmentDesc& lhs,
    const BgiShaderFunctionFragmentDesc& rhs)
{
    return !(lhs == rhs);
}


bool operator==(
    const BgiShaderFunctionBufferDesc& lhs,
    const BgiShaderFunctionBufferDesc& rhs)
{
    return lhs.nameInShader == rhs.nameInShader &&
           lhs.type == rhs.type &&
           lhs.bindIndex == rhs.bindIndex &&
           lhs.arraySize == rhs.arraySize &&
           lhs.binding == rhs.binding &&
           lhs.writable == rhs.writable;
}

bool operator!=(
    const BgiShaderFunctionBufferDesc& lhs,
    const BgiShaderFunctionBufferDesc& rhs)
{
    return !(lhs == rhs);
}

bool operator==(
    const BgiShaderFunctionParamDesc& lhs,
    const BgiShaderFunctionParamDesc& rhs)
{
    return lhs.nameInShader == rhs.nameInShader &&
           lhs.type == rhs.type && 
           lhs.location == rhs.location &&
           lhs.interstageSlot == rhs.interstageSlot &&
           lhs.interpolation == rhs.interpolation &&
           lhs.sampling == rhs.sampling &&
           lhs.storage == rhs.storage &&
           lhs.role == rhs.role &&
           lhs.arraySize == rhs.arraySize;
}

bool operator!=(
    const BgiShaderFunctionParamDesc& lhs,
    const BgiShaderFunctionParamDesc& rhs)
{
    return !(lhs == rhs);
}

bool operator==(
    const BgiShaderFunctionParamBlockDesc& lhs,
    const BgiShaderFunctionParamBlockDesc& rhs)
{
    return lhs.blockName == rhs.blockName &&
           lhs.instanceName == rhs.instanceName &&
           lhs.members == rhs.members &&
           lhs.arraySize == rhs.arraySize &&
           lhs.interstageSlot == rhs.interstageSlot;
}

bool operator!=(
    const BgiShaderFunctionParamBlockDesc& lhs,
    const BgiShaderFunctionParamBlockDesc& rhs)
{
    return !(lhs == rhs);
}

bool operator==(
    const BgiShaderFunctionParamBlockDesc::Member& lhs,
    const BgiShaderFunctionParamBlockDesc::Member& rhs)
{
    return lhs.name == rhs.name &&
           lhs.type == rhs.type;
}

bool operator!=(
    const BgiShaderFunctionParamBlockDesc::Member& lhs,
    const BgiShaderFunctionParamBlockDesc::Member& rhs)
{
    return !(lhs == rhs);
}

bool operator==(
        const BgiShaderFunctionComputeDesc& lhs,
        const BgiShaderFunctionComputeDesc& rhs)
{
    return lhs.localSize == rhs.localSize;
}

bool operator!=(
    const BgiShaderFunctionComputeDesc& lhs,
    const BgiShaderFunctionComputeDesc& rhs)
{
    return !(lhs == rhs);
}

bool operator==(
        const BgiShaderFunctionTessellationDesc& lhs,
        const BgiShaderFunctionTessellationDesc& rhs)
{
    return lhs.patchType == rhs.patchType &&
           lhs.spacing == rhs.spacing &&
           lhs.ordering == rhs.ordering &&
           lhs.numVertsPerPatchIn == rhs.numVertsPerPatchIn &&
           lhs.numVertsPerPatchOut == rhs.numVertsPerPatchOut;
}

bool operator!=(
    const BgiShaderFunctionTessellationDesc& lhs,
    const BgiShaderFunctionTessellationDesc& rhs)
{
    return !(lhs == rhs);
}

bool operator==(
    const BgiShaderFunctionDesc& lhs,
    const BgiShaderFunctionDesc& rhs)
{
    return lhs.debugName == rhs.debugName &&
           lhs.shaderStage == rhs.shaderStage &&
           // Omitted. Only used tmp during shader compile
           // lhs.shaderCodeDeclarations == rhs.shaderCodeDeclarations
           // lhs.shaderCode == rhs.shaderCode
           // lhs.generatedShaderCodeOut == rhs.generatedShaderCodeOut
           lhs.textures == rhs.textures &&
           lhs.constantParams == rhs.constantParams &&
           lhs.stageInputs == rhs.stageInputs &&
           lhs.stageOutputs == rhs.stageOutputs &&
           lhs.computeDescriptor == rhs.computeDescriptor &&
           lhs.tessellationDescriptor == rhs.tessellationDescriptor &&
           lhs.geometryDescriptor == rhs.geometryDescriptor &&
           lhs.fragmentDescriptor == rhs.fragmentDescriptor;
}

bool operator!=(
    const BgiShaderFunctionDesc& lhs,
    const BgiShaderFunctionDesc& rhs)
{
    return !(lhs == rhs);
}

void
BgiShaderFunctionAddTexture(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const uint32_t bindIndex /* = 0 */,
    const uint32_t dimensions /* = 2 */,
    const BgiFormat &format /* = BgiFormatFloat32Vec4*/,
    const BgiShaderTextureType textureType /* = BgiShaderTextureTypeTexture */)
{
    BgiShaderFunctionTextureDesc texDesc;
    texDesc.nameInShader = nameInShader;
    texDesc.bindIndex = bindIndex;
    texDesc.dimensions = dimensions;
    texDesc.format = format;
    texDesc.textureType = textureType;
    texDesc.arraySize = 0;
    texDesc.writable = false;

    desc->textures.push_back(std::move(texDesc));
}

void
BgiShaderFunctionAddArrayOfTextures(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const uint32_t arraySize,
    const uint32_t bindIndex /* = 0 */,
    const uint32_t dimensions /* = 2 */,
    const BgiFormat &format /* = BgiFormatFloat32Vec4*/,
    const BgiShaderTextureType textureType /* = BgiShaderTextureTypeTexture */)
{
    BgiShaderFunctionTextureDesc texDesc;
    texDesc.nameInShader = nameInShader;
    texDesc.bindIndex = bindIndex;
    texDesc.dimensions = dimensions;
    texDesc.format = format;
    texDesc.textureType = textureType;
    texDesc.arraySize = arraySize;
    texDesc.writable = false;

    desc->textures.push_back(std::move(texDesc));
}

void
BgiShaderFunctionAddWritableTexture(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const uint32_t bindIndex /* = 0 */,
    const uint32_t dimensions /* = 2 */,
    const BgiFormat &format /* = BgiFormatFloat32Vec4*/,
    const BgiShaderTextureType textureType /* = BgiShaderTextureTypeTexture */)
{
    BgiShaderFunctionTextureDesc texDesc;
    texDesc.nameInShader = nameInShader;
    texDesc.bindIndex = bindIndex;
    texDesc.dimensions = dimensions;
    texDesc.format = format;
    texDesc.textureType = textureType;
    texDesc.arraySize = 0;
    texDesc.writable = true;

    desc->textures.push_back(std::move(texDesc));
}

void
BgiShaderFunctionAddBuffer(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const std::string &type,
    const uint32_t bindIndex,
    const BgiBindingType binding,
    const uint32_t arraySize)
{
    BgiShaderFunctionBufferDesc bufDesc;
    bufDesc.nameInShader = nameInShader;
    bufDesc.type = type;
    bufDesc.binding = binding;
    bufDesc.arraySize = arraySize;
    bufDesc.bindIndex = bindIndex;
    bufDesc.writable = false;

    desc->buffers.push_back(std::move(bufDesc));
}

void
BgiShaderFunctionAddWritableBuffer(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const std::string &type,
    const uint32_t bindIndex)
{
    BgiShaderFunctionBufferDesc bufDesc;
    bufDesc.nameInShader = nameInShader;
    bufDesc.type = type;
    bufDesc.bindIndex = bindIndex;
    bufDesc.binding = BgiBindingTypePointer;
    bufDesc.writable = true;

    desc->buffers.push_back(std::move(bufDesc));
}

void
BgiShaderFunctionAddConstantParam(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const std::string &type,
    const SHADER_KEYWORD &role)
{
    BgiShaderFunctionParamDesc paramDesc;
    paramDesc.nameInShader = nameInShader;
    paramDesc.type = type;
    paramDesc.role = role;
    
    desc->constantParams.push_back(std::move(paramDesc));
}

void
BgiShaderFunctionAddStageInput(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const std::string &type,
    const SHADER_KEYWORD &role)
{
    BgiShaderFunctionParamDesc paramDesc;
    paramDesc.nameInShader = nameInShader;
    paramDesc.type = type;
    paramDesc.role = role;

    desc->stageInputs.push_back(std::move(paramDesc));
}

void
BgiShaderFunctionAddStageInput(
        BgiShaderFunctionDesc *functionDesc,
        BgiShaderFunctionParamDesc const &paramDesc)
{
    functionDesc->stageInputs.push_back(paramDesc);
}

void
BgiShaderFunctionAddGlobalVariable(
   BgiShaderFunctionDesc *desc,
   const std::string &nameInShader,
   const std::string &type,
   const std::string &arraySize)
{
    BgiShaderFunctionParamDesc paramDesc;
    paramDesc.nameInShader = nameInShader;
    paramDesc.type = type;
    paramDesc.arraySize = arraySize;
    desc->stageGlobalMembers.push_back(std::move(paramDesc));
}

void
BgiShaderFunctionAddStageOutput(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const std::string &type,
    const SHADER_KEYWORD &role)
{
    BgiShaderFunctionParamDesc paramDesc;
    paramDesc.nameInShader = nameInShader;
    paramDesc.type = type;
    paramDesc.role = role;

    desc->stageOutputs.push_back(std::move(paramDesc));
}

void
BgiShaderFunctionAddStageOutput(
    BgiShaderFunctionDesc *desc,
    const std::string &nameInShader,
    const std::string &type,
    const uint32_t location)
{
    BgiShaderFunctionParamDesc paramDesc;
    paramDesc.nameInShader = nameInShader;
    paramDesc.type = type;
    paramDesc.location = location;

    desc->stageOutputs.push_back(std::move(paramDesc));
}

void
BgiShaderFunctionAddStageOutput(
        BgiShaderFunctionDesc *functionDesc,
        BgiShaderFunctionParamDesc const &paramDesc)
{
    functionDesc->stageOutputs.push_back(paramDesc);
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
