#include "shaderProgram.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiShaderProgram::BgiShaderProgram(BgiShaderProgramDesc const& desc)
    : _descriptor(desc)
{
}

BgiShaderProgram::~BgiShaderProgram() = default;

BgiShaderProgramDesc const&
BgiShaderProgram::GetDescriptor() const
{
    return _descriptor;
}

BgiShaderProgramDesc::BgiShaderProgramDesc()
{
}

bool operator==(
    const BgiShaderProgramDesc& lhs,
    const BgiShaderProgramDesc& rhs)
{
    return lhs.debugName == rhs.debugName;
}

bool operator!=(
    const BgiShaderProgramDesc& lhs,
    const BgiShaderProgramDesc& rhs)
{
    return !(lhs == rhs);
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
