#pragma once

#include "core/base.h"

GUNGNIR_NAMESPACE_OPEN_SCOPE

enum DRIVER {
    taskDriver,
    renderDriver,
    OpenGL,
    Metal,
    Vulkan
};

enum SHADER_KEYWORD {
    Position,
    PointCoord,
    ClipDistance,
    CullDistance,
    VertexID,
    InstanceID,
    PrimitiveID,
    SampleID,
    SamplePosition,
    FragCoord,
    FrontFacing,
    Layer,
    BaseVertex,
    BaseInstance,
    ViewportIndex,
    PositionInPatch,
    PatchID,
    GlobalInvocationID,
    BaryCoordNoPerspNV
};

GUNGNIR_NAMESPACE_CLOSE_SCOPE
