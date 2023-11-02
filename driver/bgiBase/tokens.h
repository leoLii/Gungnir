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

// TF_DECLARE_PUBLIC_TOKENS(HgiTokens, HGI_API, HGI_TOKENS);

#define BGI_SHADER_KEYWORD_TOKENS    \
    (Position) \
    (PointCoord) \
    (ClipDistance) \
    (CullDistance) \
    (VertexID) \
    (InstanceID) \
    (PrimitiveID) \
    (SampleID) \
    (SamplePosition) \
    (FragCoord) \
    (FrontFacing) \
    (Layer) \
    (BaseVertex) \
    (BaseInstance) \
    (ViewportIndex) \
    (PositionInPatch) \
    (PatchID) \
    (GlobalInvocationID) \
    (BaryCoordNoPerspNV)

// TF_DECLARE_PUBLIC_TOKENS(
//     HgiShaderKeywordTokens, HGI_API, HGI_SHADER_KEYWORD_TOKENS);

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_BASE_TOKENS_H
