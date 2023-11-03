#ifndef GUNGNIR_CORE_DEFINES_H
#define GUNGNIR_CORE_DEFINES_H

#define GUNGNIR_NAMESPACE_OPEN_SCOPE namespace gungnir {
#define GUNGNIR_NAMESPACE_CLOSE_SCOPE }

//
// OS
//

#if defined(__linux__)
#define ARCH_OS_LINUX
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#define ARCH_OS_DARWIN
#if TARGET_OS_IPHONE
#define ARCH_OS_IOS
#else
#define ARCH_OS_OSX
#endif
#elif defined(_WIN32) || defined(_WIN64)
#define ARCH_OS_WINDOWS
#endif

#endif // GUNGNIR_CORE_DEFINES_H
