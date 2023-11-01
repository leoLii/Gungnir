#include "driver/bgiBase/shaderSection.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiShaderSection::BgiShaderSection(
    const std::string &identifier,
    const BgiShaderSectionAttributeVector& attributes,
    const std::string &defaultValue,
    const std::string &arraySize,
    const std::string &blockInstanceIdentifier)
  : _identifierVar(identifier)
  , _attributes(attributes)
  , _defaultValue(defaultValue)
  , _arraySize(arraySize)
  , _blockInstanceIdentifier(blockInstanceIdentifier)
{
}

BgiShaderSection::~BgiShaderSection() = default;

void
BgiShaderSection::WriteType(std::ostream& ss) const
{
}

void
BgiShaderSection::WriteIdentifier(std::ostream& ss) const
{
    ss << _identifierVar;
}

void
BgiShaderSection::WriteBlockInstanceIdentifier(std::ostream& ss) const
{
    ss << _blockInstanceIdentifier;
}

void
BgiShaderSection::WriteDeclaration(std::ostream& ss) const
{
    WriteType(ss);
    ss << " ";
    WriteIdentifier(ss);
    WriteArraySize(ss);
    ss << ";";
}

void
BgiShaderSection::WriteParameter(std::ostream& ss) const
{
    WriteType(ss);
    ss << " ";
    WriteIdentifier(ss);
}

void
BgiShaderSection::WriteArraySize(std::ostream& ss) const
{
    if (!_arraySize.empty()) {
        ss << "[" << _arraySize << "]";
    }
}

const BgiShaderSectionAttributeVector&
BgiShaderSection::GetAttributes() const
{
    return _attributes;
}

const std::string&
BgiShaderSection::_GetDefaultValue() const
{
    return _defaultValue;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
