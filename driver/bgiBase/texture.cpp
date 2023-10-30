#include "driver/bgiBase/texture.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiTexture::BgiTexture(BgiTextureDesc const& desc)
    : _descriptor(desc)
{
}

BgiTexture::~BgiTexture() = default;

BgiTextureDesc const&
BgiTexture::GetDescriptor() const
{
    return _descriptor;
}

size_t
BgiTexture::_GetByteSizeOfResource(const BgiTextureDesc &descriptor)
{
    // Compute all mip levels down to 1x1(x1)
    const std::vector<BgiMipInfo> mipInfos = BgiGetMipInfos(
        descriptor.format, descriptor.dimensions, descriptor.layerCount);

    // Number of mip levels actually used.
    const size_t mipLevels = std::min(
        mipInfos.size(), size_t(descriptor.mipLevels));

    // Get the last mip level actually used.
    const BgiMipInfo &mipInfo = mipInfos[mipLevels - 1];

    // mipInfo.byteOffset is the sum of all mip levels prior
    // to the last mip level actually used.
    return
        mipInfo.byteOffset + descriptor.layerCount * mipInfo.byteSizePerLayer;

}

bool operator==(const BgiComponentMapping& lhs,
    const BgiComponentMapping& rhs)
{
    return lhs.r == rhs.r &&
           lhs.g == rhs.g &&
           lhs.b == rhs.b &&
           lhs.a == rhs.a;
}

bool operator!=(const BgiComponentMapping& lhs,
    const BgiComponentMapping& rhs)
{
    return !(lhs == rhs);
}

bool operator==(const BgiTextureDesc& lhs,
    const BgiTextureDesc& rhs)
{
    return  lhs.debugName == rhs.debugName &&
            lhs.usage == rhs.usage &&
            lhs.format == rhs.format &&
            lhs.componentMapping == rhs.componentMapping &&
            lhs.type == rhs.type &&
            lhs.dimensions == rhs.dimensions &&
            lhs.sampleCount == rhs.sampleCount &&
            lhs.pixelsByteSize == rhs.pixelsByteSize
            // Omitted because data ptr is set to nullptr after CreateTexture
            // lhs.initialData == rhs.initialData
    ;
}

bool operator!=(const BgiTextureDesc& lhs,
    const BgiTextureDesc& rhs)
{
    return !(lhs == rhs);
}

bool operator==(const BgiTextureViewDesc& lhs,
    const BgiTextureViewDesc& rhs)
{
    return  lhs.debugName == rhs.debugName &&
            lhs.format == rhs.format &&
            lhs.layerCount == rhs.layerCount &&
            lhs.mipLevels == rhs.mipLevels &&
            lhs.sourceTexture == rhs.sourceTexture &&
            lhs.sourceFirstLayer == rhs.sourceFirstLayer &&
            lhs.sourceFirstMip == rhs.sourceFirstMip
    ;
}

bool operator!=(const BgiTextureViewDesc& lhs,
    const BgiTextureViewDesc& rhs)
{
    return !(lhs == rhs);
}

BgiTextureView::BgiTextureView(BgiTextureViewDesc const& desc)
{
}

BgiTextureView::~BgiTextureView() = default;

void
BgiTextureView::SetViewTexture(BgiTextureHandle const& handle)
{
    _viewTexture = handle;
}

BgiTextureHandle const&
BgiTextureView::GetViewTexture() const
{
    return _viewTexture;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
