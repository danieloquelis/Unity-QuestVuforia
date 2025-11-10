/*===============================================================================
Copyright (c) 2024 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_CAMERA_INTRINSICS_H_
#define _VU_CAMERA_INTRINSICS_H_

#include <VuforiaEngine/Core/Basic.h>
#include <VuforiaEngine/Core/System.h>

#include <stdint.h> // Required for standard types (int32_t, etc.)

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup CameraIntrinsicsGroup Camera Intrinsics Handling
 * \{
 */

/// \brief Camera distortion model
VU_ENUM(VuCameraDistortionMode)
{
    VU_CAMERA_DISTORTION_MODE_LINEAR = 0x1,  ///< Linear model (no distortion or undistortion)
    VU_CAMERA_DISTORTION_MODE_1PARAM = 0x5,  ///< ARCTAN model with one parameter
    VU_CAMERA_DISTORTION_MODE_2PARAMS = 0x6, ///< 2 radial parameters, no tangential parameters
    VU_CAMERA_DISTORTION_MODE_3PARAMS = 0x2, ///< 3 radial parameters, no tangential parameters
    VU_CAMERA_DISTORTION_MODE_4PARAMS = 0x3, ///< 2 radial parameters, plus 2 tangential parameters
    VU_CAMERA_DISTORTION_MODE_5PARAMS = 0x4, ///< 3 radial parameters, plus 2 tangential parameters
    VU_CAMERA_DISTORTION_MODE_6PARAMS = 0x7, ///< 6 radial parameters (rational), no tangential parameters
    VU_CAMERA_DISTORTION_MODE_8PARAMS = 0x8, ///< 6 radial parameters (rational), plus 2 tangential parameters
};

/// \brief Vuforia camera intrinsics
typedef struct VuCameraIntrinsics
{
    /// \brief Camera frame resolution in pixels
    VuVector2F size;

    /// \brief Focal length in both the x and y directions
    VuVector2F focalLength;

    /// \brief Principal point
    VuVector2F principalPoint;

    /// \brief Camera distortion mode
    VuCameraDistortionMode distortionMode;

    /// \brief Radial distortion coefficients
    VuVector8F distortionParameters;
} VuCameraIntrinsics;

/// \brief Get the associated field-of-view of camera intrinsics in degrees
/**
 * \note The function returns a zero vector upon an error.
 */
VU_API VuVector2F VU_API_CALL vuCameraIntrinsicsGetFov(const VuCameraIntrinsics* intrinsics);

/// \brief Get a 3x3 matrix of the camera intrinsics using a pinhole camera model
/**
 * \note The function returns a zero matrix upon an error.
 */
VU_API VuMatrix33F VU_API_CALL vuCameraIntrinsicsGetMatrix(const VuCameraIntrinsics* intrinsics);

/// \brief Create a perspective projection matrix from camera intrinsics data that is immediately suitable for rendering in OpenGL
/**
 * \note The projection matrix uses an OpenGL-style column-major matrix with the following right-handed
 * coordinate system convention for the view space:

 * - The X coordinate system axis points to the right and the Y axis points downwards.
 * - The camera is positioned at the coordinate system origin and points in the positive Z direction.
 * - Normalized device coordinates are used where the Z coordinates are normalized to the range (-1, 1).
 *
 * \param intrinsics Camera intrinsics
 * \param nearPlane Near clipping plane
 * \param farPlane Far clipping plane
 * \param rotation Rotation to apply to the projection matrix (e.g. can be used for baking screen rotation into the projection matrix)
 * \returns Projection matrix
 */
VU_API VuMatrix44F VU_API_CALL vuCameraIntrinsicsGetProjectionMatrix(const VuCameraIntrinsics* intrinsics, float nearPlane, float farPlane,
                                                                     VuRotation rotation);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_CAMERA_INTRINSICS_H_
