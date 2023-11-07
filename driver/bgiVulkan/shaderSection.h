#pragma once

#include "core/base.h"

#include "driver/bgiBase/shaderSection.h"
#include "driver/bgiBase/shaderFunctionDesc.h"
#include "driver/bgiVulkan/api.h"
#include "driver/bgiVulkan/vulkanBridge.h"

#include <string>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

/// \class HgiVulkanShaderSection
///
/// Base class for Vulkan code sections. The generator holds these
///
class BgiVulkanShaderSection : public BgiShaderSection
{
public:
    BGIVULKAN_API
    explicit BgiVulkanShaderSection(
        const std::string &identifier,
        const BgiShaderSectionAttributeVector &attributes = {},
        const std::string &storageQualifier = std::string(),
        const std::string &defaultValue = std::string(),
        const std::string &arraySize = std::string(),
        const std::string &blockInstanceIdentifier = std::string());

    BGIVULKAN_API
    ~BgiVulkanShaderSection() override;

    BGIVULKAN_API
    void WriteDeclaration(std::ostream &ss) const override;
    BGIVULKAN_API
    void WriteParameter(std::ostream &ss) const override;

    BGIVULKAN_API
    virtual bool VisitGlobalIncludes(std::ostream &ss);
    BGIVULKAN_API
    virtual bool VisitGlobalMacros(std::ostream &ss);
    BGIVULKAN_API
    virtual bool VisitGlobalStructs(std::ostream &ss);
    BGIVULKAN_API
    virtual bool VisitGlobalMemberDeclarations(std::ostream &ss);
    BGIVULKAN_API
    virtual bool VisitGlobalFunctionDefinitions(std::ostream &ss);

protected:
    const std::string _storageQualifier;
    const std::string _arraySize;

private:
    BgiVulkanShaderSection() = delete;
    BgiVulkanShaderSection & operator=(const BgiVulkanShaderSection&) = delete;
    BgiVulkanShaderSection(const BgiVulkanShaderSection&) = delete;
};

using BgiVulkanShaderSectionPtrVector = 
    std::vector<BgiVulkanShaderSection*>;

/// \class HgiVulkanMacroShaderSection
///
/// A ShaderSection for defining macros.
/// Accepts raw strings and dumps it to the global scope under includes
///
class BgiVulkanMacroShaderSection final: public BgiVulkanShaderSection
{
public:
    BGIVULKAN_API
    explicit BgiVulkanMacroShaderSection(
        const std::string &macroDeclaration,
        const std::string &macroComment);

    BGIVULKAN_API
    ~BgiVulkanMacroShaderSection() override;

    BGIVULKAN_API
    bool VisitGlobalMacros(std::ostream &ss) override;

private:
    BgiVulkanMacroShaderSection() = delete;
    BgiVulkanMacroShaderSection & operator=(
        const BgiVulkanMacroShaderSection&) = delete;
    BgiVulkanMacroShaderSection(const BgiVulkanMacroShaderSection&) = delete;

    const std::string _macroComment;
};

/// \class HgiVulkanMemberShaderSection
///
/// Declares a member in global scope, for declaring instances of structs, constant
/// params etc - it's quite flexible in it's writing capabilities
///
class BgiVulkanMemberShaderSection final: public BgiVulkanShaderSection
{
public:
    BGIVULKAN_API
    explicit BgiVulkanMemberShaderSection(
        const std::string &identifier,
        const std::string &typeName,
        const BgiInterpolationType interpolation,
        const BgiSamplingType sampling,
        const BgiStorageType storage,
        const BgiShaderSectionAttributeVector &attributes,
        const std::string &storageQualifier,
        const std::string &defaultValue = std::string(),
        const std::string &arraySize = std::string(),
        const std::string &blockInstanceIdentifier = std::string());

    BGIVULKAN_API
    ~BgiVulkanMemberShaderSection() override;

    BGIVULKAN_API
    bool VisitGlobalMemberDeclarations(std::ostream &ss) override;

    BGIVULKAN_API
    void WriteType(std::ostream& ss) const override;

private:
    BgiVulkanMemberShaderSection() = delete;
    BgiVulkanMemberShaderSection & operator=(
        const BgiVulkanMemberShaderSection&) = delete;
    BgiVulkanMemberShaderSection(const BgiVulkanMemberShaderSection&) = delete;

    std::string _typeName;
    BgiInterpolationType _interpolation;
    BgiSamplingType _sampling;
    BgiStorageType _storage;
};

/// \class HgiVulkanBlockShaderSection
///
/// For writing out uniform blocks, defines them in the global member declerations.
///
class BgiVulkanBlockShaderSection final: public BgiVulkanShaderSection
{
public:
    BGIVULKAN_API
    explicit BgiVulkanBlockShaderSection(
            const std::string &identifier,
            const BgiShaderFunctionParamDescVector &parameters);

    BGIVULKAN_API
    ~BgiVulkanBlockShaderSection() override;

    BGIVULKAN_API
    bool VisitGlobalMemberDeclarations(std::ostream &ss) override;

private:
    const BgiShaderFunctionParamDescVector _parameters;
};

/// \class HgiVulkanMemberShaderSection
///
/// Declares OpenGL textures, and their cross language function
///
class BgiVulkanTextureShaderSection final: public BgiVulkanShaderSection
{
public:
    BGIVULKAN_API
    explicit BgiVulkanTextureShaderSection(
        const std::string &identifier,
        const uint32_t layoutIndex,
        const uint32_t dimensions,
        const BgiFormat format,
        const BgiShaderTextureType textureType,
        const uint32_t arraySize,
        const bool writable,
        const BgiShaderSectionAttributeVector &attributes,
        const std::string &defaultValue = std::string());

    BGIVULKAN_API
    ~BgiVulkanTextureShaderSection() override;

    BGIVULKAN_API
    void WriteType(std::ostream &ss) const override;

    BGIVULKAN_API
    bool VisitGlobalMemberDeclarations(std::ostream &ss) override;
    BGIVULKAN_API
    bool VisitGlobalFunctionDefinitions(std::ostream &ss) override;

private:
    BgiVulkanTextureShaderSection() = delete;
    BgiVulkanTextureShaderSection & operator=(
        const BgiVulkanTextureShaderSection&) = delete;
    BgiVulkanTextureShaderSection(const BgiVulkanTextureShaderSection&) = delete;

    void _WriteSamplerType(std::ostream &ss) const;
    void _WriteSampledDataType(std::ostream &ss) const;

    const uint32_t _dimensions;
    const BgiFormat _format;
    const BgiShaderTextureType _textureType;
    const uint32_t _arraySize;
    const bool _writable;
    static const std::string _storageQualifier;
};

/// \class HgiVulkanBufferShaderSection
///
/// Declares Vulkan buffers, and their cross language function
///
class BgiVulkanBufferShaderSection final: public BgiVulkanShaderSection
{
public:
    BGIVULKAN_API
    explicit BgiVulkanBufferShaderSection(
        const std::string &identifier,
        const uint32_t layoutIndex,
        const std::string &type,
        const BgiBindingType binding,
        const std::string arraySize,
        const bool writable,
        const BgiShaderSectionAttributeVector &attributes);

    BGIVULKAN_API
    ~BgiVulkanBufferShaderSection() override;

    BGIVULKAN_API
    void WriteType(std::ostream &ss) const override;

    BGIVULKAN_API
    bool VisitGlobalMemberDeclarations(std::ostream &ss) override;

private:
    BgiVulkanBufferShaderSection() = delete;
    BgiVulkanBufferShaderSection & operator=(
        const BgiVulkanBufferShaderSection&) = delete;
    BgiVulkanBufferShaderSection(const BgiVulkanBufferShaderSection&) = delete;

    const std::string _type;
    const BgiBindingType _binding;
    const std::string _arraySize;
    const bool _writable;
};

/// \class HgiVulkanKeywordShaderSection
///
/// Declares reserved Vulkan shader inputs, and their cross language function
///
class BgiVulkanKeywordShaderSection final: public BgiVulkanShaderSection
{
public:
    BGIVULKAN_API
    explicit BgiVulkanKeywordShaderSection(
        const std::string &identifier,
        const std::string &type,
        const std::string &keyword);

    BGIVULKAN_API
    ~BgiVulkanKeywordShaderSection() override;

    BGIVULKAN_API
    void WriteType(std::ostream &ss) const override;

    BGIVULKAN_API
    bool VisitGlobalMemberDeclarations(std::ostream &ss) override;

private:
    BgiVulkanKeywordShaderSection() = delete;
    BgiVulkanKeywordShaderSection & operator=(
        const BgiVulkanKeywordShaderSection&) = delete;
    BgiVulkanKeywordShaderSection(const BgiVulkanKeywordShaderSection&) = delete;

    const std::string _type;
    const std::string _keyword;
};

/// \class HgiVulkanInterstageBlockShaderSection
///
/// Defines and writes out an interstage interface block
///
class BgiVulkanInterstageBlockShaderSection final: public BgiVulkanShaderSection
{
public:
    BGIVULKAN_API
    explicit BgiVulkanInterstageBlockShaderSection(
        const std::string &blockIdentifier,
        const std::string &blockInstanceIdentifier,
        const BgiShaderSectionAttributeVector &attributes,
        const std::string &qualifier,
        const std::string &arraySize,
        const BgiVulkanShaderSectionPtrVector &members);

    BGIVULKAN_API
    bool VisitGlobalMemberDeclarations(std::ostream &ss) override;

private:
    BgiVulkanInterstageBlockShaderSection() = delete;
    BgiVulkanInterstageBlockShaderSection & operator=(
        const BgiVulkanInterstageBlockShaderSection&) = delete;
    BgiVulkanInterstageBlockShaderSection(
        const BgiVulkanInterstageBlockShaderSection&) = delete;

    const std::string _qualifier;
    const BgiVulkanShaderSectionPtrVector _members;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE
