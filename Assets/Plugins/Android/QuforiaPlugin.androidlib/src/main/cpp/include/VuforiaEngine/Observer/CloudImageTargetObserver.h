/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_CLOUDIMAGETARGETOBSERVER_H_
#define _VU_CLOUDIMAGETARGETOBSERVER_H_

/**
 * \file CloudImageTargetObserver.h
 * \brief Observer for the Cloud Image Target feature
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup CloudImageTargetObserverGroup Cloud Image Target Feature
 * \{
 */

/// \brief Configuration error for Cloud Image Target Observer creation
VU_ENUM(VuCloudImageTargetCreationError)
{
    VU_CLOUD_IMAGE_TARGET_CREATION_ERROR_NONE = 0x0,                  ///< No error
    VU_CLOUD_IMAGE_TARGET_CREATION_ERROR_INTERNAL = 0x1,              ///< An internal error occurred while creating the observer
    VU_CLOUD_IMAGE_TARGET_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2, ///< An error occurred while auto-activating the observer
    VU_CLOUD_IMAGE_TARGET_CREATION_ERROR_FEATURE_NOT_SUPPORTED = 0x3, ///< Feature is not supported by the current license
    VU_CLOUD_IMAGE_TARGET_CREATION_ERROR_SERVICE_MISSING_AUTHENTICATION =
        0x4, ///< Missing authentication details (user authentication or authentication secret)
    VU_CLOUD_IMAGE_TARGET_CREATION_ERROR_NO_NETWORK_CONNECTION = 0x5, ///< No network connection
    VU_CLOUD_IMAGE_TARGET_CREATION_ERROR_SERVICE_NOT_AVAILABLE = 0x6  ///< Service is not available
};

/// \brief Cloud Image Target query errors
VU_ENUM(VuCloudImageTargetQueryError)
{
    VU_CLOUD_IMAGE_TARGET_QUERY_ERROR_NONE = 0x0,                   ///< No error
    VU_CLOUD_IMAGE_TARGET_QUERY_ERROR_AUTHORIZATION_FAILED = 0x1,   ///< Credentials are wrong or outdated
    VU_CLOUD_IMAGE_TARGET_QUERY_ERROR_PROJECT_SUSPENDED = 0x2,      ///< The specified project was suspended
    VU_CLOUD_IMAGE_TARGET_QUERY_ERROR_NO_NETWORK_CONNECTION = 0x3,  ///< Device has no network connection
    VU_CLOUD_IMAGE_TARGET_QUERY_ERROR_SERVICE_NOT_AVAILABLE = 0x4,  ///< Server not found, down or overloaded
    VU_CLOUD_IMAGE_TARGET_QUERY_ERROR_BAD_FRAME_QUALITY = 0x5,      ///< Low frame quality has been continuously observed
    VU_CLOUD_IMAGE_TARGET_QUERY_ERROR_UPDATE_SDK = 0x6,             ///< SDK Version outdated
    VU_CLOUD_IMAGE_TARGET_QUERY_ERROR_TIMESTAMP_OUT_OF_RANGE = 0x7, ///< Client/Server clocks too far away
    VU_CLOUD_IMAGE_TARGET_QUERY_ERROR_REQUEST_TIMEOUT = 0x8         ///< No response to network request after timeout
};

/// \brief Configuration for Cloud Image Target observer creation
typedef struct VuCloudImageTargetConfig
{
    /// \brief User name for logging in to the cloud-based image recognition server
    const char* userAuth;

    /// \brief Secret key for logging in to the cloud-based image recognition server
    const char* secretAuth;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;
} VuCloudImageTargetConfig;

/// \brief Default Cloud Image Target observer configuration
/**
 * \note Use this function to initialize the VuCloudImageTargetConfig data structure with default values
 */
VU_API VuCloudImageTargetConfig VU_API_CALL vuCloudImageTargetConfigDefault();

/// \brief Target info for a Cloud Image Target from its respective observation
typedef struct VuCloudImageTargetObservationTargetInfo
{
    /// \brief Target name
    const char* name;

    /// \brief Persistent system-wide unique ID associated to the Cloud Image Target target
    /**
     * \note The unique ID can't be changed.
     */
    const char* uniqueId;

    /// Metadata string associated with the Cloud Image Target
    const char* metadata;

    /// Tracking quality rating for the Cloud Image Target
    /**
     * Integer in the range 0..5 that reflects the expected tracking
     * quality of this target. A low quality rating indicates that tracking may
     * by poor or unstable for the Cloud Image Target.
     */
    int8_t trackingRating;
} VuCloudImageTargetObservationTargetInfo;

/*! \var VU_OBSERVER_CLOUD_IMAGE_TARGET_TYPE VU_OBSERVER_CLOUD_IMAGE_TARGET_TYPE
 * \brief Type identifier for Cloud Image Target observers
 */
VU_CONST_INT(VU_OBSERVER_CLOUD_IMAGE_TARGET_TYPE, 0xA);

/*! \var VU_OBSERVATION_CLOUD_IMAGE_TARGET_TYPE VU_OBSERVATION_CLOUD_IMAGE_TARGET_TYPE
 * \brief Type identifier for Cloud Image Target observations
 */
VU_CONST_INT(VU_OBSERVATION_CLOUD_IMAGE_TARGET_TYPE, 0xA);

