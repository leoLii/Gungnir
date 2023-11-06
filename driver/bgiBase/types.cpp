#include "core/base.h"
#include "core/utils/diagnostic.h"

#include "driver/bgiBase/types.h"

#include <algorithm>

GUNGNIR_NAMESPACE_OPEN_SCOPE

std::size_t
BgiGetComponentCount(const BgiFormat f)
{
    switch (f) {
    case BgiFormatUNorm8:
    case BgiFormatSNorm8:
    case BgiFormatFloat16:
    case BgiFormatFloat32:
    case BgiFormatInt16:
    case BgiFormatUInt16:
    case BgiFormatInt32:
    case BgiFormatFloat32UInt8: // treat as a single component
        return 1;
    case BgiFormatUNorm8Vec2:
    case BgiFormatSNorm8Vec2:
    case BgiFormatFloat16Vec2:
    case BgiFormatFloat32Vec2:
    case BgiFormatInt16Vec2:
    case BgiFormatUInt16Vec2:
    case BgiFormatInt32Vec2:
        return 2;
    // case BgiFormatUNorm8Vec3: // Unsupported Metal (MTLPixelFormat)
    // case BgiFormatSNorm8Vec3: // Unsupported Metal (MTLPixelFormat)
    case BgiFormatFloat16Vec3:
    case BgiFormatFloat32Vec3:
    case BgiFormatInt16Vec3:
    case BgiFormatUInt16Vec3:
    case BgiFormatInt32Vec3:
    case BgiFormatBC6FloatVec3:
    case BgiFormatBC6UFloatVec3:
        return 3;
    case BgiFormatUNorm8Vec4:
    case BgiFormatSNorm8Vec4:
    case BgiFormatFloat16Vec4:
    case BgiFormatFloat32Vec4:
    case BgiFormatInt16Vec4:
    case BgiFormatUInt16Vec4:
    case BgiFormatInt32Vec4:
    case BgiFormatBC7UNorm8Vec4:
    case BgiFormatBC7UNorm8Vec4srgb:
    case BgiFormatUNorm8Vec4srgb:
    case BgiFormatBC1UNorm8Vec4:
    case BgiFormatBC3UNorm8Vec4:
    case BgiFormatPackedInt1010102:
        return 4;
    case BgiFormatCount:
    case BgiFormatInvalid:
        UTILS_CODING_ERROR("Invalid Format");
        return 0;
    }
    UTILS_CODING_ERROR("Missing Format");
    return 0;
}

size_t
BgiGetDataSizeOfFormat(
    const BgiFormat f,
    size_t * const blockWidth,
    size_t * const blockHeight)
{
    if (blockWidth) {
        *blockWidth = 1;
    }
    if (blockHeight) {
        *blockHeight = 1;
    }

    switch(f) {
    case BgiFormatUNorm8:
    case BgiFormatSNorm8:
        return 1;
    case BgiFormatUNorm8Vec2:
    case BgiFormatSNorm8Vec2:
        return 2;
    // case BgiFormatUNorm8Vec3: // Unsupported Metal (MTLPixelFormat)
    // case BgiFormatSNorm8Vec3: // Unsupported Metal (MTLPixelFormat)
    //     return 3;
    case BgiFormatUNorm8Vec4:
    case BgiFormatSNorm8Vec4:
    case BgiFormatUNorm8Vec4srgb:
        return 4;
    case BgiFormatFloat16:
    case BgiFormatInt16:
    case BgiFormatUInt16:
        return 2;
    case BgiFormatFloat16Vec2:
    case BgiFormatInt16Vec2:
    case BgiFormatUInt16Vec2:
        return 4;
    case BgiFormatFloat16Vec3:
    case BgiFormatInt16Vec3:
    case BgiFormatUInt16Vec3:
        return 6;
    case BgiFormatFloat16Vec4:
    case BgiFormatInt16Vec4:
    case BgiFormatUInt16Vec4:
        return 8;
    case BgiFormatFloat32:
    case BgiFormatInt32:
        return 4;
    case BgiFormatPackedInt1010102:
        return 4;
    case BgiFormatFloat32Vec2:
    case BgiFormatInt32Vec2:
    case BgiFormatFloat32UInt8: // XXX: implementation dependent
        return 8;
    case BgiFormatFloat32Vec3:
    case BgiFormatInt32Vec3:
        return 12;
    case BgiFormatFloat32Vec4:
    case BgiFormatInt32Vec4:
        return 16;
    case BgiFormatBC6FloatVec3:
    case BgiFormatBC6UFloatVec3:
    case BgiFormatBC7UNorm8Vec4:
    case BgiFormatBC7UNorm8Vec4srgb:
    case BgiFormatBC1UNorm8Vec4:
    case BgiFormatBC3UNorm8Vec4:
        if (blockWidth) {
            *blockWidth = 4;
        }
        if (blockHeight) {
            *blockHeight = 4;
        }
        return 16;
    case BgiFormatCount:
    case BgiFormatInvalid:
        UTILS_CODING_ERROR("Invalid Format");
        return 0;
    }
    UTILS_CODING_ERROR("Missing Format");
    return 0;
}

