#ifndef GUNGNIR_DRIVER_BASE_TYPES_H
#define GUNGNIR_DRIVER_BASE_TYPES_H

#include "core/base.h"
#include "core/math/math.h"

#include "driver/bgiBase/api.h"

#include <vector>
#include <limits>
#include <stdlib.h>

GUNGNIR_NAMESPACE_OPEN_SCOPE

/// \enum HgiFormat
///
/// HgiFormat describes the memory format of image buffers used in Hgi.
/// These formats are closely aligned with HdFormat and allow us to keep Hgi
/// independent of Hd.
///
/// For reference, see:
///   https://www.khronos.org/registry/vulkan/specs/1.1/html/vkspec.html#VkFormat
enum BgiFormat : int
{
    BgiFormatInvalid = -1,

    // UNorm8 - a 1-byte value representing a float between 0 and 1.
    // float value = (unorm / 255.0f);
    BgiFormatUNorm8 = 0,
    BgiFormatUNorm8Vec2,
    /* HgiFormatUNorm8Vec3 */ // Unsupported Metal (MTLPixelFormat)
    BgiFormatUNorm8Vec4,

    // SNorm8 - a 1-byte value representing a float between -1 and 1.
    // float value = max(snorm / 127.0f, -1.0f);
    BgiFormatSNorm8,
    BgiFormatSNorm8Vec2,
    /* HgiFormatSNorm8Vec3 */ // Unsupported Metal (MTLPixelFormat)
    BgiFormatSNorm8Vec4,

    // Float16 - a 2-byte IEEE half-precision float.
    BgiFormatFloat16,
    BgiFormatFloat16Vec2,
    BgiFormatFloat16Vec3,
    BgiFormatFloat16Vec4,

    // Float32 - a 4-byte IEEE float.
    BgiFormatFloat32,
    BgiFormatFloat32Vec2,
    BgiFormatFloat32Vec3,
    BgiFormatFloat32Vec4,

    // Int16 - a 2-byte signed integer
    BgiFormatInt16,
    BgiFormatInt16Vec2,
    BgiFormatInt16Vec3,
    BgiFormatInt16Vec4,

    // UInt16 - a 2-byte unsigned integer
    BgiFormatUInt16,
    BgiFormatUInt16Vec2,
    BgiFormatUInt16Vec3,
    BgiFormatUInt16Vec4,

    // Int32 - a 4-byte signed integer
    BgiFormatInt32,
    BgiFormatInt32Vec2,
    BgiFormatInt32Vec3,
    BgiFormatInt32Vec4,

    // UNorm8 SRGB - a 1-byte value representing a float between 0 and 1.
    // Gamma compression/decompression happens during read/write.
    // Alpha component is linear.
    /* HgiFormatUNorm8srgb */     // Unsupported by OpenGL
    /* HgiFormatUNorm8Vec2srgb */ // Unsupported by OpenGL
    /* HgiFormatUNorm8Vec3srgb */ // Unsupported Metal (MTLPixelFormat)
    BgiFormatUNorm8Vec4srgb,

    // BPTC compressed. 3-component, 4x4 blocks, signed floating-point
    BgiFormatBC6FloatVec3,

    // BPTC compressed. 3-component, 4x4 blocks, unsigned floating-point
    BgiFormatBC6UFloatVec3,

    // BPTC compressed. 4-component, 4x4 blocks, unsigned byte.
    // Representing a float between 0 and 1.
    BgiFormatBC7UNorm8Vec4,

    // BPTC compressed. 4-component, 4x4 blocks, unsigned byte, sRGB.
    // Representing a float between 0 and 1.
    BgiFormatBC7UNorm8Vec4srgb,

    // S3TC/DXT compressed. 4-component, 4x4 blocks, unsigned byte
    // Representing a float between 0 and 1.
    BgiFormatBC1UNorm8Vec4,

    // S3TC/DXT compressed. 4-component, 4x4 blocks, unsigned byte
    // Representing a float between 0 and 1.
    BgiFormatBC3UNorm8Vec4,

    // Depth stencil format (Float32 can be used for just depth)
    BgiFormatFloat32UInt8,

    // Packed 32-bit value with four normalized signed two's complement
    // integer values arranged as 10 bits, 10 bits, 10 bits, and 2 bits.
    BgiFormatPackedInt1010102,

    BgiFormatCount
};

/// \class HgiMipInfo
///
/// HgiMipInfo describes size and other info for a mip level.
struct BgiMipInfo
{
    /// Offset in bytes from start of texture data to start of mip map.
    std::size_t byteOffset;
    /// Dimension of mip GfVec3i.
    Vector3i dimensions;
    /// size of (one layer if array of) mip map in bytes.
    std::size_t byteSizePerLayer;
};

/// Return the count of components in the given format.
BGI_API
std::size_t BgiGetComponentCount(BgiFormat f);

/// Return the size of a single element of the given format.
///
/// For an uncompressed format, returns the number of bytes per pixel
/// and sets blockWidth and blockHeight to 1.
/// For a compressed format (e.g., BC6), returns the number of bytes per
/// block and sets blockWidth and blockHeight to the width and height of
/// a block.
///
BGI_API
std::size_t BgiGetDataSizeOfFormat(
    BgiFormat f,
    std::size_t* blockWidth = nullptr,
    std::size_t* blockHeight = nullptr);

/// Return whether the given format uses compression.
BGI_API
bool BgiIsCompressed(BgiFormat f);

/// Returns the size necessary to allocate a buffer of given dimensions
/// and format, rounding dimensions up to suitable multiple when
/// using a compressed format.
BGI_API
std::size_t BgiGetDataSize(
    BgiFormat f,
    const Vector3i& dimensions);

/// Returns the scalar type of the format, in the form of an HgiFormat, if
/// possible.
BGI_API
BgiFormat BgiGetComponentBaseFormat(
    BgiFormat f);

/// Returns mip infos.
///
/// If dataByteSize is specified, the levels stops when the total memory
/// required by all levels up to that point reach the specified value.
/// Otherwise, the levels stop when all dimensions are 1.
/// Mip map sizes are calculated by dividing the previous mip level by two and
/// rounding down to the nearest integer (minimum integer is 1).
/// level 0: 37x53
/// level 1: 18x26
/// level 2: 9x13
/// level 3: 4x6
/// level 4: 2x3
/// level 5: 1x1
BGI_API
std::vector<BgiMipInfo>
BgiGetMipInfos(
    BgiFormat format,
    const Vector3i& dimensions,
    const std::size_t layerCount,
    const std::size_t dataByteSize = std::numeric_limits<std::size_t>::max());

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif // GUNGNIR_DRIVER_BASE_TYPES_H
