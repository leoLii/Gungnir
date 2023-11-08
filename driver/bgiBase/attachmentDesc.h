#pragma once

#include "core/base.h"
#include "core/math/math.h"

#include "driver/bgiBase/api.h"
#include "driver/bgiBase/enums.h"
#include "driver/bgiBase/types.h"

#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

using namespace math;

namespace driver {

/// \struct HgiAttachmentDesc
///
/// Describes the properties of a framebuffer attachment.
///
/// <ul>
/// <li>format:
///   The format of the attachment.
///   Must match what is set in HgiTextureDesc.</li>
/// <li>usage:
///   Describes how the texture is intended to be used.
///   Must match what is set in HgiTextureDesc.</li>
/// <li>loadOp:
///   The operation to perform on the attachment pixel data prior to rendering.</li>
/// <li>storeOp:
///   The operation to perform on the attachment pixel data after rendering.</li>
/// <li>clearValue:
///   The value to clear the attachment with (r,g,b,a) or (depth,stencil,x,x)</li>
/// <li>colorMask:
///   Whether to permit or restrict writing to component channels.</li>
/// <li>blendEnabled:
///   Determines if a blend operation should be applied to the attachment.</li>
/// <li> ***BlendFactor:
///   The blend factors for source and destination.</li>
/// <li> ***BlendOp: 
///   The blending operation.</li>
/// <li> blendConstantColor:
///   The constant color for blend operations.</li>
///
struct BgiAttachmentDesc
{
    BgiAttachmentDesc() 
    : format(BgiFormatInvalid)
    , usage(0)
    , loadOp(BgiAttachmentLoadOpLoad)
    , storeOp(BgiAttachmentStoreOpStore)
    , clearValue(0)
    , colorMask(BgiColorMaskRed | BgiColorMaskGreen |
                BgiColorMaskBlue | BgiColorMaskAlpha)
    , blendEnabled(false)
    , srcColorBlendFactor(BgiBlendFactorZero)
    , dstColorBlendFactor(BgiBlendFactorZero)
    , colorBlendOp(BgiBlendOpAdd)
    , srcAlphaBlendFactor(BgiBlendFactorZero)
    , dstAlphaBlendFactor(BgiBlendFactorZero)
    , alphaBlendOp(BgiBlendOpAdd)
    , blendConstantColor(0.0f, 0.0f, 0.0f, 0.0f)
    {}

    BgiFormat format;
    BgiTextureUsage usage;
    BgiAttachmentLoadOp loadOp;
    BgiAttachmentStoreOp storeOp;
    Vector4f clearValue;
    BgiColorMask colorMask;
    bool blendEnabled;
    BgiBlendFactor srcColorBlendFactor;
    BgiBlendFactor dstColorBlendFactor;
    BgiBlendOp colorBlendOp;
    BgiBlendFactor srcAlphaBlendFactor;
    BgiBlendFactor dstAlphaBlendFactor;
    BgiBlendOp alphaBlendOp;
    Vector4f blendConstantColor;
};

using BgiAttachmentDescVector = std::vector<BgiAttachmentDesc>;

BGI_API
bool operator==(
    const BgiAttachmentDesc& lhs,
    const BgiAttachmentDesc& rhs);

BGI_API
bool operator!=(
    const BgiAttachmentDesc& lhs,
    const BgiAttachmentDesc& rhs);

BGI_API
std::ostream& operator<<(
    std::ostream& out,
    const BgiAttachmentDesc& attachment);

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
