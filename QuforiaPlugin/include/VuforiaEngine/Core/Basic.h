/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_BASIC_H_
#define _VU_BASIC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <VuforiaEngine/Core/System.h>

#include <stdint.h> // Required for standard types (int32_t, etc.)

/** \addtogroup BasicTypeGroup Basic Vuforia Types
 * \{
 */

/// \brief Boolean value for true
#define VU_TRUE 1

/// \brief Boolean value for false
#define VU_FALSE 0

/// \brief Boolean type
typedef uint32_t VuBool;

/// \brief Error code type
/**
 * \note The error code's data type is the same as the underlying data type of VU_ENUM values.
 */
typedef int32_t VuErrorCode;

/// \brief Bitflag code type
typedef uint32_t VuFlags;

/// \brief Error code result from an operation
/**
 * \note When a function fails by returning VU_FAILED, check the function-specific error code in the respective out parameter.
 */
VU_ENUM(VuResult)
{
    VU_FAILED = 0x0, ///< Failed operation
    VU_SUCCESS = 0x1 ///< Successful operation
};

/// \brief Vuforia Controller handle
typedef struct VuController_ VuController;

/// \brief Rotation angle for camera intrinsics, rendering, etc.
VU_ENUM(VuRotation)
{
    VU_ROTATION_ANGLE_0 = 0x1,   ///< 0 degrees
    VU_ROTATION_ANGLE_90 = 0x2,  ///< 90 degrees
    VU_ROTATION_ANGLE_180 = 0x3, ///< 180 degrees
    VU_ROTATION_ANGLE_270 = 0x4  ///< 270 degrees
};

/** \} */

/** \addtogroup MathTypeGroup Math data types
 * \{
 * Vector and matrix types for math operation
 */

/// \brief 4x4 matrix (float)
/**
 * \note Elements are stored in column-major order:
 * data[ 0], data[ 4], data[ 8], data[12]
 * data[ 1], data[ 5], data[ 9], data[13]
 * data[ 2], data[ 6], data[10], data[14]
 * data[ 3], data[ 7], data[11], data[15]
 *
 * When the matrix represents a pose, the Vuforia Engine uses the OpenGL column-major
 * matrix convention with a right-handed coordinate system on all platforms, devices
 * and rendering backends as a cross-platform representation:
 *
 * - X axis points to the right
 * - Y axis points upwards
 * - Z axis points towards viewer
 *
 *      Y+
 *      |
 *      |
 *      O-----X+
 *     /
 *    /
 *   Z+
 *
 * Rendering solutions using a backend with the same matrix convention as OpenGL can
 * use the matrix as is, while those with a different convention (e.g. DirectX) require
 * conversion before use.
 */
typedef struct VuMatrix44F
{
    /// \brief Data member for storing matrix values
    float data[16];
} VuMatrix44F;

/// \brief 3x3 matrix (float)
/**
 * \note Elements are stored in column-major order:
 * data[0], data[3], data[6]
 * data[1], data[4], data[7]
 * data[2], data[5], data[8]
 */
typedef struct VuMatrix33F
{
    /// \brief Data member for storing matrix values
    float data[9];
} VuMatrix33F;

/// \brief 2D vector (integer)
typedef struct VuVector2I
{
    /// \brief Data member for storing vector values
    int32_t data[2];
} VuVector2I;

/// \brief 2D vector (float)
typedef struct VuVector2F
{
    /// \brief Data member for storing vector values
    float data[2];
} VuVector2F;

/// \brief 3D vector (integer)
typedef struct VuVector3I
{
    /// \brief Data member for storing vector values
    int32_t data[3];
} VuVector3I;

/// \brief 3D vector (float)
typedef struct VuVector3F
{
    /// \brief Data member for storing vector values
    float data[3];
} VuVector3F;

/// \brief 4D vector (integer)
typedef struct VuVector4I
{
    /// \brief Data member for storing vector values
    int32_t data[4];
} VuVector4I;

/// \brief 4D vector (float)
typedef struct VuVector4F
{
    /// \brief Data member for storing vector values
    float data[4];
} VuVector4F;

/// \brief 8D vector (float)
typedef struct VuVector8F
{
    /// \brief Data member for storing vector values
    float data[8];
} VuVector8F;

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_BASIC_H_
