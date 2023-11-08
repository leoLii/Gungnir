#include "driver/bgiBase/shaderFunction.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

BgiShaderFunctionDesc const&
BgiShaderFunction::GetDescriptor() const
{
    return _descriptor;
}

BgiShaderFunction::BgiShaderFunction(BgiShaderFunctionDesc const& desc)
    : _descriptor(desc)
{
}

BgiShaderFunction::~BgiShaderFunction() = default;

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
