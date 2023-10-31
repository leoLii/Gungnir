#include "driver/bgiBase/shaderGenerator.h"

#include "pxr/base/tf/stringUtils.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiShaderGenerator::BgiShaderGenerator(const BgiShaderFunctionDesc &descriptor)
    : _descriptor(descriptor)
{
}

BgiShaderGenerator::~BgiShaderGenerator() = default;

void
BgiShaderGenerator::Execute()
{
    std::stringstream ss;

    // Use the protected version which can be overridden
    _Execute(ss);

    // Capture the result as specified by the descriptor or locally.
    if (_descriptor.generatedShaderCodeOut) {
       *_descriptor.generatedShaderCodeOut = ss.str();
    } else {
       _localGeneratedShaderCode = ss.str();
    }
}

const char *
BgiShaderGenerator::_GetShaderCodeDeclarations() const
{
    static const char *emptyString = "";
    return _descriptor.shaderCodeDeclarations
                ? _descriptor.shaderCodeDeclarations : emptyString;
}

const char *
BgiShaderGenerator::_GetShaderCode() const
{
    static const char *emptyString = "";
    return _descriptor.shaderCode
                ? _descriptor.shaderCode : emptyString;
}

BgiShaderStage
BgiShaderGenerator::_GetShaderStage() const
{
    return _descriptor.shaderStage;
}

const char *
BgiShaderGenerator::GetGeneratedShaderCode() const
{
    // Return the result as specified by the descriptor or locally.
    if (_descriptor.generatedShaderCodeOut) {
       return _descriptor.generatedShaderCodeOut->c_str();
    } else {
       return _localGeneratedShaderCode.c_str();
    }
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
