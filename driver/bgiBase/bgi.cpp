#include "bgi.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

Bgi::Bgi()
    : _uniqueIdCounter(1)
{
}

Bgi::~Bgi() = default;

void
Bgi::SubmitCmds(BgiCmds* cmds, BgiSubmitWaitType wait)
{
    if (cmds && !cmds->IsSubmitted()) {
        _SubmitCmds(cmds, wait);
        cmds->_SetSubmitted();
    }
}

static Bgi*
_MakeNewPlatformDefaultBgi()
{
    const char* bgiType = "BgiVulkan";

    BgiFactoryBase* factory = BgiFactoryBase();
    Bgi* instance = factory->New();
    if (!instance) {
        // TF_CODING_ERROR("[PluginLoad] Cannot construct instance of type '%s'\n",
        //         plugType.GetTypeName().c_str());
        return nullptr;
    }

    return instance;
}

Bgi*
Bgi::GetPlatformDefaultBgi()
{
    // TF_WARN("GetPlatformDefaultHgi is deprecated. "
    //         "Please use CreatePlatformDefaultHgi");

    return _MakeNewPlatformDefaultBgi();
}

BgiUniquePtr
Bgi::CreatePlatformDefaultBgi()
{
    return BgiUniquePtr(_MakeNewPlatformDefaultBgi());
}

bool
Bgi::IsSupported()
{
    if (BgiUniquePtr const instance = CreatePlatformDefaultBgi()) {
        return instance->IsBackendSupported();
    }

    return false;
}

uint64_t
Bgi::GetUniqueId()
{
    return _uniqueIdCounter.fetch_add(1);
}

bool
Bgi::_SubmitCmds(BgiCmds* cmds, BgiSubmitWaitType wait)
{
    return cmds->_Submit(this, wait);
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
