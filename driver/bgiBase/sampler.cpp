#include "sampler.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiSampler::BgiSampler(BgiSamplerDesc const& desc)
    : _descriptor(desc)
{
}

BgiSampler::~BgiSampler() = default;

BgiSamplerDesc const&
BgiSampler::GetDescripter() const
{
    return _descriptor;
}

bool operator==(const BgiSamplerDesc& lhs,
    const BgiSamplerDesc& rhs)
{
    return  lhs.debugName == rhs.debugName &&
            lhs.magFilter == rhs.magFilter &&
            lhs.minFilter == rhs.minFilter &&
            lhs.mipFilter == rhs.mipFilter &&
            lhs.addressModeU == rhs.addressModeU &&
            lhs.addressModeV == rhs.addressModeV &&
            lhs.addressModeW == rhs.addressModeW
    ;
}

bool operator!=(const BgiSamplerDesc& lhs,
    const BgiSamplerDesc& rhs)
{
    return !(lhs == rhs);
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE