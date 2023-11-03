#ifndef GUNGNIR_CORE_UTILS_DIAGNOSTIC_H
#define GUNGNIR_CORE_UTILS_DIAGNOSTIC_H

#include "core/base.h"

#include "core/utils/api.h"

#include <iostream>
#include <stdarg.h>

namespace utils {

#define UTILS_VERIFY(cond, ...) \
    ((cond) ? true : \
    utils::verifyFailedHelper(# cond, utils::verifyStringFormat(__VA_ARGS__)))

UTILS_API bool
verifyFailedHelper(const char *condition, const char *msg);

inline const char *
verifyStringFormat() { return nullptr; }

UTILS_API const char *
verifyStringFormat(char const *format, ...);


// TODO: Alternat printf() because sometimes it's unsafe.
#define UTILS_CODING_ERROR printf
#define UTILS_WARN printf
#define UTILS_ERROR printf

}

#endif // GUNGNIR_CORE_UTILS_DIAGNOSTIC_H
