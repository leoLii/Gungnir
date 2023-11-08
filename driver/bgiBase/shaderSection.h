#pragma once

#include "core/base.h"

#include "driver/bgiBase/api.h"

#include <memory>
#include <ostream>
#include <string>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

//struct to hold attribute definitions
struct BgiShaderSectionAttribute
{
    std::string identifier;
    std::string index;
};

using BgiShaderSectionAttributeVector =
    std::vector<BgiShaderSectionAttribute>;

/// \class HgiShaderSection
///
/// A base class for a Shader Section.
/// In its simplest form then it is a construct that knows
/// how to declare itself, define and pass as param.
/// Can be subclassed to add more behaviour for complex cases
/// and to hook into the visitor tree.
///
class BgiShaderSection
{
public:
    BGI_API
    virtual ~BgiShaderSection();

    /// Write out the type, shader section does not hold a type
    /// string as how a type is defined is fully controlled
    /// by sub classes and no assumptions are made
    BGI_API
    virtual void WriteType(std::ostream& ss) const;

    /// Writes the unique name of an instance of the section
    BGI_API
    virtual void WriteIdentifier(std::ostream& ss) const;

    /// Writes a decleration statement for a member or in global scope
    BGI_API
    virtual void WriteDeclaration(std::ostream& ss) const;

    /// Writes the section as a parameter to a function
    BGI_API
    virtual void WriteParameter(std::ostream& ss) const;

    /// Writes the arraySize to a function
    BGI_API
    virtual void WriteArraySize(std::ostream& ss) const;

    /// Writes the block instance name of an instance of the section
    BGI_API
    virtual void WriteBlockInstanceIdentifier(std::ostream& ss) const;

    /// Returns the attributes of the section
    BGI_API
    const BgiShaderSectionAttributeVector& GetAttributes() const;
    
    /// Returns the arraySize of the section
    const std::string& GetArraySize() const {
        return _arraySize;
    }

    /// Returns whether the section has a block instance identifier
    bool HasBlockInstanceIdentifier() const {
        return !_blockInstanceIdentifier.empty();
    }

protected:
    BGI_API
    explicit BgiShaderSection(
            const std::string &identifier,
            const BgiShaderSectionAttributeVector& attributes = {},
            const std::string &defaultValue = std::string(),
            const std::string &arraySize = std::string(),
            const std::string &blockInstanceIdentifier = std::string());

    BGI_API
    const std::string& _GetDefaultValue() const;

private:
    const std::string _identifierVar;
    const BgiShaderSectionAttributeVector _attributes;
    const std::string _defaultValue;
    const std::string _arraySize;
    const std::string _blockInstanceIdentifier;
};

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
