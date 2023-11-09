#pragma once

#include "common/base.h"

#include "driver/bgiBase/api.h"

#include <stdint.h>

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

/// \class HgiHandle
///
/// Handle that contains a hgi object and unique id.
///
/// The unique id is used to compare two handles to guard against pointer
/// aliasing, where the same memory address is used to create a similar object,
/// but it is not actually the same object.
///
/// Handle is not a shared or weak_ptr and destruction of the contained object
/// should be explicitely managed by the client via the HgiDestroy*** functions.
///
/// If shared/weak ptr functionality is desired, the client creating Hgi objects
/// can wrap the returned handle in a shared_ptr.
///
template<class T>
class BgiHandle
{
public:
    BgiHandle() : _ptr(nullptr), _id(0) {}
    BgiHandle(T* obj, uint64_t id) : _ptr(nullptr), _id(0) {}

    T*
    Get() const {
        return _ptr;
    }

    // Note this only checks if a ptr is set, it does not offer weak_ptr safety.
    explicit operator bool() const {return _ptr!=nullptr;}

    // Pointer access operator
    T* operator ->() const {return _ptr;}

    bool operator==(const BgiHandle& other) const {
        return _id == other._id;
    }

    bool operator!=(const BgiHandle& other) const {
        return !(*this == other);
    }

private:
    T* _ptr;
    uint64_t _id;
};

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
