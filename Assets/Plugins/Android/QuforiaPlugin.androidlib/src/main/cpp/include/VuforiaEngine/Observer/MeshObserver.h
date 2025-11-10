/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_MESHOBSERVER_H_
#define _VU_MESHOBSERVER_H_

/**
 * \file MeshObserver.h
 * \brief Observer for the Mesh feature
 */

#include <VuforiaEngine/Core/Geometry.h>
#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup MeshObserverGroup Mesh Feature
 * \{
 * The Mesh Observer enables the retrieval of meshes provided by a specific source, such as an
 * Area Target Capture instance.
 */

/*! \var VU_OBSERVER_MESH_TYPE VU_OBSERVER_MESH_TYPE
 * \brief Type identifier for Mesh observers
 */
VU_CONST_INT(VU_OBSERVER_MESH_TYPE, 0xF);

/*! \var VU_OBSERVATION_MESH_TYPE VU_OBSERVATION_MESH_TYPE
 * \brief Type identifier for Mesh observations
 */
VU_CONST_INT(VU_OBSERVATION_MESH_TYPE, 0xF);

/// \brief Get all Mesh observers
VU_API VuResult VU_API_CALL vuEngineGetMeshObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Get all Mesh observations from the Vuforia state
VU_API VuResult VU_API_CALL vuStateGetMeshObservations(const VuState* state, VuObservationList* list);

/// \brief The Mesh observation block represents a single self-contained mesh
/**
 * Each mesh block holds metadata and the actual mesh data. The metadata consists of a unique ID,
 * a timestamp, and a version number. These fields enable the unique identification of each mesh block
 * and the tracking of potential updates to the mesh block. Both the ID and the version are positive numbers.
 * The IDs are unique within a Vuforia session, they are generated at runtime and are not persistent across
 * Vuforia sessions. Moreover, the IDs are not reused, meaning that a newly added mesh block can never
 * have the same ID as an already removed mesh block. Whenever a mesh block has been updated, its
 * timestamp is adjusted to reflect the time of the change, and its version is increased.
 * The actual data of a mesh block consists of a transformation matrix, the axis aligned bounding box of the mesh,
 * and a pointer to a \ref VuMesh structure that holds the mesh.
 */
typedef struct VuMeshObservationBlock
{
    /// \brief Unique ID of the mesh block
    int32_t id;

    /// \brief Timestamp of the last update to the mesh block
    int64_t timestamp;

    /// \brief Current version of the mesh block, incremented each time it is updated
    int32_t version;

    /// \brief Transformation of the mesh block to the observation coordinate system
    /**
     * The transformation is represented as a 4x4 matrix using the OpenGL convention. The matrix is decomposable into
     * translation, rotation and scale components.
     *
     * To transform the mesh block to world space, combine the observation pose with the
     * transformation of the mesh block as shown below:
     *
     * \code
     * VuPoseInfo poseInfo;
     * vuObservationGetPoseInfo(observation, &poseInfo);
     * VuMatrix44F modelMatrix = vuMatrix44FMultiplyMatrix(poseInfo.pose, block.transform);
     * \endcode
     */
    VuMatrix44F transform;

    /// \brief Axis-aligned bounding box of the observed mesh
    VuAABB bbox;

    /// \brief Mesh data
    VuMesh* mesh;
} VuMeshObservationBlock;

/// \brief List of Mesh observation blocks
typedef struct VuMeshObservationBlockList_ VuMeshObservationBlockList;

/// \brief Get number of elements in a Mesh observation block list
VU_API VuResult VU_API_CALL vuMeshObservationBlockListGetSize(const VuMeshObservationBlockList* list, int32_t* listSize);

/// \brief Get an element in a Mesh observation block list
VU_API VuResult VU_API_CALL vuMeshObservationBlockListGetElement(const VuMeshObservationBlockList* list, int32_t element,
                                                                 VuMeshObservationBlock* block);

/// \brief Status information on the mesh data in the observation
VU_ENUM(VuMeshObservationStatus)
{
    VU_MESH_OBSERVATION_STATUS_NORMAL = 0x1, ///< Mesh is normal
    VU_MESH_OBSERVATION_STATUS_LIMITED =
        0x2 ///< Mesh data is limited. The mesh might contain less detail or pieces of the mesh might be missing.
};

/// \brief Mesh observation info
/**
 * The Mesh observation info always holds a complete list of meshes. It includes meshes that have been
 * updated, recently added, and meshes that have remained unchanged. Consider meshes removed if they were
 * in the list in a previous observation but are no longer reported in the list of the current observation.
 */
typedef struct VuMeshObservationInfo
{
    /// Status of the mesh
    VuMeshObservationStatus meshStatus;

    /// \brief List of mesh blocks
    /**
     * \note The lifetime of the list is bound to the lifetime of the observation.
     */
    const VuMeshObservationBlockList* meshes;
} VuMeshObservationInfo;

/// \brief Get observation info associated with a Mesh observation
VU_API VuResult VU_API_CALL vuMeshObservationGetInfo(const VuObservation* observation, VuMeshObservationInfo* info);

/// \brief Status info for the pose of Mesh observations
/**
 * Provides further information on the pose status reported as part of VuPoseInfo. The status info is retrieved with \ref
 * vuMeshObservationGetStatusInfo.
 *
 * \note All enum values defined by \ref VuObservationPoseStatus may be reported as part of a Mesh observation, except \ref
 * VU_OBSERVATION_POSE_STATUS_TRACKED.
 *
 * \see VuPoseInfo
 * \see VuObservationPoseStatus
 * \see vuObservationHasPoseInfo
 * \see vuObservationGetPoseInfo
 * \see vuStateGetObservationsWithPoseInfo
 * \see vuMeshObservationGetStatusInfo
 */
VU_ENUM(VuMeshObservationStatusInfo)
{
    VU_MESH_OBSERVATION_STATUS_INFO_NORMAL =
        0x1, ///< Tracking working normally. Reported for \ref VU_OBSERVATION_POSE_STATUS_EXTENDED_TRACKED.
    VU_MESH_OBSERVATION_STATUS_INFO_NOT_OBSERVED = 0x2, ///< Mesh is not observed. Reported for \ref #VU_OBSERVATION_POSE_STATUS_NO_POSE.
    VU_MESH_OBSERVATION_STATUS_INFO_RELOCALIZING =
        0x3, ///< The tracking system is currently relocalizing. Reported for \ref VU_OBSERVATION_POSE_STATUS_LIMITED.
    VU_MESH_OBSERVATION_STATUS_INFO_MISSING_DATA = 0x4, ///< Asynchronous loading failed. The Mesh pose is based on incomplete data.
                                                        ///< Tracking quality might decrease or the tracking might be lost.
                                                        ///< Reported for \ref VU_OBSERVATION_POSE_STATUS_LIMITED.
    VU_MESH_OBSERVATION_STATUS_INFO_WRONG_SCALE =
        0x5, ///< The scale of the source observer does not match the physical scale of the
             ///< target. Reported for \ref VU_OBSERVATION_POSE_STATUS_TRACKED, \ref VU_OBSERVATION_POSE_STATUS_EXTENDED_TRACKED or \ref
             ///< VU_OBSERVATION_POSE_STATUS_LIMITED.
};

/// \brief Get status info associated to the pose status of a Mesh observation.
/**
 * The status info is intended to be used in combination with \ref VuObservationPoseStatus retrieved via \ref vuObservationGetPoseInfo.
 */
VU_API VuResult VU_API_CALL vuMeshObservationGetStatusInfo(const VuObservation* observation, VuMeshObservationStatusInfo* statusInfo);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_MESHOBSERVER_H_
