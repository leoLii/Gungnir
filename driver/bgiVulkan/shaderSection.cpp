#include "core/utils/diagnostic.h"

#include "driver/bgiVulkan/shaderSection.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiVulkanShaderSection::BgiVulkanShaderSection(
    const std::string &identifier,
    const BgiShaderSectionAttributeVector &attributes,
    const std::string &storageQualifier,
    const std::string &defaultValue,
    const std::string &arraySize,
    const std::string &blockInstanceIdentifier)
  : BgiShaderSection(identifier, attributes, defaultValue,
                     arraySize, blockInstanceIdentifier)
  , _storageQualifier(storageQualifier)
  , _arraySize(arraySize)
{
}

BgiVulkanShaderSection::~BgiVulkanShaderSection() = default;

void
BgiVulkanShaderSection::WriteDeclaration(std::ostream &ss) const
{
    //If it has attributes, write them with corresponding layout
    //identifiers and indicies
    const BgiShaderSectionAttributeVector &attributes = GetAttributes();

    if(!attributes.empty()) {
        ss << "layout(";
        for (size_t i = 0; i < attributes.size(); ++i) {
            const BgiShaderSectionAttribute &a = attributes[i];
            if (i > 0) {
                ss << ", ";
            }
            ss << a.identifier;
            if(!a.index.empty()) {
                ss << " = " << a.index;
            }
        }
        ss << ") ";
    }
    //If it has a storage qualifier, declare it
    if(!_storageQualifier.empty()) {
        ss << _storageQualifier << " ";
    }
    WriteType(ss);
    ss << " ";
    WriteIdentifier(ss);
    WriteArraySize(ss);
    ss << ";\n";
}

void
BgiVulkanShaderSection::WriteParameter(std::ostream &ss) const
{
    WriteType(ss);
    ss << " ";
    WriteIdentifier(ss);
    ss << ";";
}

bool
BgiVulkanShaderSection::VisitGlobalIncludes(std::ostream &ss)
{
    return false;
}

bool
BgiVulkanShaderSection::VisitGlobalMacros(std::ostream &ss)
{
    return false;
}

bool
BgiVulkanShaderSection::VisitGlobalStructs(std::ostream &ss)
{
    return false;
}

bool BgiVulkanShaderSection::VisitGlobalMemberDeclarations(std::ostream &ss)
{
    return false;
}

bool
BgiVulkanShaderSection::VisitGlobalFunctionDefinitions(std::ostream &ss)
{
    return false;
}

BgiVulkanMacroShaderSection::BgiVulkanMacroShaderSection(
    const std::string &macroDeclaration,
    const std::string &macroComment)
  : BgiVulkanShaderSection(macroDeclaration)
  , _macroComment(macroComment)
{
}

BgiVulkanMacroShaderSection::~BgiVulkanMacroShaderSection() = default;

bool
BgiVulkanMacroShaderSection::VisitGlobalMacros(std::ostream &ss)
{
    WriteIdentifier(ss);
    return true;
}

BgiVulkanMemberShaderSection::BgiVulkanMemberShaderSection(
    const std::string &identifier,
    const std::string &typeName,
    const BgiInterpolationType interpolation,
    const BgiSamplingType sampling,
    const BgiStorageType storage,
    const BgiShaderSectionAttributeVector &attributes,
    const std::string &storageQualifier,
    const std::string &defaultValue,
    const std::string &arraySize,
    const std::string &blockInstanceIdentifier)
  : BgiVulkanShaderSection(identifier,
                           attributes,
                           storageQualifier,
                           defaultValue,
                           arraySize,
                           blockInstanceIdentifier)
  , _typeName(typeName)
  , _interpolation(interpolation)
  , _sampling(sampling)
  , _storage(storage)
{
}

BgiVulkanMemberShaderSection::~BgiVulkanMemberShaderSection() = default;

bool
BgiVulkanMemberShaderSection::VisitGlobalMemberDeclarations(std::ostream &ss)
{
    if (HasBlockInstanceIdentifier()) {
        return true;
    }

    switch (_interpolation) {
    case BgiInterpolationDefault:
        break;
    case BgiInterpolationFlat:
        ss << "flat ";
        break;
    case BgiInterpolationNoPerspective:
        ss << "noperspective ";
        break;
    }
    switch (_sampling) {
    case BgiSamplingDefault:
        break;
    case BgiSamplingCentroid:
        ss << "centroid ";
        break;
    case BgiSamplingSample:
        ss << "sample ";
        break;
    }
    switch (_storage) {
    case BgiStorageDefault:
        break;
    case BgiStoragePatch:
        ss << "patch ";
        break;
    }
    WriteDeclaration(ss);
    return true;
}

void
BgiVulkanMemberShaderSection::WriteType(std::ostream& ss) const
{
    ss << _typeName;
}

BgiVulkanBlockShaderSection::BgiVulkanBlockShaderSection(
    const std::string &identifier,
    const BgiShaderFunctionParamDescVector &parameters)
  : BgiVulkanShaderSection(identifier)
  , _parameters(parameters)
{
}

BgiVulkanBlockShaderSection::~BgiVulkanBlockShaderSection() = default;

bool
BgiVulkanBlockShaderSection::VisitGlobalMemberDeclarations(std::ostream &ss)
{
    ss << "layout(push_constant) " << "uniform" << " ";
    WriteIdentifier(ss);
    ss << "\n";
    ss << "{\n";
    for(const BgiShaderFunctionParamDesc &param : _parameters) {
        ss << "    " << param.type << " " << param.nameInShader << ";\n";
    }
    ss << "\n};\n";
    return true;
}

const std::string BgiVulkanTextureShaderSection::_storageQualifier = "uniform";

BgiVulkanTextureShaderSection::BgiVulkanTextureShaderSection(
    const std::string &identifier,
    const unsigned int layoutIndex,
    const unsigned int dimensions,
    const BgiFormat format,
    const BgiShaderTextureType textureType,
    const uint32_t arraySize,
    const bool writable,
    const BgiShaderSectionAttributeVector &attributes,
    const std::string &defaultValue)
  : BgiVulkanShaderSection( identifier,
                        attributes,
                        _storageQualifier,
                        defaultValue,
                        arraySize > 0 ? 
                        "[" + std::to_string(arraySize) + "]" :
                        "")
  , _dimensions(dimensions)
  , _format(format)
  , _textureType(textureType)
  , _arraySize(arraySize)
  , _writable(writable)
{
}

BgiVulkanTextureShaderSection::~BgiVulkanTextureShaderSection() = default;

static std::string
_GetTextureTypePrefix(BgiFormat const &format)
{
    if (format >= BgiFormatUInt16 && format <= BgiFormatUInt16Vec4) {
        return "u"; // e.g., usampler, uvec4
    }
    if (format >= BgiFormatInt32 && format <= BgiFormatInt32Vec4) {
        return "i"; // e.g., isampler, ivec4
    }
    return ""; // e.g., sampler, vec4
}

void
BgiVulkanTextureShaderSection::_WriteSamplerType(std::ostream &ss) const
{
    if (_writable) {
        if (_textureType == BgiShaderTextureTypeArrayTexture) {
            ss << "image" << _dimensions << "DArray";
        } else {
            ss << "image" << _dimensions << "D";
        }
    } else {
        if (_textureType == BgiShaderTextureTypeShadowTexture) {
            ss << _GetTextureTypePrefix(_format) << "sampler"
               << _dimensions << "DShadow";
        } else if (_textureType == BgiShaderTextureTypeArrayTexture) {
            ss << _GetTextureTypePrefix(_format) << "sampler" 
               << _dimensions << "DArray";
        } else {
            ss << _GetTextureTypePrefix(_format) << "sampler" 
               << _dimensions << "D";
        }
    }
}

void
BgiVulkanTextureShaderSection::_WriteSampledDataType(std::ostream &ss) const
{
    if (_textureType == BgiShaderTextureTypeShadowTexture) {
        ss << "float";
    } else {
        ss << _GetTextureTypePrefix(_format) << "vec4";
    }
}

void
BgiVulkanTextureShaderSection::WriteType(std::ostream &ss) const
{
    if(_dimensions < 1 || _dimensions > 3) {
        UTILS_CODING_ERROR("Invalid texture dimension");
    }
    _WriteSamplerType(ss); // e.g. sampler<N>D, isampler<N>D, usampler<N>D
}

bool
BgiVulkanTextureShaderSection::VisitGlobalMemberDeclarations(std::ostream &ss)
{
    WriteDeclaration(ss);
    return true;
}

bool
BgiVulkanTextureShaderSection::VisitGlobalFunctionDefinitions(std::ostream &ss)
{
    // Used to unify texture sampling and writing across platforms that depend 
    // on samplers and don't store textures in global space.
    const uint32_t sizeDim = 
        (_textureType == BgiShaderTextureTypeArrayTexture) ? 
        (_dimensions + 1) : _dimensions;
    const uint32_t coordDim = 
        (_textureType == BgiShaderTextureTypeShadowTexture ||
         _textureType == BgiShaderTextureTypeArrayTexture) ? 
        (_dimensions + 1) : _dimensions;

    const std::string sizeType = sizeDim == 1 ? 
        "int" :
        "ivec" + std::to_string(sizeDim);
    const std::string intCoordType = coordDim == 1 ? 
        "int" :
        "ivec" + std::to_string(coordDim);
    const std::string floatCoordType = coordDim == 1 ? 
        "float" :
        "vec" + std::to_string(coordDim);

    if (_arraySize > 0) {
        WriteType(ss);
        ss << " BgiGetSampler_";
        WriteIdentifier(ss);
        ss << "(uint index) {\n";
        ss << "    return ";
        WriteIdentifier(ss);
        ss << "[index];\n}\n";
    } else {
        ss << "#define BgiGetSampler_";
        WriteIdentifier(ss);
        ss << "() ";
        WriteIdentifier(ss);
        ss << "\n";
    }

    if (_writable) {
        // Write a function that lets you write to the texture with 
        // HgiSet_texName(uv, data).
        ss << "void BgiSet_";
        WriteIdentifier(ss);
        ss << "(" << intCoordType << " uv, vec4 data) {\n";
        ss << "    ";
        ss << "imageStore(";
        WriteIdentifier(ss);
        ss << ", uv, data);\n";
        ss << "}\n";

        // HgiGetSize_texName()
        ss << sizeType << " BgiGetSize_";
        WriteIdentifier(ss);
        ss << "() {\n";
        ss << "    ";
        ss << "return imageSize(";
        WriteIdentifier(ss);
        ss << ");\n";
        ss << "}\n";
    } else {
        const std::string arrayInput = (_arraySize > 0) ? "uint index, " : "";
        const std::string arrayIndex = (_arraySize > 0) ? "[index]" : "";
        
        // Write a function that lets you query the texture with 
        // HgiGet_texName(uv).
        _WriteSampledDataType(ss); // e.g., vec4, ivec4, uvec4
        ss << " BgiGet_";
        WriteIdentifier(ss);
        ss << "(" << arrayInput << floatCoordType << " uv) {\n";
        ss << "    ";
        _WriteSampledDataType(ss);
        ss << " result = texture(";
        WriteIdentifier(ss);
        ss << arrayIndex << ", uv);\n";
        ss << "    return result;\n";
        ss << "}\n";
        
        // HgiGetSize_texName()
        ss << sizeType << " BgiGetSize_";
        WriteIdentifier(ss);
        ss << "(" << ((_arraySize > 0) ? "uint index" : "")  << ") {\n";
        ss << "    ";
        ss << "return textureSize(";
        WriteIdentifier(ss);
        ss << arrayIndex << ", 0);\n";
        ss << "}\n";

        // HgiTextureLod_texName()
        _WriteSampledDataType(ss);
        ss << " BgiTextureLod_";
        WriteIdentifier(ss);
        ss << "(" << arrayInput << floatCoordType << " coord, float lod) {\n";
        ss << "    ";
        ss << "return textureLod(";
        WriteIdentifier(ss);
        ss << arrayIndex << ", coord, lod);\n";
        ss << "}\n";
        
        // HgiTexelFetch_texName()
        if (_textureType != BgiShaderTextureTypeShadowTexture) {
            _WriteSampledDataType(ss);
            ss << " BgiTexelFetch_";
            WriteIdentifier(ss);
            ss << "(" << arrayInput << intCoordType << " coord) {\n";
            ss << "    ";
            _WriteSampledDataType(ss);
            ss << " result = texelFetch(";
            WriteIdentifier(ss);
            ss << arrayIndex << ", coord, 0);\n";
            ss << "    return result;\n";
            ss << "}\n";
        }
    }

    return true;
}

