#include "driver/bgiBase/buffer.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiBuffer::BgiBuffer(BgiBufferDesc const& desc)
    : _descriptor(desc)
{
}

BgiBuffer::~BgiBuffer() = default;

BgiBufferDesc const&
BgiBuffer::GetDescriptor() const
{
    return _descriptor;
}

bool operator==(
    const BgiBufferDesc& lhs,
    const BgiBufferDesc& rhs)
{
    return lhs.debugName == rhs.debugName &&
           lhs.usage == rhs.usage &&
           lhs.byteSize == rhs.byteSize
           // Omitted. Only used tmp during creation of buffer.
           // lhs.initialData == rhs.initialData &&
    ;
}

bool operator!=(
    const BgiBufferDesc& lhs,
    const BgiBufferDesc& rhs)
{
    return !(lhs == rhs);
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
