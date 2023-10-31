#ifndef GUNGNIR_DRIVER_BASE_SHADER_GENERATOR_H
#define GUNGNIR_DRIVER_BASE_SHADER_GENERATOR_H

#include "core/base.h"

#include "driver/bgiBase/api.h"
#include "driver/bgiBase/enums.h"

#include <iosfwd>
#include <string>

GUNGNIR_NAMESPACE_OPEN_SCOPE

struct BgiShaderFunctionDesc;

/// \class HgiShaderGenerator
///
/// Base class for shader function generation
/// Given a descriptor, converts glslfx domain language to concrete shader
/// languages. Can be extended with new types of code sections and specialized
/// for different APIs. It's main role is to make GLSLFX a write once language,
/// no matter the API
///
class BgiShaderGenerator
{
public:
    BGI_API
    virtual ~BgiShaderGenerator();

    // Execute shader generation.
    BGI_API
    void Execute();

    // Return generated shader source.
    BGI_API
    const char *GetGeneratedShaderCode() const;

protected:
    BGI_API
    explicit BgiShaderGenerator(const BgiShaderFunctionDesc &descriptor);

    BGI_API
    virtual void _Execute(std::ostream &ss) = 0;

    BGI_API
    const char *_GetShaderCodeDeclarations() const;

    BGI_API
    const char *_GetShaderCode() const;

    BGI_API
    BgiShaderStage _GetShaderStage() const;

private:
    const BgiShaderFunctionDesc &_descriptor;

    // This is used if the descriptor does not specify a string
    // to be used as the destination for generated output.
    std::string _localGeneratedShaderCode;

    BgiShaderGenerator() = delete;
    BgiShaderGenerator & operator=(const BgiShaderGenerator&) = delete;
    BgiShaderGenerator(const BgiShaderGenerator&) = delete;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_BASE_SHADER_GENERATOR_H