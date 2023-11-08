#pragma once

#include "core/base.h"
#include "core/math/math.h"

#include "driver/bgiBase/api.h"
#include "driver/bgiBase/enums.h"
#include "driver/bgiBase/handle.h"
#include "driver/bgiBase/types.h"

#include <string>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

using namespace math;

namespace driver {

/// \struct BgiComponentMapping
/// 
struct BgiComponentMapping
{
    BgiComponentSwizzle r;
    BgiComponentSwizzle g;
    BgiComponentSwizzle b;
    BgiComponentSwizzle a;
};

BGI_API
bool operator==(
    const BgiComponentMapping& lhs,
    const BgiComponentMapping& rhs);

BGI_API
bool operator!=(
    const BgiComponentMapping& lhs,
    const BgiComponentMapping& rhs);

/// \struct HgiTextureDesc
///
/// Describes the properties needed to create a GPU texture.
///
/// <ul>
/// <li>debugName:
///   This label can be applied as debug label for GPU debugging.</li>
/// <li>usage:
///   Describes how the texture is intended to be used.</li>
/// <li>format:
///   The format of the texture.
/// <li>componentMapping:
///   The mapping of rgba components when accessing the texture.</li>
/// <li>dimensions:
///   The resolution of the texture (width, height, depth).</li>
/// <li>type:
///   Type of texture (2D, 3D).</li>
/// <li>layerCount:
///   The number of layers (texture-arrays).</li>
/// <li>mipLevels:
///   The number of mips in texture.</li>
/// <li>sampleCount:
///   samples per texel (multi-sampling).</li>
/// <li>pixelsByteSize:
///   Byte size (length) of pixel data (i.e., initialData).</li>
/// <li>initialData:
///   CPU pointer to initialization pixels of the texture.
///   The memory is consumed immediately during the creation of the HgiTexture.
///   The application may alter or free this memory as soon as the constructor
///   of the HgiTexture has returned.
///   Data may optionally include pixels for each mip-level.
///   HgiGetMipInitialData can be used to get to each mip's data and describes
///   in more detail how mip dimensions are rounded.</li>
/// </ul>
///
struct BgiTextureDesc
{
    BGI_API
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
    Vector3i dimensions;
    uint16_t layerCount;
    uint16_t mipLevels;
    BgiSampleCount sampleCount;
    size_t pixelsByteSize;
    void const* initialData;
};

BGI_API
bool operator==(
    const BgiTextureDesc& lhs,
    const BgiTextureDesc& rhs);

BGI_API
bool operator!=(
    const BgiTextureDesc& lhs,
    const BgiTextureDesc& rhs);

///
/// \class HgiTexture
///
/// Represents a graphics platform independent GPU texture resource.
/// Textures should be created via Hgi::CreateTexture.
///
/// Base class for Hgi textures.
/// To the client (HdSt) texture resources are referred to via
/// opaque, stateless handles (HgTextureHandle).
///
class BgiTexture
{
public:
    BGI_API
    virtual ~BgiTexture();

    /// The descriptor describes the object.
    BGI_API
    BgiTextureDesc const& GetDescriptor() const;

    /// Returns the byte size of the GPU texture.
    /// This can be helpful if the application wishes to tally up memory usage.
    BGI_API
    virtual size_t GetByteSizeOfResource() const = 0;

    /// This function returns the handle to the Hgi backend's gpu resource, cast
    /// to a uint64_t. Clients should avoid using this function and instead
    /// use Hgi base classes so that client code works with any Hgi platform.
    /// For transitioning code to Hgi, it can however we useful to directly
    /// access a platform's internal resource handles.
    /// There is no safety provided in using this. If you by accident pass a
    /// HgiMetal resource into an OpenGL call, bad things may happen.
    /// In OpenGL this returns the GLuint resource name.
    /// In Metal this returns the id<MTLTexture> as uint64_t.
    /// In Vulkan this returns the VkImage as uint64_t.
    /// In DX12 this returns the ID3D12Resource pointer as uint64_t.
    BGI_API
    virtual uint64_t GetRawResource() const = 0;

protected:
    BGI_API
    static 
    size_t _GetByteSizeOfResource(const BgiTextureDesc &descriptor);

    BGI_API
    BgiTexture(BgiTextureDesc const& desc);

    BgiTextureDesc _descriptor;

private:
    BgiTexture() = delete;
    BgiTexture & operator=(const BgiTexture&) = delete;
    BgiTexture(const BgiTexture&) = delete;
};

using BgiTextureHandle = BgiHandle<class BgiTexture>;
using BgiTextureHandleVector = std::vector<BgiTextureHandle>;

/// \struct HgiTextureViewDesc
///
/// Describes the properties needed to create a GPU texture view from an
/// existing GPU texture object.
///
/// <ul>
/// <li>debugName:
///   This label can be applied as debug label for GPU debugging.</li>
/// <li>format:
///   The format of the texture view. This format must be compatible with
///   the sourceTexture, but does not have to be the identical format.
///   Generally: All 8-, 16-, 32-, 64-, and 128-bit color formats are 
///   compatible with other formats with the same bit length.
///   For example HgiFormatFloat32Vec4 and HgiFormatInt32Vec4 are compatible.
/// <li>layerCount:
///   The number of layers (texture-arrays).</li>
/// <li>mipLevels:
///   The number of mips in texture.</li>
/// <li>sourceTexture:
///   Handle to the HgiTexture to be used as the source data backing.</li>
/// <li>sourceFirstLayer:
///   The layer index to use from the source texture as the first layer of the
///   view.</li>
/// <li>sourceFirstMip:
///   The mip index to ues from the source texture as the first mip of the
///   view.</li>
///   </ul>
///
struct BgiTextureViewDesc
{
    BGI_API
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

BGI_API
bool operator==(
    const BgiTextureViewDesc& lhs,
    const BgiTextureViewDesc& rhs);

BGI_API
bool operator!=(
    const BgiTextureViewDesc& lhs,
    const BgiTextureViewDesc& rhs);

///
/// \class HgiTextureView
///
/// Represents a graphics platform independent GPU texture view resource.
/// Texture Views should be created via Hgi::CreateTextureView.
///
/// A TextureView aliases the data of another texture and is a thin wrapper
/// around a HgiTextureHandle. The embeded texture handle is used to
/// add the texture to resource bindings for use in shaders.
///
/// For example when using a compute shader to fill the mip levels of a
/// texture, like a lightDome texture, we can use a texture view to give the 
/// shader access to a specific mip level of a sourceTexture via a TextureView.
///
/// Another example is to conserve resources by reusing a RGBAF32 texture as
/// a RGBAI32 texture once the F32 texture is no longer needed
/// (transient resources).
///
class BgiTextureView
{
public:
    BGI_API
    BgiTextureView(BgiTextureViewDesc const& desc);

    BGI_API
    virtual ~BgiTextureView();

    /// Set the handle to the texture that aliases another texture.
    BGI_API
    void SetViewTexture(BgiTextureHandle const& handle);

    /// Returns the handle to the texture that aliases another texture.
    BGI_API
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

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
