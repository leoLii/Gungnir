#ifndef GUNGNIR_DRIVER_BASE_BUFFER_H
#define GUNGNIR_DRIVER_BASE_BUFFER_H

#include <vector>
#include <string>

#include "base.h"
#include "enums.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

/// \struct BgiBufferDesc
///
struct BgiBufferDesc
{
    BgiBufferDesc()
    : usage(BgiBufferUsageUniform)
    , byteSize(0)
    , vertexStride(0)
    , initialData(nullptr)
    {}

    std::string debugName;
    BgiBufferUsage usage;
    size_t byteSize;
    uint32_t vertexStride;
    void const* initialData;
};

/// \class BgiBuffer
///
class BgiBuffer 
{
public:
    virtual ~BgiBuffer();

    BgiBufferDesc const& GetDescriptor() const;

    virtual size_t GetByteSizeOfResource() const = 0;

    virtual uint64_t GetRawResource() const = 0;

    virtual void* GetCPUStagingAddress() = 0;

protected:
    BgiBuffer(BgiBufferDesc const& desc);

    BgiBufferDesc _descriptor;

private:
    BgiBuffer() = delete;
    BgiBuffer & operator=(const BgiBuffer&) = delete;
    BgiBuffer(const BgiBuffer&) = delete;
};

using BgiBufferHandle = BgiHandle<BgiBuffer>;
using BgiBufferHandleVector = std::vector<BgiBufferHandle>;

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif
