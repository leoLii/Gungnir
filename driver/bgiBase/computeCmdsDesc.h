#ifndef GUNGNIR_DRIVER_BASE_COMPUTE_CMDS_DESC_H
#define GUNGNIR_DRIVER_BASE_COMPUTE_CMDS_DESC_H

#include "core/base.h"

#include "driver/bgiBase/api.h"
#include "driver/bgiBase/enums.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

/// \struct HgiComputeCmdsDesc
///
/// Describes the properties to construct a HgiComputeCmds.
///
/// <ul>
/// <li>dispatchMethod:
///   The dispatch method for compute encoders.</li>
/// </ul>
///
struct BgiComputeCmdsDesc
{
    BGI_API
    BgiComputeCmdsDesc();

    BgiComputeDispatch dispatchMethod;
};

BGI_API
bool operator==(
    const BgiComputeCmdsDesc& lhs,
    const BgiComputeCmdsDesc& rhs);

BGI_API
bool operator!=(
    const BgiComputeCmdsDesc& lhs,
    const BgiComputeCmdsDesc& rhs);

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_BASE_COMPUTE_CMDS_DESC_H
