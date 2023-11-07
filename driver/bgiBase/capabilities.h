#pragma once

#include "core/base.h"

#include "driver/bgiBase/api.h"
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
    BGI_API
    virtual ~BgiCapabilities() = 0;

    bool IsSet(BgiDeviceCapabilities mask) const {
        return (_flags & mask) != 0;
    }

    BGI_API
    virtual int GetAPIVersion() const = 0;
    
    BGI_API
    virtual int GetShaderVersion() const = 0;

    BGI_API
    size_t GetMaxUniformBlockSize() const {
        return _maxUniformBlockSize;
    }

    BGI_API
    size_t GetMaxShaderStorageBlockSize() const {
        return _maxShaderStorageBlockSize;
    }

    BGI_API
    size_t GetUniformBufferOffsetAlignment() const {
        return _uniformBufferOffsetAlignment;
    }

    BGI_API
    size_t GetMaxClipDistances() const {
        return _maxClipDistances;
    }

    BGI_API
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

