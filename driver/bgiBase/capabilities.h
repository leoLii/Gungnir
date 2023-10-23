#ifndef GUNGNIR_DRIVER_BASE_CAPABILITIES_H
#define GUNGNIR_DRIVER_BASE_CAPABILITIES_H

#include "core/base.h"

#include "driver/bgiBase/enums.h"

#include <cstddef>

GUNGNIR_NAMESPACE_OPEN_SCOPE

/// \class HgiCapabilities
///
/// Reports the capabilities of the Hgi device.
///
class BgiCapabilities
{
public:
    virtual ~BgiCapabilities() = 0;

    bool IsSet(BgiDeviceCapabilities mask) const {
        return (_flags & mask) != 0;
    }

    virtual int GetAPIVersion() const = 0;
    
    virtual int GetShaderVersion() const = 0;

    size_t GetMaxUniformBlockSize() const {
        return _maxUniformBlockSize;
    }

    size_t GetMaxShaderStorageBlockSize() const {
        return _maxShaderStorageBlockSize;
    }

    size_t GetUniformBufferOffsetAlignment() const {
        return _uniformBufferOffsetAlignment;
    }

    size_t GetMaxClipDistances() const {
        return _maxClipDistances;
    }

    size_t GetPageSizeAlignment() const {
        return _pageSizeAlignment;
    }

protected:
    BgiCapabilities()
        : _maxUniformBlockSize(0)
        , _maxShaderStorageBlockSize(0)
        , _uniformBufferOffsetAlignment(0)
        , _maxClipDistances(0)
        , _pageSizeAlignment(1)
        , _flags(0)
    {}

    void _SetFlag(BgiDeviceCapabilities mask, bool value) {
        if (value) {
            _flags |= mask;
        } else {
            _flags &= ~mask;
        }
    }

    size_t _maxUniformBlockSize;
    size_t _maxShaderStorageBlockSize;
    size_t _uniformBufferOffsetAlignment;
    size_t _maxClipDistances;
    size_t _pageSizeAlignment; 

private:
    BgiCapabilities & operator=(const BgiCapabilities&) = delete;
    BgiCapabilities(const BgiCapabilities&) = delete;

    BgiDeviceCapabilities _flags;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif
