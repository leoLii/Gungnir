#ifndef GUNGNIR_DRIVER_BASE_GRAPHICS_CMDS_DESC_H
#define GUNGNIR_DRIVER_BASE_GRAPHICS_CMDS_DESC_H

#include "base.h"
#include "texture.h"
#include "attachmentDesc.h"

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

bool operator==(
    const BgiGraphicsCmdsDesc& lhs,
    const BgiGraphicsCmdsDesc& rhs);

bool operator!=(
    const BgiGraphicsCmdsDesc& lhs,
    const BgiGraphicsCmdsDesc& rhs);

std::ostream& operator<<(
    std::ostream& out,
    const BgiGraphicsCmdsDesc& desc);

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif
