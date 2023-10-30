#include "driver/bgiBase/resourceBindings.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiResourceBindings::BgiResourceBindings(BgiResourceBindingsDesc const& desc)
    : _descriptor(desc)
{
}

BgiResourceBindings::~BgiResourceBindings() = default;

BgiResourceBindingsDesc const&
BgiResourceBindings::GetDescriptor() const
{
    return _descriptor;
}

BgiBufferBindingDesc::BgiBufferBindingDesc()
    : bindingIndex(0)
    , stageUsage(BgiShaderStageVertex | BgiShaderStagePostTessellationVertex)
    , writable(false)
{
}

bool operator==(
    const BgiBufferBindingDesc& lhs,
    const BgiBufferBindingDesc& rhs)
{
    return lhs.buffers == rhs.buffers &&
           lhs.resourceType == rhs.resourceType &&
           lhs.offsets == rhs.offsets &&
           lhs.sizes == rhs.sizes &&
           lhs.bindingIndex == rhs.bindingIndex &&
           lhs.stageUsage == rhs.stageUsage &&
           lhs.writable == rhs.writable;
}

bool operator!=(
    const BgiBufferBindingDesc& lhs,
    const BgiBufferBindingDesc& rhs)
{
    return !(lhs == rhs);
}

BgiTextureBindingDesc::BgiTextureBindingDesc()
    : resourceType(BgiBindResourceTypeCombinedSamplerImage)
    , bindingIndex(0)
    , stageUsage(BgiShaderStageFragment)
    , writable(false)
{
}

bool operator==(
    const BgiTextureBindingDesc& lhs,
    const BgiTextureBindingDesc& rhs)
{
    return lhs.textures == rhs.textures &&
           lhs.resourceType == rhs.resourceType &&
           lhs.bindingIndex == rhs.bindingIndex &&
           lhs.stageUsage == rhs.stageUsage &&
           lhs.samplers == rhs.samplers &&
           lhs.writable == rhs.writable;
}

bool operator!=(
    const BgiTextureBindingDesc& lhs,
    const BgiTextureBindingDesc& rhs)
{
    return !(lhs == rhs);
}

BgiResourceBindingsDesc::BgiResourceBindingsDesc() = default;

bool operator==(
    const BgiResourceBindingsDesc& lhs,
    const BgiResourceBindingsDesc& rhs)
{
    return lhs.debugName == rhs.debugName &&
           lhs.buffers == rhs.buffers &&
           lhs.textures == rhs.textures;
}

bool operator!=(
    const BgiResourceBindingsDesc& lhs,
    const BgiResourceBindingsDesc& rhs)
{
    return !(lhs == rhs);
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
