#pragma once

#include "core/base.h"

#include "driver/bgiBase/api.h"
#include "driver/bgiBase/texture.h"
#include "driver/bgiBase/attachmentDesc.h"

#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

/// \struct HgiGraphicsCmdsDesc
///
/// Describes the properties to begin a HgiGraphicsCmds.
///
/// <ul>
/// <li>colorAttachmentDescs:
///   Describes each of the color attachments.</li>
/// <li>depthAttachmentDesc:
///   Describes the depth attachment (optional)</li>
/// <li>colorTextures:
///   The color attachment render targets.</li>
/// <li>colorResolveTextures:
///   The (optional) textures that the color textures will be resolved into
///   at the end of the render pass.</li>
/// <li>depthTexture:
///   The depth attachment render target (optional)</li>
/// <li>depthResolveTexture:
///   The (optional) texture that the depth texture will be resolved into
///   at the end of the render pass.</li>
/// <li>width:
///   Render target width (in pixels)</li>
/// <li>height:
///   Render target height (in pixels)</li>
/// </ul>
///
struct BgiGraphicsCmdsDesc
{
    BgiGraphicsCmdsDesc()
    : colorAttachmentDescs()
    , depthAttachmentDesc()
    , colorTextures()
    , colorResolveTextures()
    , depthTexture()
    , depthResolveTexture()
    {}

    inline bool HasAttachments() const {
        return !colorAttachmentDescs.empty() || depthTexture;
    }

    BgiAttachmentDescVector colorAttachmentDescs;
    BgiAttachmentDesc depthAttachmentDesc;

    BgiTextureHandleVector colorTextures;
    BgiTextureHandleVector colorResolveTextures;

    BgiTextureHandle depthTexture;
    BgiTextureHandle depthResolveTexture;
};

BGI_API
bool operator==(
    const BgiGraphicsCmdsDesc& lhs,
    const BgiGraphicsCmdsDesc& rhs);

BGI_API
bool operator!=(
    const BgiGraphicsCmdsDesc& lhs,
    const BgiGraphicsCmdsDesc& rhs);

BGI_API
std::ostream& operator<<(
    std::ostream& out,
    const BgiGraphicsCmdsDesc& desc);

GUNGNIR_NAMESPACE_CLOSE_SCOPE
