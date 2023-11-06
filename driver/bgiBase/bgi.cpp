#include "core/arch/defines.h"
#include "core/utils/diagnostic.h"

#include "driver/bgiBase/bgi.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

// "Enable Vulkan as platform default Hgi backend (WIP)"
#define BGI_ENABLE_VULKAN 1

Bgi::Bgi()
    : _uniqueIdCounter(1)
{
}

Bgi::~Bgi() = default;

void
Bgi::SubmitCmds(BgiCmds* cmds, BgiSubmitWaitType wait)
{
    TRACE_FUNCTION();

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

    PlugRegistry& plugReg = PlugRegistry::GetInstance();

    const char* bgiType = 
        #if defined(ARCH_OS_DARWIN)
            "BgiMetal";
        #elif defined(ARCH_OS_WINDOWS)
            #if BGI_ENABLE_VULKAN == 1
            "BgiVulkan";
            #endif
        #else
            ""; 
            #error Unknown Platform
            return nullptr;
        #endif

    const TfType plugType = plugReg.FindDerivedTypeByName<Bgi>(bgiType);

    PlugPluginPtr plugin = plugReg.GetPluginForType(plugType);
    if (!plugin || !plugin->Load()) {
        UTILS_CODING_ERROR(
            "[PluginLoad] PlugPlugin could not be loaded for TfType '%s'\n",
            plugType.GetTypeName().c_str());
        return nullptr;
    }

    BgiFactoryBase* factory = plugType.GetFactory<BgiFactoryBase>();
    if (!factory) {
        UTILS_CODING_ERROR("[PluginLoad] Cannot manufacture type '%s' \n",
                plugType.GetTypeName().c_str());
        return nullptr;
    }

    Bgi* instance = factory->New();
    if (!instance) {
        UTILS_CODING_ERROR("[PluginLoad] Cannot construct instance of type '%s'\n",
                plugType.GetTypeName().c_str());
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

GUNGNIR_NAMESPACE_CLOSE_SCOPE
