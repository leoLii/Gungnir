#pragma once

#include "common/arch/export.h"

#if defined(GUNGNIR_STATIC)
#   define BGIVULKAN_API
#   define BGIVULKAN_API_TEMPLATE_CLASS(...)
#   define BGIVULKAN_API_TEMPLATE_STRUCT(...)
#   define BGIVULKAN_LOCAL
#else
#   if defined(BGIVULKAN_EXPORTS)
#       define BGIVULKAN_API ARCH_EXPORT
#       define BGIVULKAN_API_TEMPLATE_CLASS(...) ARCH_EXPORT_TEMPLATE(class, __VA_ARGS__)
#       define BGIVULKAN_API_TEMPLATE_STRUCT(...) ARCH_EXPORT_TEMPLATE(struct, __VA_ARGS__)
#   else
#       define BGIVULKAN_API ARCH_IMPORT
#       define BGIVULKAN_API_TEMPLATE_CLASS(...) ARCH_IMPORT_TEMPLATE(class, __VA_ARGS__)
#       define BGIVULKAN_API_TEMPLATE_STRUCT(...) ARCH_IMPORT_TEMPLATE(struct, __VA_ARGS__)
#   endif
#   define BGIVULKAN_LOCAL ARCH_HIDDEN
#endif