/// \brief Cloud Image Target observation handler
typedef void(VU_API_CALL VuCloudImageTargetObservationsHandler)(const VuObservationList* observations, void* clientData);

/// \brief Cloud Image Target query error handler
typedef void(VU_API_CALL VuCloudImageTargetQueryErrorHandler)(VuCloudImageTargetQueryError queryError, void* clientData);

/// \brief Create a Cloud Image Target observer
/**
 * This observer supports the recognition of Image Targets using cloud-based
 * recognition. The creation process requires an active network connection
 * and runs asynchronously. After the observer has been created, activating the
 * observer starts continuous recognition of Image Targets in the camera feed.
 * Search results are reported via a registered observation handler function
 * (see VuCloudImageTargetObservationsHandler), whereas query errors are reported
 * via a registered query error handler function (see VuCloudImageTargetQueryErrorHandler).
 * Both of them are registered by calling vuCloudImageTargetObserverRegisterHandlers().
 * The VuCloudImageTargetObservation reported by the respective handler function
 * represents a known Image Target that the cloud-based image recognition system
 * has detected in the camera feed. You can inspect information about the detected
 * targets by calling the vuCloudImageTargetObservationGetTargetInfo function.
 */
VU_API VuResult VU_API_CALL vuEngineCreateCloudImageTargetObserver(VuEngine* engine, VuObserver** observer,
                                                                   const VuCloudImageTargetConfig* config,
                                                                   VuCloudImageTargetCreationError* errorCode);

/// \brief Get all Cloud Image Target observers
VU_API VuResult VU_API_CALL vuEngineGetCloudImageTargetObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Register handlers for Cloud Image Targets
/**
 * The observation handler is triggered whenever new observations are reported back by the Cloud Image Target
 * observer. The client may choose to call "vuEngineCreateImageTargetObserverFromCloudObservation()"
 * to start tracking the image.
 *
 * The error handler is triggered whenever an error gets reported by the Cloud Image Target observer.
 *
 * \note The handlers are triggered on the thread that pushes a notification about a new Vuforia State
 * and that the client can register for by calling "vuEngineRegisterStateHandler()".
 *
 * \note The list of observations delivered through the observation handler is only valid for the duration of the callback.
 * Storing and accessing the list of it's elements after the callback has finished results in undefined behavior.
 *
 * \note Passing a new handler will overwrite the previous handler. Passing NULL will disable updates for the respective handler.
 *
 * \param observer The Cloud Image Target observer to register the handlers for
 * \param observationHandler The handler for receiving Cloud Image Target observation updates
 * \param errorHandler The handler for receiving Cloud Image Target query-related error updates
 * \param clientData The client data to pass with the observation and error updates
 *
 * \returns VU_SUCCESS on success, VU_FALSE on failure.
 */
VU_API VuResult VU_API_CALL vuCloudImageTargetObserverRegisterHandlers(VuObserver* observer,
                                                                       VuCloudImageTargetObservationsHandler* observationHandler,
                                                                       VuCloudImageTargetQueryErrorHandler* errorHandler, void* clientData);

/// \brief Returns whether the Cloud Image Target observer is currently performing a cloud request
VU_API VuBool VU_API_CALL vuCloudImageTargetObserverIsRequesting(const VuObserver* observer);

/// \brief Get target info associated with a Cloud Image Target observation
VU_API VuResult VU_API_CALL vuCloudImageTargetObservationGetTargetInfo(const VuObservation* observation,
                                                                       VuCloudImageTargetObservationTargetInfo* targetInfo);

/// \brief Configuration error for Image Target creation with Cloud Observation
VU_ENUM(VuImageTargetCloudObservationCreationError)
{
    VU_IMAGE_TARGET_CLOUD_OBSERVATION_ERROR_NONE = 0x0,         ///< No error
    VU_IMAGE_TARGET_CLOUD_OBSERVATION_ERROR_INTERNAL = 0x1,     ///< Internal error
    VU_IMAGE_TARGET_CLOUD_OBSERVATION_ERROR_INVALID_SCALE = 0x2 ///< Invalid value passed to the scale parameter
};

/// \brief Configuration for creating an Image Target observer from a Cloud Image Target observation
typedef struct VuImageTargetCloudObservationConfig
{
    /// \brief Cloud Image Target observation
    VuObservation* observation;

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

    /// \brief Offset from the origin of the target to the pose reported by an observation, relative to the target's frame of reference
    /**
     * \note The pose is represented as a pose matrix using the OpenGL convention.
     * The default value is an identity matrix.
     */
    VuMatrix44F poseOffset;
} VuImageTargetCloudObservationConfig;

/// \brief Default configuration for creating an Image Target observer from a Cloud Image Target observation
/**
 * \note Use this function to initialize the VuImageTargetCloudObservationConfig data structure with default values.
 */
VU_API VuImageTargetCloudObservationConfig VU_API_CALL vuImageTargetCloudObservationConfigDefault();

/// \brief Create an Image Target observer from a Cloud Image Target observation
VU_API VuResult VU_API_CALL vuEngineCreateImageTargetObserverFromCloudObservation(VuEngine* engine, VuObserver** observer,
                                                                                  const VuImageTargetCloudObservationConfig* config,
                                                                                  VuImageTargetCloudObservationCreationError* errorCode);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_CLOUDIMAGETARGETOBSERVER_H_
