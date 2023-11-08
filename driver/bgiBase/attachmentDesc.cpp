#include "driver/bgiBase/attachmentDesc.h"

#include <ostream>

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

bool operator==(
    const BgiAttachmentDesc& lhs,
    const BgiAttachmentDesc& rhs) 
{
    return  lhs.format == rhs.format &&
            lhs.usage == rhs.usage &&
            lhs.clearValue == rhs.clearValue &&
            lhs.colorMask == rhs.colorMask &&
            lhs.loadOp == rhs.loadOp &&
            lhs.storeOp == rhs.storeOp &&
            lhs.blendEnabled == rhs.blendEnabled &&
            lhs.srcColorBlendFactor == rhs.srcColorBlendFactor &&
            lhs.dstColorBlendFactor == rhs.dstColorBlendFactor &&
            lhs.colorBlendOp == rhs.colorBlendOp &&
            lhs.srcAlphaBlendFactor == rhs.srcAlphaBlendFactor &&
            lhs.dstAlphaBlendFactor == rhs.dstAlphaBlendFactor &&
            lhs.alphaBlendOp == rhs.alphaBlendOp &&
            lhs.blendConstantColor == rhs.blendConstantColor;
}

bool operator!=(
    const BgiAttachmentDesc& lhs,
    const BgiAttachmentDesc& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(
    std::ostream& out,
    const BgiAttachmentDesc& attachment)
{
    out << "BgiAttachmentDesc: {"
        << "format: " << attachment.format << ", "
        << "usage: " << attachment.usage << ", "
        << "clearValue: " << attachment.clearValue << ", "
        << "colorMask: " << attachment.colorMask << ", "
        << "loadOp: " << attachment.loadOp << ", "
        << "storeOp: " << attachment.storeOp << ", "
        << "blendEnabled: " << attachment.blendEnabled << ", "
        << "srcColorBlendFactor: " << attachment.srcColorBlendFactor << ", "
        << "dstColorBlendFactor: " << attachment.dstColorBlendFactor << ", "
        << "colorBlendOp: " << attachment.colorBlendOp << ", "
        << "srcAlphaBlendFactor: " << attachment.srcAlphaBlendFactor << ", "
        << "dstAlphaBlendFactor: " << attachment.dstAlphaBlendFactor << ", "
        << "alphaBlendOp: " << attachment.alphaBlendOp << ", "
        << "blendConstantColor: " << attachment.blendConstantColor <<
    "}";
    return out;
}

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
