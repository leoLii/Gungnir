#ifndef GUNGNIR_DRIVER_BASE_TEXTURE_H
#define GUNGNIR_DRIVER_BASE_TEXTURE_H

#include "base.h"
#include "enums.h"
#include "handle.h"
#include "types.h"

#include <string>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

/// \struct BgiComponentMapping
/// 
struct BgiComponentMapping
{
    BgiComponentSwizzle r;
    BgiComponentSwizzle g;
    BgiComponentSwizzle b;
    BgiComponentSwizzle a;
};

/// \struct BgiTextureDesc
/// 
struct BgiTextureDesc
{
    BgiTextureDesc()
    : usage(0)
    , format(BgiFormatInvalid)
    , componentMapping{
        BgiComponentSwizzleR,
        BgiComponentSwizzleG,
        BgiComponentSwizzleB,
        BgiComponentSwizzleA}
    , type(BgiTextureType2D)
    , dimensions(0)
    , layerCount(1)
    , mipLevels(1)
    , sampleCount(BgiSampleCount1)
    , pixelsByteSize(0)
    , initialData(nullptr)
    {}

    std::string debugName;
    BgiTextureUsage usage;
    BgiFormat format;
    BgiComponentMapping componentMapping;
    BgiTextureType type;
    GfVec3i dimensions;
    uint16_t layerCount;
    uint16_t mipLevels;
    BgiSampleCount sampleCount;
    size_t pixelsByteSize;
    void const* initialData;
};


/// \class BgiTexture
/// 
class BgiTexture
{
public:
    virtual ~BgiTexture();

    BgiTextureDesc const& GetDescriptor() const;

    virtual size_t GetByteSizeOfResource() const = 0;

    virtual uint64_t GetRawResource() const = 0;

protected:
    static size_t _GetByteSizeOfResource(const BgiTextureDesc &descriptor);

    BgiTexture(BgiTextureDesc const& desc);

    BgiTextureDesc _descriptor;

private:
    BgiTexture() = delete;
    BgiTexture & operator=(const BgiTexture&) = delete;
    BgiTexture(const BgiTexture&) = delete;
};

using BgiTextureHandle = BgiHandle<class BgiTexture>;
using BgiTextureHandleVector = std::vector<BgiTextureHandle>;

/// \struct BgiTextureViewDesc
///
struct BgiTextureViewDesc
{
    BgiTextureViewDesc()
    : format(BgiFormatInvalid)
    , layerCount(1)
    , mipLevels(1)
    , sourceTexture()
    , sourceFirstLayer(0)
    , sourceFirstMip(0)
    {}

    std::string debugName;
    BgiFormat format;
    uint16_t layerCount;
    uint16_t mipLevels;
    BgiTextureHandle sourceTexture;
    uint16_t sourceFirstLayer;
    uint16_t sourceFirstMip;
};

bool operator==(
    const BgiTextureViewDesc& lhs,
    const BgiTextureViewDesc& rhs);

bool operator!=(
    const BgiTextureViewDesc& lhs,
    const BgiTextureViewDesc& rhs);

/// \class BgiTextureView
///
class BgiTextureView
{
public:
    BgiTextureView(BgiTextureViewDesc const& desc);

    virtual ~BgiTextureView();

    void SetViewTexture(BgiTextureHandle const& handle);

    BgiTextureHandle const& GetViewTexture() const;

protected:
    BgiTextureHandle _viewTexture;

private:
    BgiTextureView() = delete;
    BgiTextureView & operator=(const BgiTextureView&) = delete;
    BgiTextureView(const BgiTextureView&) = delete;
};

using BgiTextureViewHandle = BgiHandle<class BgiTextureView>;
using BgiTextureViewHandleVector = std::vector<BgiTextureViewHandle>;

GUNGNIR_NAMESPACE_CLOSE_SCOPE
#endif // GUNGNIR_DRIVER_TEXTURE_H