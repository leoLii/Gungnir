#ifndef GUNGNIR_DRIVER_BASE_CMDS_H
#define GUNGNIR_DRIVER_BASE_CMDS_H

#include "base.h"
#include "enums.h"
#include <memory>

GUNGNIR_NAMESPACE_OPEN_SCOPE

class Bgi;

using BgiCmdsUniquePtr = std::unique_ptr<class BgiCmds>;

/// \class BgiCmds
///
class BgiCmds
{
public:
    virtual ~BgiCmds();

    bool IsSubmitted() const;

protected:
    friend class Bgi;

    BgiCmds();

    virtual bool _Submit(Bgi* bgi, BgiSubmitWaitType waitType);

    void _SetSubmitted();

    private:
    BgiCmds & operator=(const BgiCmds&) = delete;
    BgiCmds(const BgiCmds&) = delete;

    bool _submitted;
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE
#endif // GUNGNIR_DRIVER_CMDS_H
