#pragma once

#include "core/base.h"

#include "driver/bgiBase/api.h"
#include "driver/bgiBase/buffer.h"
#include "driver/bgiBase/enums.h"
#include "driver/bgiBase/handle.h"
#include "driver/bgiBase/sampler.h"
#include "driver/bgiBase/texture.h"
#include "driver/bgiBase/types.h"

#include <string>
#include <vector>

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

/// \struct HgiBufferBindDesc
///
/// Describes the binding information of a buffer (or array of buffers).
///
/// <ul>
/// <li>buffers:
///   The buffer(s) to be bound.
///   If there are more than one buffer, the buffers will be put in an
///   array-of-buffers. Please note that different platforms have varying
///   limits to max buffers in an array.</li>
/// <li>offsets:
///    Offset (in bytes) where data begins from the start of the buffer.
///    There is an offset corresponding to each buffer in 'buffers'.</li>
/// <li>sizes:
///    Size (in bytes) of the range of data in the buffer to bind.
///    There is a size corresponding to each buffer in 'buffers'.
///    If sizes is empty or the size for a buffer is specified as zero,
///    then the entire buffer is bound.
///    If the offset for a buffer is non-zero, then a non-zero size must
///    also be specified.</li>
/// <li>resourceType:
///    The type of buffer(s) that is to be bound.
///    All buffers in the array must have the same type.
///    Vertex, index and indirect buffers are not bound to a resourceSet.
///    They are instead passed to the draw command.</li>
/// <li>bindingIndex:
///    Binding location for the buffer(s).</li>
/// <li>stageUsage:
///    What shader stage(s) the buffer will be used in.</li>
/// <li>writable:
///    Whether the buffer binding should be non-const.</li>
/// </ul>
///
struct BgiBufferBindingDesc
{
    BGI_API
    BgiBufferBindingDesc();

    BgiBufferHandleVector buffers;
    std::vector<uint32_t> offsets;
    std::vector<uint32_t> sizes;
    BgiBindResourceType resourceType;
    uint32_t bindingIndex;
    BgiShaderStage stageUsage;
    bool writable;
};
using BgiBufferBindingDescVector = std::vector<BgiBufferBindingDesc>;

BGI_API
bool operator==(
    const BgiBufferBindingDesc& lhs,
    const BgiBufferBindingDesc& rhs);

BGI_API
bool operator!=(
    const BgiBufferBindingDesc& lhs,
    const BgiBufferBindingDesc& rhs);


/// \struct HgiTextureBindDesc
///
/// Describes the binding information of a texture (or array of textures).
///
/// <ul>
/// <li>textures:
///   The texture(s) to be bound.
///   If there are more than one texture, the textures will be put in an
///   array-of-textures (not texture-array). Please note that different
///   platforms have varying limits to max textures in an array.</li>
/// <li>samplers:
///   (optional) The sampler(s) to be bound for each texture in `textures`.
///   If empty a default sampler (clamp_to_edge, linear) should be used. </li>
/// <li>resourceType:
///    The type of the texture(s) that is to be bound.
///    All textures in the array must have the same type.</li>
/// <li>bindingIndex:
///    Binding location for the texture</li>
/// <li>stageUsage:
///    What shader stage(s) the texture will be used in.</li>
/// <li>writable:
///    Whether the texture binding should be non-const.</li>
/// </ul>
///
struct BgiTextureBindingDesc
{
    BGI_API
    BgiTextureBindingDesc();

    BgiTextureHandleVector textures;
    BgiSamplerHandleVector samplers;
    BgiBindResourceType resourceType;
    uint32_t bindingIndex;
    BgiShaderStage stageUsage;
    bool writable;
};
using BgiTextureBindingDescVector = std::vector<BgiTextureBindingDesc>;

BGI_API
bool operator==(
    const BgiTextureBindingDesc& lhs,
    const BgiTextureBindingDesc& rhs);

BGI_API
bool operator!=(
    const BgiTextureBindingDesc& lhs,
    const BgiTextureBindingDesc& rhs);


/// \struct HgiResourceBindingsDesc
///
/// Describes a set of resources that are bound to the GPU during encoding.
///
/// <ul>
/// <li>buffers:
///   The buffers to be bound (E.g. uniform or shader storage).</li>
/// <li>textures:
///   The textures to be bound.</li>
/// </ul>
///
struct BgiResourceBindingsDesc
{
    BGI_API
    BgiResourceBindingsDesc();

    std::string debugName;
    BgiBufferBindingDescVector buffers;
    BgiTextureBindingDescVector textures;
};

BGI_API
bool operator==(
    const BgiResourceBindingsDesc& lhs,
    const BgiResourceBindingsDesc& rhs);

BGI_API
bool operator!=(
    const BgiResourceBindingsDesc& lhs,
    const BgiResourceBindingsDesc& rhs);

///
/// \class HgiResourceBindings
///
/// Represents a collection of buffers, texture and vertex attributes that will
/// be used by an cmds object (and pipeline).
///
class BgiResourceBindings
{
public:
    BGI_API
    virtual ~BgiResourceBindings();

    /// The descriptor describes the object.
    BGI_API
    BgiResourceBindingsDesc const& GetDescriptor() const;

protected:
BGI_API
    BgiResourceBindings(BgiResourceBindingsDesc const& desc);

    BgiResourceBindingsDesc _descriptor;

private:
    BgiResourceBindings() = delete;
    BgiResourceBindings & operator=(const BgiResourceBindings&) = delete;
    BgiResourceBindings(const BgiResourceBindings&) = delete;
};

using BgiResourceBindingsHandle = BgiHandle<BgiResourceBindings>;
using BgiResourceBindingsHandleVector = std::vector<BgiResourceBindingsHandle>;

/// \struct HgiVertexBufferBinding
///
/// Describes a buffer to be bound during encoding.
///
/// <ul>
/// <li>buffer:
///   The buffer to be bound (e.g. uniform, storage, vertex).</li>
/// <li>byteOffset:
///   The byte offset into the buffer from where the data will be bound.</li>
/// <li>index:
///   The binding index to which the buffer will be bound.</li>
/// </ul>
///
struct BgiVertexBufferBinding
{
    BGI_API
    BgiVertexBufferBinding(BgiBufferHandle const &buffer,
                           uint32_t byteOffset,
                           uint32_t index)
        : buffer(buffer)
        , byteOffset(byteOffset)
        , index(index)
    {
    }

    BgiBufferHandle buffer;
    uint32_t byteOffset;
    uint32_t index;
};

using BgiVertexBufferBindingVector = std::vector<BgiVertexBufferBinding>;

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
