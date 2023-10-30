#ifndef GUNGNIR_DRIVER_BASE_TOKENS_H
#define GUNGNIR_DRIVER_BASE_TOKENS_H

#include "core/base.h"
// #include "pxr/base/tf/staticTokens.h"

#include "driver/bgiBase/api.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

#define BGI_TOKENS    \
    (taskDriver)      \
    (renderDriver)    \
    (OpenGL)          \
    (Metal)           \
    (Vulkan)

/*
TF_DECLARE_PUBLIC_TOKENS(HgiTokens, HGI_API, HGI_TOKENS);

#define HGI_SHADER_KEYWORD_TOKENS    \
    (hdPosition) \
    (hdPointCoord) \
    (hdClipDistance) \
    (hdCullDistance) \
    (hdVertexID) \
    (hdInstanceID) \
    (hdPrimitiveID) \
    (hdSampleID) \
    (hdSamplePosition) \
    (hdFragCoord) \
    (hdFrontFacing) \
    (hdLayer) \
    (hdBaseVertex) \
    (hdBaseInstance) \
    (hdViewportIndex) \
    (hdPositionInPatch) \
    (hdPatchID) \
    (hdGlobalInvocationID) \
    (hdBaryCoordNoPerspNV)

TF_DECLARE_PUBLIC_TOKENS(
    HgiShaderKeywordTokens, HGI_API, HGI_SHADER_KEYWORD_TOKENS);
*/

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_BASE_TOKENS_H
