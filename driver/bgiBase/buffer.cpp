#include "buffer.h"

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

GUNGNIR_NAMESPACE_CLOSE_SCOPE
