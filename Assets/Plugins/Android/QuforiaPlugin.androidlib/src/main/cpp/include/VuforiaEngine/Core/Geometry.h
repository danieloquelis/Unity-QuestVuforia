/*===============================================================================
Copyright (c) 2024 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_GEOMETRY_H_
#define _VU_GEOMETRY_H_

#include <stdint.h> // Required for standard types (int32_t, etc.)

#include <VuforiaEngine/Core/Basic.h>
#include <VuforiaEngine/Core/System.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup GeometryGroup Geometry-related data structures and functions
 * \{
 */

/// \brief Vuforia Mesh
/**
 * A simple mesh representation that holds per-vertex data and face indices.
 * The face indices consist of integer triplets, where each triplet defines a triangle.
 */
typedef struct VuMesh
{
    /// \brief Number of vertices for the mesh
    int32_t numVertices;

    /// \brief Buffer for position coordinates for the mesh
    /**
     * \note Each position consists of three subsequent floats per vertex.
     */
    const float* pos;

    /// \brief Buffer for texture coordinates for the mesh
    /**
     * \note Each texture coordinate consists of two subsequent floats per vertex.
     * This buffer must be set to NULL if the mesh has no texture coordinates.
     */
    const float* tex;

    /// \brief Buffer for normal coordinates for the mesh
    /**
     * \note Each normal consists of three subsequent floats per vertex.
     * This buffer must be set to NULL if the mesh has no normal coordinates.
     */
    const float* normal;

    /// \brief Number of triangle primitives for the mesh
    int32_t numFaces;

    /// \brief Buffer for face indices for the mesh
    const uint32_t* faceIndices;
} VuMesh;


/// \brief Axis-aligned bounding box
typedef struct VuAABB
{
    /// \brief Center of bounding box
    VuVector3F center;

    /// \brief Half-extent of bounding box (from center point to corner point)
    VuVector3F extent;
} VuAABB;

/// \brief Get minimum value from axis-aligned bounding box
VU_API VuVector3F VU_API_CALL vuAABBMin(const VuAABB* aabb);

/// \brief Get maximum value from axis-aligned bounding box
VU_API VuVector3F VU_API_CALL vuAABBMax(const VuAABB* aabb);

/// \brief Coordinates of a 2D rectangle
typedef struct VuRectangle
{
    /// \brief Coordinates of the rectangle's top-left corner
    VuVector2F topLeftCorner;

    /// \brief Coordinates of the rectangle's bottom-right corner
    VuVector2F bottomRightCorner;
} VuRectangle;

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_GEOMETRY_H_
