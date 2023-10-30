#ifndef GUNGNIR_DRIVER_BASE_API_H
#define GUNGNIR_DRIVER_BASE_API_H

#include "core/arch/export.h"

#if defined(GUNGNIR_STATIC)
#   define BGI_API
#   define BGI_API_TEMPLATE_CLASS(...)
#   define BGI_API_TEMPLATE_STRUCT(...)
#   define BGI_LOCAL
#else
#   if defined(BGI_EXPORTS)
#       define BGI_API ARCH_EXPORT
#       define BGI_API_TEMPLATE_CLASS(...) ARCH_EXPORT_TEMPLATE(class, __VA_ARGS__)
#       define BGI_API_TEMPLATE_STRUCT(...) ARCH_EXPORT_TEMPLATE(struct, __VA_ARGS__)
#   else
#       define BGI_API ARCH_IMPORT
#       define BGI_API_TEMPLATE_CLASS(...) ARCH_IMPORT_TEMPLATE(class, __VA_ARGS__)
#       define BGI_API_TEMPLATE_STRUCT(...) ARCH_IMPORT_TEMPLATE(struct, __VA_ARGS__)
#   endif
#   define BGI_LOCAL ARCH_HIDDEN
#endif

#endif // GUNGNIR_DRIVER_BGI_BASE_API_H