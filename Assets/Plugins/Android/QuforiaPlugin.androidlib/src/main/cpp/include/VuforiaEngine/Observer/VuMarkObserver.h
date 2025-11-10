/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_VUMARKOBSERVER_H_
#define _VU_VUMARKOBSERVER_H_

/**
 * \file VuMarkObserver.h
 * \brief Observer for the VuMark feature
 */

#include <VuforiaEngine/Core/Geometry.h>
#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup VuMarkObserverGroup VuMark Feature
 * \{
 */

/// \brief Configuration error for VuMark creation
VU_ENUM(VuVuMarkCreationError)
{
    VU_VUMARK_CREATION_ERROR_NONE = 0x0,                  ///< No error
    VU_VUMARK_CREATION_ERROR_INTERNAL = 0x1,              ///< An internal error occurred while creating the observer
    VU_VUMARK_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2, ///< Observer auto-activation failed
    VU_VUMARK_CREATION_ERROR_DATABASE_LOAD_ERROR = 0x3,   ///< Database file not found or an error occurred when reading
                                                          ///< data from it (potentially unknown or corrupted file)
    VU_VUMARK_CREATION_ERROR_INVALID_TARGET_NAME = 0x4,   ///< Invalid target name
    VU_VUMARK_CREATION_ERROR_TARGET_NOT_FOUND =
        0x5, ///< Target with the specified name AND matching target type for this observer not found in database
    VU_VUMARK_CREATION_ERROR_INVALID_SCALE = 0x6,        ///< Invalid value passed to the scale parameter
    VU_VUMARK_CREATION_ERROR_DATABASE_XML_REQUIRED = 0x7 ///< The VuMark could not be loaded from the DAT file directly, because the
                                                         ///< database only supports loading from the database XML file. Only VuMark
                                                         ///< databases created for Vuforia 10.15 and newer support loading from the DAT
                                                         ///< file directly. Older databases must still be loaded via the database XML file.
};

/// \brief Configuration for creating a VuMark Observer
typedef struct VuVuMarkConfig
{
    /// \brief Path to database containing the VuMark template
    const char* databasePath;

    /// \brief VuMark template name
    const char* templateName;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;

    /// \brief Scale multiplication factor
    /**
     * \note The default value is 1.0f.
     */
    float scale;

    /// \brief Offset from the origin of the VuMark to the pose reported by an observation, relative to the VuMark's frame of reference
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     * The default value is an identity matrix.
     */
    VuMatrix44F poseOffset;
} VuVuMarkConfig;

/// \brief Default VuMark configuration
/**
 * \note Use this function to initialize the VuVuMarkConfig data structure with default values.
 */
VU_API VuVuMarkConfig VU_API_CALL vuVuMarkConfigDefault();

/// \brief Template info for a VuMark from its respective observation
typedef struct VuVuMarkObservationTemplateInfo
{
    /// \brief Persistent system-wide unique ID associated with the VuMark template
    /**
     * \note The unique ID can't be changed.
     */
    const char* uniqueId;

    /// \brief VuMark template name
    const char* name;

    /// \brief VuMark template user data
    const char* userData;

    /// \brief Size (dimensions) of the VuMark in meters
    VuVector2F size;

    /// \brief Get the position of VuMark's origin
    VuVector2F origin;

    /// \brief Axis-aligned bounding box of the observed VuMark, relative to its frame of reference
    VuAABB bbox;

    /// \brief Pose offset used with VuMark template
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     */
    VuMatrix44F poseOffset;
} VuVuMarkObservationTemplateInfo;

/// \brief Status info for the pose of VuMark observations
/**
 * Provides further information on the pose status reported as part of VuPoseInfo. The status info is retrieved with \ref
 * vuVuMarkObservationGetStatusInfo.
 *
 * \note All enum values defined by \ref VuObservationPoseStatus may be reported as part of a VuMark observation.
 *
 * \see VuPoseInfo
 * \see VuObservationPoseStatus
 * \see vuObservationHasPoseInfo
 * \see vuObservationGetPoseInfo
 * \see vuStateGetObservationsWithPoseInfo
 * \see vuStateGetObservationsWithPoseInfo
 */
VU_ENUM(VuVuMarkObservationStatusInfo)
{
    VU_VUMARK_OBSERVATION_STATUS_INFO_NORMAL = 0x1, ///< Tracking is working normally. Reported for \ref VU_OBSERVATION_POSE_STATUS_TRACKED
                                                    ///< or \ref VU_OBSERVATION_POSE_STATUS_EXTENDED_TRACKED.
    VU_VUMARK_OBSERVATION_STATUS_INFO_NOT_OBSERVED = 0x2, ///< Target is not observed. Reported for \ref VU_OBSERVATION_POSE_STATUS_NO_POSE.
    VU_VUMARK_OBSERVATION_STATUS_INFO_RELOCALIZING =
        0x3 ///< The tracking system is currently relocalizing. Reported for \ref VU_OBSERVATION_POSE_STATUS_LIMITED.
};

/// \brief Status info for VuMark observation
VU_ENUM(VuMarkInstanceIdType)
{
    VU_VUMARK_INSTANCE_ID_BYTE = 0x1,   ///< Generic byte data, stored in little-endian order in the buffer
                                        ///< (e.g. an ID of 0x123456 would appear as { 0x56, 0x34, 0x12 })
    VU_VUMARK_INSTANCE_ID_STRING = 0x2, ///< Printable string data in ASCII
    VU_VUMARK_INSTANCE_ID_NUMERIC = 0x3 ///< Numeric data, not larger than a 64 bit unsigned long long
};

/// \brief Instance info for a VuMark from its respective observation
typedef struct VuVuMarkObservationInstanceInfo
{
    /// \brief The type of data stored by the VuMark instance ID
    VuMarkInstanceIdType dataType;

    /// \brief String buffer, filled with length bytes representing the VuMark instance ID if dataType is VU_VUMARK_INSTANCE_ID_STRING or
    /// VU_VUMARK_INSTANCE_ID_BYTE
    /**
     * \note The buffer is set to an empty string if the VuMark instance ID is of type VU_VUMARK_INSTANCE_ID_NUMERIC
     * \note The lifetime of the buffer is bound to the lifetime of the VuMark instance observation
     * that was used to retrieve this data.
     */
    const char* buffer;

    /// \brief Byte length of the string buffer
    int32_t length;

    /// \brief Numeric representation of the VuMark instance ID if dataType is VU_VUMARK_INSTANCE_ID_NUMERIC
    /**
     * \note The value is set to 0 if the VuMark instance ID is not of type VU_VUMARK_INSTANCE_ID_NUMERIC
     */
    uint64_t numericValue;
} VuVuMarkObservationInstanceInfo;

/// \brief VuMark-specific observation information
typedef struct VuVuMarkObservationInfo
{
    /// \brief A unique runtime ID for a given VuMark instance
    /**
     * The unique ID is assigned when an instance of the VuMark is first detected, and is consistent while
     * the VuMark observer remains activated. When multiple physical instances of the same VuMark appear
     * in the camera frame, then the separately generated VuMarkObservations can be distinguished using
     * this ID.
     *
     * \note The ID is unique within a Vuforia session. It is generated at runtime and is not
     * persistent across Vuforia sessions
     */
    int32_t id;
} VuVuMarkObservationInfo;

/*! \var VU_OBSERVER_VUMARK_TYPE VU_OBSERVER_VUMARK_TYPE
 * \brief Type identifier for VuMark observers
 */
VU_CONST_INT(VU_OBSERVER_VUMARK_TYPE, 0x4);

/*! \var VU_OBSERVATION_VUMARK_TYPE VU_OBSERVATION_VUMARK_TYPE
 * \brief Type identifier for VuMark observations
 */
VU_CONST_INT(VU_OBSERVATION_VUMARK_TYPE, 0x4);

/// \brief Create a VuMark observer from a database
/**
 * \note Note that loading the database may take a significant amount of time, it is therefore recommended that this method is not called on
 * the main/UI thread.
 */
VU_API VuResult VU_API_CALL vuEngineCreateVuMarkObserver(VuEngine* engine, VuObserver** observer, const VuVuMarkConfig* config,
                                                         VuVuMarkCreationError* errorCode);

/// \brief Get all VuMark observers
VU_API VuResult VU_API_CALL vuEngineGetVuMarkObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Get the unique id associated to the template from a VuMark observer
/**
 * \note The lifetime of the returned string is bound to the lifetime of the observer.
 */
VU_API VuResult VU_API_CALL vuVuMarkObserverGetTargetUniqueId(const VuObserver* observer, const char** targetId);

/// \brief Get the name associated to the VuMark template from a VuMark observer
/**
 * \note The lifetime of the returned string is bound to the lifetime of the observer.
 */
VU_API VuResult VU_API_CALL vuVuMarkObserverGetTemplateName(const VuObserver* observer, const char** templateName);

/// \brief Get the axis-aligned bounding box associated to the VuMark template from a VuMark observer, relative to its frame of reference
VU_API VuResult VU_API_CALL vuVuMarkObserverGetTemplateAABB(const VuObserver* observer, VuAABB* bbox);

/// \brief Get the size in meters associated to the VuMark template from a VuMark observer
VU_API VuResult VU_API_CALL vuVuMarkObserverGetTemplateSize(const VuObserver* observer, VuVector2F* size);

/// \brief Re-scale the VuMark template size associated to a VuMark observer
VU_API VuResult VU_API_CALL vuVuMarkObserverSetTemplateScale(VuObserver* observer, float scale);

/// \brief Get the pose transformation offset associated to the VuMark template from a VuMark observer
/**
 * \note The pose offset is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuVuMarkObserverGetTemplatePoseOffset(const VuObserver* observer, VuMatrix44F* poseOffset);

/// \brief Set the pose transformation offset associated to the VuMark template from a VuMark observer
/**
 * \note The pose offset is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuVuMarkObserverSetTemplatePoseOffset(VuObserver* observer, const VuMatrix44F* poseOffset);

/// \brief Get the user data for a VuMark template from a VuMark observer's underlying database
/**
 * \note The lifetime of the returned string is bound to the lifetime of the observer.
 */
VU_API VuResult VU_API_CALL vuVuMarkObserverGetUserData(const VuObserver* observer, const char** userData);

/// \brief Get the position of the origin of a VuMark template from a VuMark observer
VU_API VuResult VU_API_CALL vuVuMarkObserverGetOrigin(const VuObserver* observer, VuVector2F* origin);

/// \brief Get whether tracking should use the VuMark template's background image or not
VU_API VuResult VU_API_CALL vuVuMarkObserverGetTrackingFromRuntimeAppearance(const VuObserver* observer, VuBool* isEnabled);

/// \brief Set whether tracking should use the VuMark template's background image or not
VU_API VuResult VU_API_CALL vuVuMarkObserverSetTrackingFromRuntimeAppearance(VuObserver* observer, VuBool enable);

/// Set the tracking optimization of the target associated to the VuMark observer.
/**
 * This setting modifies the internal target tracking parameters to optimize the tracking quality and robustness.
 *
 * \note Only VU_TRACKING_OPTIMIZATION_DEFAULT and VU_TRACKING_OPTIMIZATION_AR_CONTROLLER
 * are allowed. This function will fail if VU_TRACKING_OPTIMIZATION_LOW_FEATURE_OBJECTS is provided.
 *
 * \note This operation will reset any tracking operation for the VuMark observer.
 * It is recommended to use this function before starting the Vuforia Engine.
 */
VU_API VuResult VU_API_CALL vuVuMarkObserverSetTrackingOptimization(VuObserver* observer, VuTrackingOptimization optimization);

/// Get the tracking optimization of the target associated to the VuMark observer.
VU_API VuResult VU_API_CALL vuVuMarkObserverGetTrackingOptimization(const VuObserver* observer, VuTrackingOptimization* optimization);

/// \brief Get all VuMark observations
VU_API VuResult VU_API_CALL vuStateGetVuMarkObservations(const VuState* state, VuObservationList* observationList);

/// \brief Get status info associated to the pose status of a VuMark observation.
/**
 * The status info is intended to be used in combination with \ref VuObservationPoseStatus retrieved via \ref vuObservationGetPoseInfo.
 */
VU_API VuResult VU_API_CALL vuVuMarkObservationGetStatusInfo(const VuObservation* observation, VuVuMarkObservationStatusInfo* statusInfo);

/// \brief Get VuMark-specific info associated with a VuMark observation
VU_API VuResult VU_API_CALL vuVuMarkObservationGetInfo(const VuObservation* observation, VuVuMarkObservationInfo* info);

/// \brief Get VuMark template info associated with a VuMark observation
VU_API VuResult VU_API_CALL vuVuMarkObservationGetTemplateInfo(const VuObservation* observation,
                                                               VuVuMarkObservationTemplateInfo* templateInfo);

/// \brief Get VuMark instance info associated with a VuMark observation
VU_API VuResult VU_API_CALL vuVuMarkObservationGetInstanceInfo(const VuObservation* observation,
                                                               VuVuMarkObservationInstanceInfo* instanceInfo);

/// \brief Get VuMark instance image info associated with a VuMark observation
/**
 * \note The lifetime of the returned image is bound to the lifetime of the observation.
 */
VU_API VuResult VU_API_CALL vuVuMarkObservationGetInstanceImage(const VuObservation* observation, VuImage** instanceImage);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_VUMARKOBSERVER_H_