bool
BgiIsCompressed(const BgiFormat f)
{
    switch(f) {
    case BgiFormatBC6FloatVec3:
    case BgiFormatBC6UFloatVec3:
    case BgiFormatBC7UNorm8Vec4:
    case BgiFormatBC7UNorm8Vec4srgb:
    case BgiFormatBC1UNorm8Vec4:
    case BgiFormatBC3UNorm8Vec4:
        return true;
    default:
        return false;
    }
}

size_t
BgiGetDataSize(
    const BgiFormat format,
    const Vector3i &dimensions)
{
    size_t blockWidth, blockHeight;
    const size_t bpt =
        BgiGetDataSizeOfFormat(format, &blockWidth, &blockHeight);
    return
        ((dimensions[0] + blockWidth  - 1) / blockWidth ) *
        ((dimensions[1] + blockHeight - 1) / blockHeight) *
        std::max(1, dimensions[2]) *
        bpt;
}

BgiFormat
BgiGetComponentBaseFormat(
    const BgiFormat f)
{
    switch(f) {
    case BgiFormatUNorm8:
    case BgiFormatUNorm8Vec2:
    case BgiFormatUNorm8Vec4:
    case BgiFormatUNorm8Vec4srgb:
    case BgiFormatBC7UNorm8Vec4:
    case BgiFormatBC7UNorm8Vec4srgb:
    case BgiFormatBC1UNorm8Vec4:
    case BgiFormatBC3UNorm8Vec4:
        return BgiFormatUNorm8;
    case BgiFormatSNorm8:
    case BgiFormatSNorm8Vec2:
    case BgiFormatSNorm8Vec4:
        return BgiFormatSNorm8;
    case BgiFormatFloat16:
    case BgiFormatFloat16Vec2:
    case BgiFormatFloat16Vec3:
    case BgiFormatFloat16Vec4:
        return BgiFormatFloat16;
    case BgiFormatInt16:
    case BgiFormatInt16Vec2:
    case BgiFormatInt16Vec3:
    case BgiFormatInt16Vec4:
        return BgiFormatInt16;
    case BgiFormatUInt16:
    case BgiFormatUInt16Vec2:
    case BgiFormatUInt16Vec3:
    case BgiFormatUInt16Vec4:
        return BgiFormatUInt16;
    case BgiFormatFloat32:
    case BgiFormatFloat32Vec2:
    case BgiFormatFloat32Vec3:
    case BgiFormatFloat32Vec4:
        return BgiFormatFloat32;
    case BgiFormatInt32:
    case BgiFormatInt32Vec2:
    case BgiFormatInt32Vec3:
    case BgiFormatInt32Vec4:
        return BgiFormatInt32;
    case BgiFormatFloat32UInt8:
        return BgiFormatFloat32UInt8;
    case BgiFormatBC6FloatVec3:
        return BgiFormatBC6FloatVec3;
    case BgiFormatBC6UFloatVec3:
        return BgiFormatBC6UFloatVec3;
    case BgiFormatPackedInt1010102:
        return  BgiFormatPackedInt1010102;
    case BgiFormatCount:
    case BgiFormatInvalid:
        UTILS_CODING_ERROR("Invalid Format");
        return BgiFormatInvalid;
    }
    UTILS_CODING_ERROR("Missing Format");
    return BgiFormatInvalid;
}

uint16_t
_ComputeNumMipLevels(const Vector3i &dimensions)
{
    const int dim = std::max({dimensions[0], dimensions[1], dimensions[2]});
    
    for (uint16_t i = 1; i < 8 * sizeof(int) - 1; i++) {
        const int powerTwo = 1 << i;
        if (powerTwo > dim) {
            return i;
        }
    }
    
    // Can never be reached, but compiler doesn't know that.
    return 1;
}

std::vector<BgiMipInfo>
BgiGetMipInfos(
    const BgiFormat format,
    const Vector3i& dimensions,
    const size_t layerCount,
    const size_t dataByteSize)
{
    const bool is2DArray = layerCount > 1;
    if (is2DArray && dimensions[2] != 1) {
        UTILS_CODING_ERROR("An array of 3D textures is invalid");
    }

    const uint16_t numMips = _ComputeNumMipLevels(dimensions);

    std::vector<BgiMipInfo> result;
    result.reserve(numMips);

    size_t byteOffset = 0;
    Vector3i size = dimensions;

    for (uint16_t mipLevel = 0; mipLevel < numMips; mipLevel++) {
        const size_t byteSize = BgiGetDataSize(format, size);

        result.push_back({ byteOffset, size, byteSize });

        byteOffset += byteSize * layerCount;
        if (byteOffset >= dataByteSize) {
            break;
        }

        size[0] = std::max(size[0] / 2, 1);
        size[1] = std::max(size[1] / 2, 1);
        size[2] = std::max(size[2] / 2, 1);
    }

    return result;
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
