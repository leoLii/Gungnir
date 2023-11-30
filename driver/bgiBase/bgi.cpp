#include "common/arch/defines.h"
#include "common/utils/diagnostic.h"

#include "driver/bgiBase/bgi.h"
#include "driver/bgiVulkan/bgi.h"

// "Enable Vulkan as platform default Hgi backend (WIP)"
#define BGI_ENABLE_VULKAN 1

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace driver {

Bgi::Bgi()
    : _uniqueIdCounter(1)
{
}

Bgi::~Bgi() = default;

void
Bgi::SubmitCmds(BgiCmds* cmds, BgiSubmitWaitType wait)
{
    if (cmds && UTILS_VERIFY(!cmds->IsSubmitted())) {
        _SubmitCmds(cmds, wait);
        cmds->_SetSubmitted();
    }
}

static Bgi*
_MakeNewPlatformDefaultBgi()
{
    // We use the plugin system to construct derived Hgi classes to avoid any
    // linker complications.

    //const char* bgiType = 
    //    #if defined(ARCH_OS_DARWIN)
    //        "BgiMetal";
    //    #elif defined(ARCH_OS_WINDOWS)
    //        #if BGI_ENABLE_VULKAN == 1
    //        "BgiVulkan";
    //        #endif
    //    #else
    //        ""; 
    //        #error Unknown Platform
    //        return nullptr;
    //    #endif

    //BgiFactory<BgiVulkan>* factory = new BgiFactory<BgiVulkan>();
    //if (!factory) {
    //    UTILS_CODING_ERROR("Cannot create bgiFactory \n");
    //    return nullptr;
    //}

    Bgi* instance = new BgiVulkan();
    if (!instance) {
        UTILS_CODING_ERROR("Cannot construct instance of bgi\n");
        return nullptr;
    }

    return instance;
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

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
