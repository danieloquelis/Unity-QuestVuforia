/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_MULTITARGETOBSERVER_H_
#define _VU_MULTITARGETOBSERVER_H_

/**
 * \file MultiTargetObserver.h
 * \brief Observer for the Multi-Target feature
 */

#include <VuforiaEngine/Core/Geometry.h>
#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup MultiTargetObserverGroup Multi-Target Feature
 * \{
 * Multi-Targets are a combination of targets, named "parts". They can be created by loading from database using
 * vuEngineCreateMultiTargetObserver() or by creating them at runtime from a list of parts using
 * vuEngineCreateMultiTargetObserverFromPartsConfig() and providing a VuMultiTargetPartsConfig containing
 * a VuMultiTargetPartConfigList. Parts can also be added or removed from an existing Multi-Target with
 * vuMultiTargetObserverAddPart() and vuMultiTargetObserverRemovePart().
 */

/// \brief Configuration error for Multi-Target creation
VU_ENUM(VuMultiTargetCreationError)
{
    VU_MULTI_TARGET_CREATION_ERROR_NONE = 0x0,                  ///< No error
    VU_MULTI_TARGET_CREATION_ERROR_INTERNAL = 0x1,              ///< An internal error occurred while creating the observer
    VU_MULTI_TARGET_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2, ///< Observer auto-activation failed
    VU_MULTI_TARGET_CREATION_ERROR_DATABASE_LOAD_ERROR = 0x3,   ///< Database file not found or an error occurred when reading data from it
                                                                ///< (potentially unknown or corrupted file)
    VU_MULTI_TARGET_CREATION_ERROR_INVALID_TARGET_NAME = 0x4,   ///< Invalid target name
    VU_MULTI_TARGET_CREATION_ERROR_TARGET_NOT_FOUND =
        0x5, ///< Target with the specified name AND matching target type for this observer not found in database
    VU_MULTI_TARGET_CREATION_ERROR_ASSEMBLY_ERROR = 0x6 ///< An error occurred while trying to set up the parts that form the Multi-Target
};

/// \brief Configuration error when creating a Multi-Target from parts
VU_ENUM(VuMultiTargetPartsCreationError)
{
    VU_MULTI_TARGET_PARTS_CREATION_ERROR_NONE = 0x0,                  ///< No error
    VU_MULTI_TARGET_PARTS_CREATION_ERROR_INTERNAL = 0x1,              ///< An internal error occurred while creating the observer
    VU_MULTI_TARGET_PARTS_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2, ///< Observer auto-activation failed
    VU_MULTI_TARGET_PARTS_CREATION_ERROR_DATABASE_LOAD_ERROR = 0x3, ///< Database file not found or an error occurred when reading data from
                                                                    ///< it (potentially unknown or corrupted file)
    VU_MULTI_TARGET_PARTS_CREATION_ERROR_INVALID_TARGET_NAME = 0x4, ///< Invalid target name
    VU_MULTI_TARGET_PARTS_CREATION_ERROR_ASSEMBLY_ERROR =
        0x5, ///< An error occurred while trying to set up the parts that form the Multi-Target
    VU_MULTI_TARGET_PARTS_CREATION_ERROR_DATABASE_XML_REQUIRED =
        0x6 ///< The Multi Target part could not be loaded from the DAT file directly, because the database only
            ///< supports loading from the database XML file. Only Image Target databases created for Vuforia 10.15
            ///< and newer support loading from the DAT file directly. Older databases must still be loaded
            ///< via the database XML file.
};

/// \brief Configuration for Multi-Target observer creation
typedef struct VuMultiTargetConfig
{
    /// \brief Path to database containing targets
    const char* databasePath;

    /// \brief Target name
    const char* targetName;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;

    /// \brief Offset from the origin of the target to the pose reported by an observation, relative to the target's frame of reference
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     * The default value is an identity matrix.
     */
    VuMatrix44F poseOffset;
} VuMultiTargetConfig;

/// \brief Default Multi-Target configuration
/**
 * \note Use this function to initialize the VuMultiTargetConfig data structure with default values.
 */
VU_API VuMultiTargetConfig VU_API_CALL vuMultiTargetConfigDefault();

/// \brief Configuration for a Multi-Target part
typedef struct VuMultiTargetPartConfig
{
    /// \brief Target name
    /**
     * \note Name of an Image Target from the database that the Multi-Target belongs to.
     */
    const char* targetName;

    /// \brief Pose offset of the part relative to the Multi-Target
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     * The default value is an identity matrix.
     */
    VuMatrix44F poseOffset;
} VuMultiTargetPartConfig;

/// \brief Default Multi-Target part configuration
VU_API VuMultiTargetPartConfig VU_API_CALL vuMultiTargetPartConfigDefault();

/// \brief Multi-Target part config list
typedef struct VuMultiTargetPartConfigList_ VuMultiTargetPartConfigList;

/// \brief Configuration for a new Multi-Target from parts
typedef struct VuMultiTargetPartsConfig
{
    /// \brief Path to database containing targets
    const char* databasePath;

    /// \brief Name of the new Multi-Target
    const char* targetName;

    /// \brief Parts that the new Multi-Target should consist of
    const VuMultiTargetPartConfigList* parts;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;

    /// \brief Offset from the origin of the target to the pose reported by an observation, relative to the target's frame of reference
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     * The default value is an identity matrix.
     */
    VuMatrix44F poseOffset;
} VuMultiTargetPartsConfig;

/// \brief Default Multi-Target parts configuration
VU_API VuMultiTargetPartsConfig VU_API_CALL vuMultiTargetPartsConfigDefault();

/// \brief Target info for Multi-Target
typedef struct VuMultiTargetObservationTargetInfo
{
    /// \brief Target name
    const char* name;

    /// \brief Size (dimensions) of the observed target in meters
    VuVector3F size;

    /// \brief Axis-aligned bounding box of the observed target, relative to the target's frame of reference
    VuAABB bbox;

    /// \brief Pose offset used with the observed target
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     */
    VuMatrix44F poseOffset;
} VuMultiTargetObservationTargetInfo;

/// \brief Status info for the pose of Multi-Target observations
/**
 * Provides further information on the pose status reported as part of VuPoseInfo. The status info is retrieved with \ref
 * vuMultiTargetObservationGetStatusInfo.
 *
 * \note All enum values defined by \ref VuObservationPoseStatus may be reported as part of a Multi-Target observation.
 *
 * \see VuPoseInfo
 * \see VuObservationPoseStatus
 * \see vuObservationHasPoseInfo
 * \see vuObservationGetPoseInfo
 * \see vuStateGetObservationsWithPoseInfo
 * \see vuStateGetObservationsWithPoseInfo
 */
VU_ENUM(VuMultiTargetObservationStatusInfo)
{
    VU_MULTI_TARGET_OBSERVATION_STATUS_INFO_NORMAL =
        0x1, ///< Tracking is working normally. Reported for \ref VU_OBSERVATION_POSE_STATUS_TRACKED or \ref
             ///< VU_OBSERVATION_POSE_STATUS_EXTENDED_TRACKED.
    VU_MULTI_TARGET_OBSERVATION_STATUS_INFO_NOT_OBSERVED =
        0x2, ///< Target is not observed. Reported for \ref VU_OBSERVATION_POSE_STATUS_NO_POSE.
    VU_MULTI_TARGET_OBSERVATION_STATUS_INFO_RELOCALIZING =
        0x3 ///< The tracking system is currently relocalizing. Reported for \ref VU_OBSERVATION_POSE_STATUS_LIMITED.
};

/*! \var VU_OBSERVER_MULTI_TARGET_TYPE VU_OBSERVER_MULTI_TARGET_TYPE
 * \brief Type identifier for Multi-Target observers
 */
VU_CONST_INT(VU_OBSERVER_MULTI_TARGET_TYPE, 0x3);

/*! \var VU_OBSERVATION_MULTI_TARGET_TYPE VU_OBSERVATION_MULTI_TARGET_TYPE
 * \brief Type identifier for Multi-Target observations
 */
VU_CONST_INT(VU_OBSERVATION_MULTI_TARGET_TYPE, 0x3);

/// \brief Multi-Target part
typedef struct VuMultiTargetPart_ VuMultiTargetPart;

/// \brief Multi-Target part list
typedef struct VuMultiTargetPartList_ VuMultiTargetPartList;

/// \brief Create a Multi-Target observer from a database
/**
 * \note Note that loading the database may take a significant amount of time, it is therefore recommended that this method is not called on
 * the main/UI thread.
 */
VU_API VuResult VU_API_CALL vuEngineCreateMultiTargetObserver(VuEngine* engine, VuObserver** observer, const VuMultiTargetConfig* config,
                                                              VuMultiTargetCreationError* errorCode);

/// \brief Create a Multi-Target observer from parts
VU_API VuResult VU_API_CALL vuEngineCreateMultiTargetObserverFromPartsConfig(VuEngine* engine, VuObserver** observer,
                                                                             const VuMultiTargetPartsConfig* config,
                                                                             VuMultiTargetPartsCreationError* errorCode);

/// \brief Get all Multi-Target observers
VU_API VuResult VU_API_CALL vuEngineGetMultiTargetObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Get the unique ID associated to the target from a Multi-Target observer
/**
 * \note The lifetime of the returned string is bound to the lifetime of the observer.
 */
VU_API VuResult VU_API_CALL vuMultiTargetObserverGetTargetUniqueId(const VuObserver* observer, const char** targetId);

/// \brief Get the name associated to the target from a Multi-Target observer
/**
 * \note The lifetime of the returned string is bound to the lifetime of the observer.
 */
VU_API VuResult VU_API_CALL vuMultiTargetObserverGetTargetName(const VuObserver* observer, const char** targetName);

/// \brief Get the size in meters associated to the target from a Multi-Target observer
VU_API VuResult VU_API_CALL vuMultiTargetObserverGetTargetSize(const VuObserver* observer, VuVector3F* size);

/// \brief Get the axis-aligned bounding box associated to the target from a Multi-Target observer, relative to the target's frame of
/// reference
VU_API VuResult VU_API_CALL vuMultiTargetObserverGetAABB(const VuObserver* observer, VuAABB* bbox);

/// \brief Get the pose transformation offset associated to the target from a Multi-Target observer
/**
 * \note The pose offset is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuMultiTargetObserverGetTargetPoseOffset(const VuObserver* observer, VuMatrix44F* poseOffset);

/// \brief Set the pose transformation offset associated to the target from a Multi-Target observer
/**
 * \note The pose offset is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuMultiTargetObserverSetTargetPoseOffset(VuObserver* observer, const VuMatrix44F* poseOffset);

/// Set the tracking optimization of the target associated to the Multi-Target observer.
/**
 * This setting modifies the internal target tracking parameters to optimize the tracking quality and robustness.
 *
 * \note Only VU_TRACKING_OPTIMIZATION_DEFAULT and VU_TRACKING_OPTIMIZATION_AR_CONTROLLER
 * are allowed. This function will fail if VU_TRACKING_OPTIMIZATION_LOW_FEATURE_OBJECTS is provided.
 *
 * \note This operation will reset any tracking operation for the Multi-Target observer.
 * It is recommended to use this function before starting the Vuforia Engine.
 */
VU_API VuResult VU_API_CALL vuMultiTargetObserverSetTrackingOptimization(VuObserver* observer, VuTrackingOptimization optimization);

/// Get the tracking optimization of the target associated to the Multi-Target observer.
VU_API VuResult VU_API_CALL vuMultiTargetObserverGetTrackingOptimization(const VuObserver* observer, VuTrackingOptimization* optimization);

/// \brief Get all Multi-Target observations
VU_API VuResult VU_API_CALL vuStateGetMultiTargetObservations(const VuState* state, VuObservationList* observationList);

/// \brief Get status info associated to the pose status of an Multi-Target observation.
/**
 * The status info is intended to be used in combination with \ref VuObservationPoseStatus retrieved via \ref vuObservationGetPoseInfo.
 */
VU_API VuResult VU_API_CALL vuMultiTargetObservationGetStatusInfo(const VuObservation* observation,
                                                                  VuMultiTargetObservationStatusInfo* statusInfo);

/// \brief Get target info associated with Multi-Target observation
VU_API VuResult VU_API_CALL vuMultiTargetObservationGetTargetInfo(const VuObservation* observation,
                                                                  VuMultiTargetObservationTargetInfo* targetInfo);

/// \brief Create Multi-Target part config list
VU_API VuResult VU_API_CALL vuMultiTargetPartConfigListCreate(VuMultiTargetPartConfigList** list);

/// \brief Get number of elements in a Multi-Target part config list
VU_API VuResult VU_API_CALL vuMultiTargetPartConfigListGetSize(const VuMultiTargetPartConfigList* list, int32_t* listSize);

/// \brief Get an element in a Multi-Target part config list
VU_API VuResult VU_API_CALL vuMultiTargetPartConfigListGetElement(const VuMultiTargetPartConfigList* list, int32_t element,
                                                                  VuMultiTargetPartConfig* config);

/// \brief Append an element to a Multi-Target part config list
VU_API VuResult VU_API_CALL vuMultiTargetPartConfigListAppendElement(VuMultiTargetPartConfigList* list,
                                                                     const VuMultiTargetPartConfig* config);

/// \brief Destroy a Multi-Target part config list
VU_API VuResult VU_API_CALL vuMultiTargetPartConfigListDestroy(VuMultiTargetPartConfigList* list);

/// \brief Get a list of all parts of a Multi-Target
VU_API VuResult VU_API_CALL vuMultiTargetObserverGetParts(const VuObserver* observer, VuMultiTargetPartList* partsList);

/// \brief Add a part to a Multi-Target
VU_API VuResult VU_API_CALL vuMultiTargetObserverAddPart(VuObserver* observer, const VuMultiTargetPartConfig* config);

/// \brief Remove a part from a Multi-Target
VU_API VuResult VU_API_CALL vuMultiTargetObserverRemovePart(VuObserver* observer, const char* targetName);

/// \brief Create a Multi-Target part list
VU_API VuResult VU_API_CALL vuMultiTargetPartListCreate(VuMultiTargetPartList** list);

/// \brief Get number of elements in a Multi-Target part list
VU_API VuResult VU_API_CALL vuMultiTargetPartListGetSize(const VuMultiTargetPartList* list, int32_t* listSize);

/// \brief Get an element in a Multi-Target part list
VU_API VuResult VU_API_CALL vuMultiTargetPartListGetElement(const VuMultiTargetPartList* list, int32_t element,
                                                            const VuMultiTargetPart** part);

/// \brief Destroy a Multi-Target part list
VU_API VuResult VU_API_CALL vuMultiTargetPartListDestroy(VuMultiTargetPartList* list);

/// \brief Get the unique ID associated to a Multi-Target part
/**
 * \note The lifetime of the returned string is bound to the lifetime of the Multi-Target part.
 */
VU_API VuResult VU_API_CALL vuMultiTargetPartGetTargetUniqueId(const VuMultiTargetPart* part, const char** targetId);

/// \brief Get the name associated to a Multi-Target part
/**
 * \note The lifetime of the returned string is bound to the lifetime of the Multi-Target part.
 */
VU_API VuResult VU_API_CALL vuMultiTargetPartGetTargetName(const VuMultiTargetPart* part, const char** targetName);

/// \brief Get the size associated with a Multi-Target part in meters
VU_API VuResult VU_API_CALL vuMultiTargetPartGetTargetSize(const VuMultiTargetPart* part, VuVector2F* size);

/// \brief Get the pose transformation offset associated with a Multi-Target part
/**
 * \note The pose transformation offset is represented as a pose matrix using the OpenGL convention.
 * This pose is relative to the Multi-Target to which the part belongs.
 */
VU_API VuResult VU_API_CALL vuMultiTargetPartGetTargetPoseOffset(const VuMultiTargetPart* part, VuMatrix44F* poseOffset);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_MULTITARGETOBSERVER_H_
