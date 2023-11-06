#ifndef GUNGNIR_CORE_MATH_MATH_H
#define GUNGNIR_CORE_MATH_MATH_H

#include "core/base.h"

// This defination must be placed before including Eigen, cause Eigen might have included windowskit 
// and windowskit has defined min and max, it's a disaster when you use your own min max functions
// because compiler will recognize your min max function as definations in windowskit.
// In the future we will replace eigen.
#define NOMINMAX
#include <Eigen/Dense>

GUNGNIR_NAMESPACE_OPEN_SCOPE

using Vector2f = Eigen::Vector2f;
using Vector2d = Eigen::Vector2d;
using Vector2i = Eigen::Vector2i;

using Vector3f = Eigen::Vector3f;
using Vector3d = Eigen::Vector3d;
using Vector3i = Eigen::Vector3i;

using Vector4f = Eigen::Vector4f;
using Vector4d = Eigen::Vector4d;
using Vector4i = Eigen::Vector4i;

using Matrixf = Eigen::Matrix4f;
using Matrixd = Eigen::Matrix4d;
using Matrixi = Eigen::Matrix4i;

GUNGNIR_NAMESPACE_CLOSE_SCOPE

#endif
