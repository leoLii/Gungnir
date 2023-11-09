#pragma once

#include "common/base.h"

#include "common/utils/api.h"

#include <iostream>
#include <stdarg.h>

GUNGNIR_NAMESPACE_OPEN_SCOPE

#define UTILS_VERIFY(cond, ...) \
    ((cond) ? true : \
    utils::verifyFailedHelper(# cond, utils::verifyStringFormat(__VA_ARGS__)))

namespace utils {

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

GUNGNIR_NAMESPACE_CLOSE_SCOPE
