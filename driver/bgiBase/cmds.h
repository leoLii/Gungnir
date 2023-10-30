#ifndef GUNGNIR_DRIVER_BASE_CMDS_H
#define GUNGNIR_DRIVER_BASE_CMDS_H

#include "core/base.h"

#include "driver/bgiBase/api.h"
#include "driver/bgiBase/enums.h"

#include <memory>

GUNGNIR_NAMESPACE_OPEN_SCOPE

class Bgi;

using BgiCmdsUniquePtr = std::unique_ptr<class BgiCmds>;

/// \class HgiCmds
///
/// Graphics commands are recorded in 'cmds' objects which are later submitted
/// to hgi. HgiCmds is the base class for other cmds objects.
///
class BgiCmds
{
public:
    BGI_API
    virtual ~BgiCmds();

    /// Returns true if the HgiCmds object has been submitted to GPU.
    BGI_API
    bool IsSubmitted() const;

protected:
    friend class Bgi;

    BGI_API
    BgiCmds();

    // Submit can be called inside of Hgi::SubmitCmds to commit the
    // command buffer to the GPU. Returns true if work was committed.
    // The default implementation returns false.
    BGI_API
    virtual bool _Submit(Bgi* bgi, BgiSubmitWaitType waitType);

    // Flags the HgiCmds object as 'submitted' to GPU.
    BGI_API
    void _SetSubmitted();

    private:
    BgiCmds & operator=(const BgiCmds&) = delete;
    BgiCmds(const BgiCmds&) = delete;

    bool _submitted;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_BASE_CMDS_H
