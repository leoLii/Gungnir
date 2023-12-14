#pragma once

#include "common/macros.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

#if GUNGNIR_WINDOWS
using SharedLibraryHandle = void*; // HANDLE
using WindowHandle = void*;        // HWND
#elif GUNGNIR_LINUX
using SharedLibraryHandle = void*;
struct WindowHandle
{
    void* pDisplay;
    unsigned long window;
};
#else
#error "Platform not specified!"
#endif

GUNGNIR_NAMESPACE_CLOSE_SCOPE // namespace Gungnir
