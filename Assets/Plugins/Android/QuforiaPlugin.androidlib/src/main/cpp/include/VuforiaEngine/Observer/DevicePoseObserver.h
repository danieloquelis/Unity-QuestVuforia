/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_DEVICEPOSEOBSERVER_H_
#define _VU_DEVICEPOSEOBSERVER_H_

/**
 * \file DevicePoseObserver.h
 * \brief Observer for tracking the device pose
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup DevicePoseObserverGroup Device Pose Feature
 * \{
 * An activated Device Pose observer will always output a reason for an observation with
 * a NO_POSE status in its status info.
 */

/// \brief Configuration error for Device Pose creation
VU_ENUM(VuDevicePoseCreationError)
{
    VU_DEVICE_POSE_CREATION_ERROR_NONE = 0x0,                  ///< No error
    VU_DEVICE_POSE_CREATION_ERROR_INTERNAL = 0x1,              ///< An internal error occurred while creating the observer
    VU_DEVICE_POSE_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2, ///< An error occurred while auto-activating the observer
    VU_DEVICE_POSE_CREATION_ERROR_FEATURE_NOT_SUPPORTED = 0x3  ///< Device tracking functionality is not supported on the current device
};

/// \brief Configuration for creating a Device Pose observer
typedef struct VuDevicePoseConfig
{
    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;

    /// \brief Observer mode
    /**
     * Activate static usage mode of the Device Pose: in this case the pose will be set to identity.
     * You can change that after creation by calling vuDevicePoseObserverSetStaticMode().
     * \note The default value is VU_FALSE.
     *
     * \note When static mode is on, this configures the Device Pose for statically mounted devices,
     * e.g. a mobile device on a tripod looking at an object. In such a scenario, standard Vuforia Fusion tracking will
     * not succeed to initialize for lack of movement. Use the staticMode in this setting to configure the device tracker
     * to return static poses to stabilize tracking e.g. in case of the object being occluded.
     */
    VuBool staticMode;
} VuDevicePoseConfig;

/// \brief Default Device Pose configuration
/**
 * \note Use this function to initialize the VuDevicePoseConfig data structure with default values.
 */
VU_API VuDevicePoseConfig VU_API_CALL vuDevicePoseConfigDefault();

/// \brief Status info for the pose of Device Pose observations
/**
 * Provides further information on the pose status reported as part of VuPoseInfo. The status info is retrieved with \ref
 * vuDevicePoseObservationGetStatusInfo.
 *
 * \note All enum values defined by \ref VuObservationPoseStatus may be reported as part of a Device Pose observation, except \ref
 * VU_OBSERVATION_POSE_STATUS_EXTENDED_TRACKED.
 *
 * \see VuPoseInfo
 * \see VuObservationPoseStatus
 * \see vuObservationHasPoseInfo
 * \see vuObservationGetPoseInfo
 * \see vuStateGetObservationsWithPoseInfo
 * \see vuDevicePoseObservationGetStatusInfo
 */
VU_ENUM(VuDevicePoseObservationStatusInfo)
{
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_NORMAL =
        0x1, ///< Tracking is working normally. Reported for \ref VU_OBSERVATION_POSE_STATUS_TRACKED.
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_NOT_OBSERVED =
        0x2, ///< Device pose is not observed. Reported for \ref VU_OBSERVATION_POSE_STATUS_NO_POSE.
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_UNKNOWN =
        0x3, ///< Unknown reason for \ref VU_OBSERVATION_POSE_STATUS_NO_POSE or \ref VU_OBSERVATION_POSE_STATUS_LIMITED.
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_INITIALIZING =
        0x4, ///< The tracking system is currently initializing. Reported for \ref VU_OBSERVATION_POSE_STATUS_NO_POSE or \ref
             ///< VU_OBSERVATION_POSE_STATUS_LIMITED.
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_RELOCALIZING =
        0x5, ///< The tracking system is currently relocalizing. Reported for \ref VU_OBSERVATION_POSE_STATUS_NO_POSE or \ref
             ///< VU_OBSERVATION_POSE_STATUS_LIMITED.
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_EXCESSIVE_MOTION =
        0x6, ///< The device is moving too fast. Reported for \ref VU_OBSERVATION_POSE_STATUS_LIMITED.
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_INSUFFICIENT_FEATURES =
        0x7, ///< There are insufficient features available in the scene. Reported for \ref VU_OBSERVATION_POSE_STATUS_LIMITED.
    VU_DEVICE_POSE_OBSERVATION_STATUS_INFO_INSUFFICIENT_LIGHT =
        0x8 ///< Not enough light for accurate tracking. Reported for \ref VU_OBSERVATION_POSE_STATUS_LIMITED.
};

/*! \var VU_OBSERVER_DEVICE_POSE_TYPE VU_OBSERVER_DEVICE_POSE_TYPE
 * \brief Type identifier for Device Pose observers
 */
VU_CONST_INT(VU_OBSERVER_DEVICE_POSE_TYPE, 0x8);

/*! \var VU_OBSERVATION_DEVICE_POSE_TYPE VU_OBSERVATION_DEVICE_POSE_TYPE
 * \brief Type identifier for Device Pose observations
 */
VU_CONST_INT(VU_OBSERVATION_DEVICE_POSE_TYPE, 0x8);

/// \brief Create a Device Pose observer
VU_API VuResult VU_API_CALL vuEngineCreateDevicePoseObserver(VuEngine* engine, VuObserver** observer, const VuDevicePoseConfig* config,
                                                             VuDevicePoseCreationError* errorCode);

/// \brief Get all Device Pose observers
VU_API VuResult VU_API_CALL vuEngineGetDevicePoseObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Get all Device Pose observations
VU_API VuResult VU_API_CALL vuStateGetDevicePoseObservations(const VuState* state, VuObservationList* observationList);

/// \brief Get status info associated to the pose status of a Device Pose observation.
/**
 * The status info is intended to be used in combination with \ref VuObservationPoseStatus retrieved via \ref vuObservationGetPoseInfo.
 */
VU_API VuResult VU_API_CALL vuDevicePoseObservationGetStatusInfo(const VuObservation* observation,
                                                                 VuDevicePoseObservationStatusInfo* statusInfo);

/// \brief Set the Device Pose to static
/**
 * Configures the Device Pose for statically mounted devices, e.g. a mobile device on a tripod looking at an object.
 * In such a scenario, standard Vuforia Fusion tracking will not succeed to initialize for lack of movement.
 * This setting configures the device tracker to return static poses to stabilize tracking e.g. in case of the object being occluded.
 *
 * \note Changing the static mode will internally also trigger a device tracking reset and re-initialization. All Device Pose observations
 * are reset to VU_OBSERVATION_POSE_STATUS_NO_POSE and tracking is lost on any targets tracked by extended tracking. Any anchors created
 * during the session are destroyed.
 */
VU_API VuResult VU_API_CALL vuDevicePoseObserverSetStaticMode(VuObserver* observer, VuBool staticModeEnabled);

/// \brief Get if the Device Pose is set to static
VU_API VuResult VU_API_CALL vuDevicePoseObserverGetStaticMode(VuObserver* observer, VuBool* staticModeEnabled);

/// \brief Reset world tracking
/**
 * \note This resets and re-initializes device tracking. All Device Pose observations are reset to
 * VU_OBSERVATION_POSE_STATUS_NO_POSE and tracking is lost on any targets tracked by
 * extended tracking. Any anchors created during the session are destroyed.
 */
VU_API VuResult VU_API_CALL vuEngineResetWorldTracking(VuEngine* engine);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_DEVICEPOSEOBSERVER_H_
