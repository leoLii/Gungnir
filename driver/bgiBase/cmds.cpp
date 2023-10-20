#include "cmds.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

BgiCmds::BgiCmds()
    : _submitted(false)
{}

BgiCmds::~BgiCmds() = default;

bool
BgiCmds::IsSubmitted() const
{
    return _submitted;
}

bool
BgiCmds::_Submit(Bgi* bgi, BgiSubmitWaitType waitType)
{
    return false;
}

void
BgiCmds::_SetSubmitted()
{
    _submitted = true;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
