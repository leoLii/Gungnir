#pragma once

#include "common/base.h"

#include "driver/bgiBase/api.h"
#include "driver/bgiBase/enums.h"
#include "driver/bgiBase/handle.h"
#include "driver/bgiBase/types.h"

#include <string>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {
/// \struct HgiSamplerDesc
///
/// Describes the properties needed to create a GPU sampler.
///
/// <ul>
/// <li>debugName:
///   This label can be applied as debug label for GPU debugging.</li>
/// <li>magFilter:
///    The (magnification) filter used to combine pixels when the sample area is
///    smaller than a pixel.</li>
/// <li>minFilter:
///    The (minification) filter used to combine pixels when the sample area is
///    larger than a pixel.</li>
/// <li> mipFilter:
///    The filter used for combining pixels between two mipmap levels.</li>
/// <li>addressMode***: 
///    Wrapping modes.</li>
/// <li>borderColor: 
///    The border color for clamped texture values.</li>
/// <li>enableCompare: 
///    Enables sampler comparison against a reference value during lookups.</li>
/// <li>compareFunction: 
///    The comparison function to apply if sampler compare is enabled.</li>
/// </ul>
///

struct BgiSamplerDesc
{
    BGI_API
    BgiSamplerDesc()
        : magFilter(BgiSamplerFilterNearest)
        , minFilter(BgiSamplerFilterNearest)
        , mipFilter(BgiMipFilterNotMipmapped)
        , addressModeU(BgiSamplerAddressModeClampToEdge)
        , addressModeV(BgiSamplerAddressModeClampToEdge)
        , addressModeW(BgiSamplerAddressModeClampToEdge)
        , borderColor(BgiBorderColorTransparentBlack)
        , enableCompare(false)
        , compareFunction(BgiCompareFunctionNever)
    {}

    std::string debugName;
    BgiSamplerFilter magFilter;
    BgiSamplerFilter minFilter;
    BgiMipFilter mipFilter;
    BgiSamplerAddressMode addressModeU;
    BgiSamplerAddressMode addressModeV;
    BgiSamplerAddressMode addressModeW;
    BgiBorderColor borderColor;
    bool enableCompare;
    BgiCompareFunction compareFunction;
};

BGI_API
bool operator==(
    const BgiSamplerDesc& lhs,
    const BgiSamplerDesc& rhs);

BGI_API
bool operator!=(
    const BgiSamplerDesc& lhs,
    const BgiSamplerDesc& rhs);

///
/// \class HgiSampler
///
/// Represents a graphics platform independent GPU sampler resource that
/// perform texture sampling operations.
/// Samplers should be created via Hgi::CreateSampler.
///
class BgiSampler
{
public:
    BGI_API
    virtual ~BgiSampler();

    /// The descriptor describes the object.
    BGI_API
    BgiSamplerDesc const& GetDescripter() const;

    /// This function returns the handle to the Hgi backend's gpu resource, cast
    /// to a uint64_t. Clients should avoid using this function and instead
    /// use Hgi base classes so that client code works with any Hgi platform.
    /// For transitioning code to Hgi, it can however we useful to directly
    /// access a platform's internal resource handles.
    /// There is no safety provided in using this. If you by accident pass a
    /// HgiMetal resource into an OpenGL call, bad things may happen.
    /// In OpenGL this returns the GLuint resource name.
    /// In Metal this returns the id<MTLSamplerState> as uint64_t.
    /// In Vulkan this returns the VkSampler as uint64_t.
    BGI_API
    virtual uint64_t GetRawResource() const = 0;

protected:
    BGI_API
    BgiSampler(BgiSamplerDesc const& desc);

    BgiSamplerDesc _descriptor;

private:
    BgiSampler() = delete;
    BgiSampler & operator=(const BgiSampler&) = delete;
    BgiSampler(const BgiSampler&) = delete;
};

using BgiSamplerHandle = BgiHandle<BgiSampler>;
using BgiSamplerHandleVector = std::vector<BgiSamplerHandle>;

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