BgiVulkanBufferShaderSection::BgiVulkanBufferShaderSection(
    const std::string &identifier,
    const uint32_t layoutIndex,
    const std::string &type,
    const BgiBindingType binding,
    const std::string arraySize,
    const bool writable,
    const BgiShaderSectionAttributeVector &attributes)
  : BgiVulkanShaderSection( identifier,
                            attributes,
                            "buffer",
                            "")
  , _type(type)
  , _binding(binding)
  , _arraySize(arraySize)
  , _writable(writable)
{
}

BgiVulkanBufferShaderSection::~BgiVulkanBufferShaderSection() = default;

void
BgiVulkanBufferShaderSection::WriteType(std::ostream &ss) const
{
    ss << _type;
}

bool
BgiVulkanBufferShaderSection::VisitGlobalMemberDeclarations(std::ostream &ss)
{
    //If it has attributes, write them with corresponding layout
    //identifiers and indicies
    const BgiShaderSectionAttributeVector &attributes = GetAttributes();

    if(!attributes.empty()) {
        ss << "layout(";
        for (size_t i = 0; i < attributes.size(); i++)
        {
            if (i > 0) {
                ss << ", ";
            }
            const BgiShaderSectionAttribute &a = attributes[i];
            ss << a.identifier;
            if(!a.index.empty()) {
                ss << " = " << a.index;
            }
        }
        ss << ") ";
    }

    // If it has a storage qualifier, declare it
    if (_binding == BgiBindingTypeUniformValue ||
        _binding == BgiBindingTypeUniformArray) {
        ss << "uniform ubo_";
    } else {
        if (!_writable) {
            ss << "readonly ";
        }
        ss << "buffer ssbo_";
    }
    WriteIdentifier(ss);
    ss << " { ";
    WriteType(ss);
    ss << " ";
    WriteIdentifier(ss);

    if (_binding == BgiBindingTypeValue ||
        _binding == BgiBindingTypeUniformValue) {
        ss << "; };\n";
    } else {
        ss << "[" << _arraySize << "]; };\n";
    }

    return true;
}

BgiVulkanKeywordShaderSection::BgiVulkanKeywordShaderSection(
    const std::string &identifier,
    const std::string &type,
    const std::string &keyword)
  : BgiVulkanShaderSection(identifier)
  , _type(type)
  , _keyword(keyword)
{
}

BgiVulkanKeywordShaderSection::~BgiVulkanKeywordShaderSection() = default;

void
BgiVulkanKeywordShaderSection::WriteType(std::ostream &ss) const
{
    ss << _type;
}

bool
BgiVulkanKeywordShaderSection::VisitGlobalMemberDeclarations(std::ostream &ss)
{
    WriteType(ss);
    ss << " ";
    WriteIdentifier(ss);
    ss << " = ";
    ss << _keyword;
    ss << ";\n";

    return true;
}

BgiVulkanInterstageBlockShaderSection::BgiVulkanInterstageBlockShaderSection(
    const std::string &blockIdentifier,
    const std::string &blockInstanceIdentifier,
    const BgiShaderSectionAttributeVector &attributes,
    const std::string &qualifier,
    const std::string &arraySize,
    const BgiVulkanShaderSectionPtrVector &members)
    : BgiVulkanShaderSection(blockIdentifier,
                             attributes,
                             qualifier,
                             std::string(),
                             arraySize,
                             blockInstanceIdentifier)
    , _qualifier(qualifier)
    , _members(members)
{
}

bool
BgiVulkanInterstageBlockShaderSection::VisitGlobalMemberDeclarations(
    std::ostream &ss)
{
    // If it has attributes, write them with corresponding layout
    // identifiers and indices
    const BgiShaderSectionAttributeVector &attributes = GetAttributes();

    if (!attributes.empty()) {
        ss << "layout(";
        for (size_t i = 0; i < attributes.size(); ++i) {
            const BgiShaderSectionAttribute &a = attributes[i];
            if (i > 0) {
                ss << ", ";
            }
            ss << a.identifier;
            if(!a.index.empty()) {
                ss << " = " << a.index;
            }
        }
        ss << ") ";
    }

    ss << _qualifier << " ";
    WriteIdentifier(ss);
    ss << " {\n";
    for (const BgiVulkanShaderSection* member : _members) {
        ss << "  ";
        member->WriteType(ss);
        ss << " ";
        member->WriteIdentifier(ss);
        ss << ";\n";
    }
    ss << "} ";
    WriteBlockInstanceIdentifier(ss);
    WriteArraySize(ss);
    ss << ";\n";
    return true;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
