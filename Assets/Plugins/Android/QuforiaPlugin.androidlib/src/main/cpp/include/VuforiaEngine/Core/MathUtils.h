/*===============================================================================
Copyright (c) 2024 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_MATHUTILS_H_
#define _VU_MATHUTILS_H_

/**
 * \file MathUtils.h
 * \brief Linear algebra-related utility functions
 */

#include <VuforiaEngine/Core/Basic.h>
#include <VuforiaEngine/Core/System.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup MathUtilityGroup Math Utilities
 * \{
 * Linear algebra-related data types and functions used by Vuforia Engine
 */

// Vector 2D Math Utilities

/// \brief Get a 2D zero vector (0,0)
VU_API VuVector2F VU_API_CALL vuZeroVector2F();

/// \brief Get a 2D one vector (1,1)
VU_API VuVector2F VU_API_CALL vuOneVector2F();

/// \brief Negate a 2D vector
/**
 * \note result = -v
 */
VU_API VuVector2F VU_API_CALL vuVector2FNegate(VuVector2F v);

/// \brief Get the absolute vector of a 2D vector
/**
 * \note result = |v|
 */
VU_API VuVector2F VU_API_CALL vuVector2FAbs(VuVector2F v);

/// \brief Compute the Euclidian norm of a 2D vector
/**
 * \note result = ||v||
 */
VU_API float VU_API_CALL vuVector2FMag(VuVector2F v);

/// \brief Compute the Squared Euclidian norm of a 2D vector
/**
 * \note result = ||v||^2
 */
VU_API float VU_API_CALL vuVector2FMagSquared(VuVector2F v);

/// \brief Get a normalized 2D vector
/**
 * \note result = v/||v||
 */
VU_API VuVector2F VU_API_CALL vuVector2FNormalize(VuVector2F v);

/// \brief Multiply a 2D vector by a scalar value
/**
 * \note result = v * s
 */
VU_API VuVector2F VU_API_CALL vuVector2FScale(VuVector2F v, float s);

/// \brief Get the sum of two 2D vectors
/**
 * \note result = v1 + v2
 */
VU_API VuVector2F VU_API_CALL vuVector2FAdd(VuVector2F v1, VuVector2F v2);

/// \brief Get the difference of two 2D vectors
/**
 * \note result = v1 - v2
 */
VU_API VuVector2F VU_API_CALL vuVector2FSub(VuVector2F v1, VuVector2F v2);

/// \brief Compute the angle between two 2D vectors in degrees
VU_API float VU_API_CALL vuVector2FAngle(VuVector2F v1, VuVector2F v2);

/// \brief Compute the Euclidean distance between two 2D vectors
VU_API float VU_API_CALL vuVector2FDist(VuVector2F v1, VuVector2F v2);

/// \brief Compute the linear interpolation between two 2D vectors
/**
 * \note result = v1*t+(1-t)*v2
 */
VU_API VuVector2F VU_API_CALL vuVector2FLerp(VuVector2F v1, VuVector2F v2, float t);

/// \brief Compute the dot product of two 2D vectors
/**
 * \note result = v1.v2
 */
VU_API float VU_API_CALL vuVector2FDot(VuVector2F v1, VuVector2F v2);


// Vector 3D Math Utilities

/// \brief Get a 3D zero vector (0,0,0)
VU_API VuVector3F VU_API_CALL vuZeroVector3F();

/// \brief Get a 3D one vector (1,1,1)
VU_API VuVector3F VU_API_CALL vuOneVector3F();

/// \brief Convert a 2D vector to a 3D vector (insert 0 as the last component)
VU_API VuVector3F VU_API_CALL vuVector2FToVector3F(VuVector2F v);

/// \brief Negate a 3D vector
/**
 * \note result = -v
 */
VU_API VuVector3F VU_API_CALL vuVector3FNegate(VuVector3F v);

/// \brief Get the absolute vector of a 3D vector
/**
 * \note result = |v|
 */
VU_API VuVector3F VU_API_CALL vuVector3FAbs(VuVector3F v);

/// \brief Compute the Euclidian norm of a 3D vector
/**
 * \note result = ||v||
 */
VU_API float VU_API_CALL vuVector3FMag(VuVector3F v);

/// \brief Compute the Squared Euclidian norm of a 3D vector
/**
 * \note result = ||v||^2
 */
VU_API float VU_API_CALL vuVector3FMagSquared(VuVector3F v);

/// \brief Get a normalized 3D vector
/**
 * \note result = v/||v||
 */
VU_API VuVector3F VU_API_CALL vuVector3FNormalize(VuVector3F v);

/// \brief Multiply a 3D vector by a scalar value
/**
 * \note result = v * s
 */
VU_API VuVector3F VU_API_CALL vuVector3FScale(VuVector3F v, float s);

/// \brief Get the sum of two 3D vectors
/**
 * \note result = v1 + v2
 */
VU_API VuVector3F VU_API_CALL vuVector3FAdd(VuVector3F v1, VuVector3F v2);

/// \brief Get the difference of two 3D vectors
/**
 * \note result = v1 - v2
 */
VU_API VuVector3F VU_API_CALL vuVector3FSub(VuVector3F v1, VuVector3F v2);

/// \brief Compute the angle between two 3D vectors in degrees
VU_API float VU_API_CALL vuVector3FAngle(VuVector3F v1, VuVector3F v2);

/// \brief Compute the Euclidean distance between two 3D vectors
VU_API float VU_API_CALL vuVector3FDist(VuVector3F v1, VuVector3F v2);

/// \brief Compute the linear interpolation between two 3D vectors
/**
 * \note result = v1*t+(1-t)*v2
 */
VU_API VuVector3F VU_API_CALL vuVector3FLerp(VuVector3F v1, VuVector3F v2, float t);

/// \brief Compute the dot product of two 3D vectors
/**
 * \note result = v1.v2
 */
VU_API float VU_API_CALL vuVector3FDot(VuVector3F v1, VuVector3F v2);

/// \brief Compute the cross product of two 3D vectors
/**
 * \note result = v1 x v2
 */
VU_API VuVector3F VU_API_CALL vuVector3FCross(VuVector3F v1, VuVector3F v2);

/// \brief Transform a 3D vector by a 4x4 matrix
/**
 * \note result = m * v (pre-multiply)
 */
VU_API VuVector3F VU_API_CALL vuVector3FTransform(VuMatrix44F m, VuVector3F v);

/// \brief Transform a 3D vector by a 4x4 matrix
/**
 * \note result = v * m (post-multiply)
 */
VU_API VuVector3F VU_API_CALL vuVector3FTransformR(VuVector3F v, VuMatrix44F m);

/// \brief Transform a normal by a 4x4 matrix (rotation only)
/**
 * \note result = m * v (pre-multiply)
 */
VU_API VuVector3F VU_API_CALL vuVector3FTransformNormal(VuMatrix44F m, VuVector3F v);

/// \brief Transform a normal by a 4x4 matrix (rotation only)
/**
 * \note result = v * m (post-multiply)
 */
VU_API VuVector3F VU_API_CALL vuVector3FTransformNormalR(VuVector3F v, VuMatrix44F m);


// Vector 4D Math Utilities

/// \brief Get a 4D zero vector (0,0,0,0)
VU_API VuVector4F VU_API_CALL vuZeroVector4F();

/// \brief Get a 4D one vector (1,1,1,1)
VU_API VuVector4F VU_API_CALL vuOneVector4F();

/// \brief Negate a 4D vector
/**
 * \note result = -v
 */
VU_API VuVector4F VU_API_CALL vuVector4FNegate(VuVector4F v);

/// \brief Get the absolute vector of a 4D vector
/**
 * \note result = |v|
 */
VU_API VuVector4F VU_API_CALL vuVector4FAbs(VuVector4F v);

/// \brief Compute the Euclidian norm of a 4D vector
/**
 * \note result = ||v||
 */
VU_API float VU_API_CALL vuVector4FMag(VuVector4F v);

/// \brief Compute the Squared Euclidian norm of a 4D vector
/**
 * \note result = ||v||^2
 */
VU_API float VU_API_CALL vuVector4FMagSquared(VuVector4F v);

/// \brief Get a normalized 4D vector
/**
 * \note result = v/||v||
 */
VU_API VuVector4F VU_API_CALL vuVector4FNormalize(VuVector4F v);

/// \brief Multiply a 4D vector by a scalar value
/**
 * \note result = v * s
 */
VU_API VuVector4F VU_API_CALL vuVector4FScale(VuVector4F v, float s);

/// \brief Get the sum of two 4D vectors
/**
 * \note result = v1 + v2
 */
VU_API VuVector4F VU_API_CALL vuVector4FAdd(VuVector4F v1, VuVector4F v2);

/// \brief Get the difference of two 4D vectors
/**
 * \note result = v1 - v2
 */
VU_API VuVector4F VU_API_CALL vuVector4FSub(VuVector4F v1, VuVector4F v2);

/// \brief Compute the Euclidean distance between two 4D vectors
VU_API float VU_API_CALL vuVector4FDist(VuVector4F v1, VuVector4F v2);

/// \brief Compute the linear interpolation between two 4D vectors
/**
 * \note result = v1*t+(1-t)*v2
 */
VU_API VuVector4F VU_API_CALL vuVector4FLerp(VuVector4F v1, VuVector4F v2, float t);

/// \brief Compute the dot product of two 4D vectors
/**
 * \note result = v1.v2
 */
VU_API float VU_API_CALL vuVector4FDot(VuVector4F v1, VuVector4F v2);

/// \brief Transform a 4D vector by a 4x4 matrix
/**
 * \note result = m * v (pre-multiply)
 */
VU_API VuVector4F VU_API_CALL vuVector4FTransform(VuMatrix44F m, VuVector4F v);

/// \brief Transform a 4D vector by 4x4 matrix and return the result
/**
 * \note result = v * m (post-multiply)
 */
VU_API VuVector4F VU_API_CALL vuVector4FTransformR(VuVector4F v, VuMatrix44F m);


// Vector 8D Math Utilities

/// \brief Get an 8D zero vector (0,0,0,0,0,0,0,0)
VU_API VuVector8F VU_API_CALL vuZeroVector8F();

/// \brief Get an 8D one vector (1,1,1,1,1,1,1,1)
VU_API VuVector8F VU_API_CALL vuOneVector8F();

// Matrix33F Math Utilities

/// \brief Get a 3x3 zero matrix
VU_API VuMatrix33F VU_API_CALL vuZeroMatrix33F();

/// \brief Get a 3x3 identity matrix
VU_API VuMatrix33F VU_API_CALL vuIdentityMatrix33F();


// Matrix44F Math Utilities

/// \brief Get a 4x4 zero matrix
VU_API VuMatrix44F VU_API_CALL vuZeroMatrix44F();

/// \brief Get a 4x4 identity matrix
VU_API VuMatrix44F VU_API_CALL vuIdentityMatrix44F();

/// \brief Check whether the 4x4 matrix is an identity matrix
VU_API VuBool VU_API_CALL vuMatrix44FIsIdentity(VuMatrix44F m);

/// \brief Get the transpose of a 4x4 matrix
VU_API VuMatrix44F VU_API_CALL vuMatrix44FTranspose(VuMatrix44F m);

/// \brief Get the determinant of a 4x4 matrix
VU_API float VU_API_CALL vuMatrix44FDeterminant(VuMatrix44F m);

/// \brief Get the inverse of a 4x4 matrix
VU_API VuMatrix44F VU_API_CALL vuMatrix44FInverse(VuMatrix44F m);

/// \brief Get a rotation matrix specified by axis-angle rotation (angle is in degrees)
VU_API VuMatrix44F VU_API_CALL vuMatrix44FRotationMatrix(float angle, VuVector3F axis);

/// \brief Get a translation matrix specified by a translation vector
VU_API VuMatrix44F VU_API_CALL vuMatrix44FTranslationMatrix(VuVector3F trans);

/// \brief Get a scaling matrix specified by a scale vector
VU_API VuMatrix44F VU_API_CALL vuMatrix44FScalingMatrix(VuVector3F scale);

/// \brief Get a transformation matrix composed of a translation vector, axis-angle rotation, and scaling vector
/**
 * \note result = T(trans) * R(angle, axis) * S(scale)
 */
VU_API VuMatrix44F VU_API_CALL vuMatrix44FTRSMatrix(VuVector3F trans, float angle, VuVector3F axis, VuVector3F scale);

/// \brief Get a transformation matrix composed of a translation vector and rotation (quaternion)
/**
 * \param trans Input translation vector
 * \param quat Input rotation as quaternion in [x y z w] order
 *
 * \return The resulting transformation matrix
 */
VU_API VuMatrix44F VU_API_CALL vuMatrix44FPoseQuatMatrix(VuVector3F trans, VuVector4F quat);

/// \brief Translate a matrix by a vector and return the result
/**
 * \note The resulting matrix is post-multiplied: result = M * T(trans)
 */
VU_API VuMatrix44F VU_API_CALL vuMatrix44FTranslate(VuVector3F trans, VuMatrix44F m);

/// \brief Rotate a matrix by a given axis/angle rotation (angle is in degrees)
/**
 * \note The result is post-multiplied: result = M * R(angle, axis)
 */
VU_API VuMatrix44F VU_API_CALL vuMatrix44FRotate(float angle, VuVector3F axis, VuMatrix44F m);

/// \brief Scale a matrix by a scalig vector
/**
 * \note The result is post-multiplied: result = M * S (scale)
 */
VU_API VuMatrix44F VU_API_CALL vuMatrix44FScale(VuVector3F scale, VuMatrix44F m);

/// \brief Create a perspective projection matrix immediately suitable for rendering in OpenGL (Z axis pointing towards the viewer)
/**
 * \param fovy Field of view half angle in degrees in the y direction
 * \param aspectRatio Aspect ratio that determines the field of view in the x direction
 * \param nearPlane Near clipping plane
 * \param farPlane Far clipping plane
 * \returns Perspective projection matrix
 */
VU_API VuMatrix44F VU_API_CALL vuMatrix44FPerspective(float fovy, float aspectRatio, float nearPlane, float farPlane);

/// \brief Create an orthographic projection matrix immediately suitable for rendering in OpenGL (Z axis pointing towards the viewer)
/**
 * \param left Coordinates for the left vertical clipping planes
 * \param right Coordinates for the right vertical clipping planes
 * \param bottom Coordinates for the bottom horizontal clipping planes
 * \param top Coordinates for the top horizontal clipping planes
 * \param nearPlane Near clipping plane
 * \param farPlane Far clipping plane
 * \returns Orthographic projection matrix
 */
VU_API VuMatrix44F VU_API_CALL vuMatrix44FOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

/// \brief Create a "look-at" view matrix immediately suitable for rendering in OpenGL
/**
 * \param eye Coordinates of the eye point's position
 * \param target Coordinates of the reference point's position indicating the center of the	scene
 * \param up Direction of the up vector
 * \returns View matrix
 */
VU_API VuMatrix44F VU_API_CALL vuMatrix44FLookAt(VuVector3F eye, VuVector3F target, VuVector3F up);

/// \brief Multiply the two 4x4 matrices A and B and return (mA * mB)
VU_API VuMatrix44F VU_API_CALL vuMatrix44FMultiplyMatrix(VuMatrix44F mA, VuMatrix44F mB);

/// \brief Extract translation and rotation (axis-angle; angle is in degrees) from a 4x4 matrix
/**
 * \param m Input matrix
 * \param trans Output translation vector
 * \param axis Output axis of rotation (axis-angle)
 * \param angle Output angle of rotation in degrees (axis-angle)
 */
VU_API void VU_API_CALL vuMatrix44FGetPoseRot(VuMatrix44F m, VuVector3F* trans, VuVector3F* axis, float* angle);

/// \brief Extract translation and rotation (quaternion) from a 4x4 matrix
/**
 * \param m Input matrix
 * \param trans Output translation vector
 * \param quat Output rotation as quaternion in [x y z w] order
 */
VU_API void VU_API_CALL vuMatrix44FGetPoseQuat(VuMatrix44F m, VuVector3F* trans, VuVector4F* quat);

/// \brief Decompose a 4x4 matrix into translation, rotation and non-uniform scale components
/**
 * Decomposes the matrix M such that M = T(trans) * R(angle, axis) * S(scale). M must be decomposable into T, R and S only and therefore
 * must not contain shear/skew components and the bottom row must be of form [0 0 0 w] where w != 0. Since scale decomposition is ambiguous,
 * the function will return a scale vector having at most one negative entry.
 *
 * \param m Input matrix
 * \param trans Output translation vector
 * \param angle Output rotation angle in degrees
 * \param axis Output rotation axis (normalized)
 * \param scale Output vector containing the diagonal entries of a scale matrix S
 *
 * \see vuMatrix44FTRSMatrix
 */
VU_API void VU_API_CALL vuMatrix44FGetTRS(VuMatrix44F m, VuVector3F* trans, VuVector3F* axis, float* angle, VuVector3F* scale);

/// \brief Decompose a 4x4 matrix into translation, rotation and non-uniform scale components
/**
 * Decomposes the matrix M such that M = T(trans) * R(quat) * S(scale). M must be decomposable into T, R and S only and therefore must not
 * contain shear/skew components and the bottom row must be of form [0 0 0 w] where w != 0. Since scale decomposition is ambiguous, the
 * function will return a scale vector having at most one negative entry.
 *
 * \param m Input matrix
 * \param trans Output translation vector
 * \param quat Output rotation as a normalized quaternion in [x y z w] order
 * \param scale Output vector containing the diagonal entries of a scale matrix S
 *
 * \see vuMatrix44FTRSMatrix
 */
VU_API void VU_API_CALL vuMatrix44FGetTRSQuat(VuMatrix44F m, VuVector3F* trans, VuVector4F* quat, VuVector3F* scale);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_MATHUTILS_H_
