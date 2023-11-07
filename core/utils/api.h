#pragma once

#include "core/arch/export.h"

#if defined(GUNGNIR_STATIC)
#   define UTILS_API
#   define UTILS_API_TEMPLATE_CLASS(...)
#   define UTILS_API_TEMPLATE_STRUCT(...)
#   define UTILS_LOCAL
#else
#   if defined(UTILS_EXPORTS)
#       define UTILS_API ARCH_EXPORT
#       define UTILS_API_TEMPLATE_CLASS(...) ARCH_EXPORT_TEMPLATE(class, __VA_ARGS__)
#       define UTILS_API_TEMPLATE_STRUCT(...) ARCH_EXPORT_TEMPLATE(struct, __VA_ARGS__)
#   else
#       define UTILS_API ARCH_IMPORT
#       define UTILS_API_TEMPLATE_CLASS(...) ARCH_IMPORT_TEMPLATE(class, __VA_ARGS__)
#       define UTILS_API_TEMPLATE_STRUCT(...) ARCH_IMPORT_TEMPLATE(struct, __VA_ARGS__)
#   endif
#   define UTILS_LOCAL ARCH_HIDDEN
#endif
