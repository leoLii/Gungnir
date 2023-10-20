#include "graphicsCmdsDesc.h"

#include <ostream>

GUNGNIR_NAMESPACE_OPEN_SCOPE

bool operator==(
    const BgiGraphicsCmdsDesc& lhs,
    const BgiGraphicsCmdsDesc& rhs) 
{
    return  lhs.depthAttachmentDesc == rhs.depthAttachmentDesc &&
            lhs.colorAttachmentDescs == rhs.colorAttachmentDescs &&
            lhs.depthTexture == rhs.depthTexture &&
            lhs.depthResolveTexture == rhs.depthResolveTexture &&
            lhs.colorTextures == rhs.colorTextures &&
            lhs.colorResolveTextures == rhs.colorResolveTextures;
}

bool operator!=(
    const BgiGraphicsCmdsDesc& lhs,
    const BgiGraphicsCmdsDesc& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(
    std::ostream& out,
    const BgiGraphicsCmdsDesc& desc)
{
    out << "BgiGraphicsCmdsDesc: {";

    for (BgiAttachmentDesc const& a : desc.colorAttachmentDescs) {
        out << a;
    }
    
    for (size_t i=0; i<desc.colorTextures.size(); i++) {
        out << "colorTexture" << i << " ";
        out << "dimensions:" << 
            desc.colorTextures[i]->GetDescriptor().dimensions << ", ";
    }

    for (size_t i=0; i<desc.colorResolveTextures.size(); i++) {
        out << "colorResolveTexture" << i << ", ";
    }

    if (desc.depthTexture) {
        out << desc.depthAttachmentDesc;
        out << "depthTexture ";
        out << "dimensions:" << desc.depthTexture->GetDescriptor().dimensions;
    }

    if (desc.depthResolveTexture) {
        out << "depthResolveTexture";
    }

    out << "}";
    return out;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
