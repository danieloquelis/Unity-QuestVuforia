/*===============================================================================
Copyright (c) 2024 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_VALIDATIONAREAOBSERVER_H_
#define _VU_VALIDATIONAREAOBSERVER_H_

/**
 * \file ValidationAreaObserver.h
 * \brief Validation Area observer for the StepCheck feature
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup ValidationAreaObserverGroup Step Check Feature
 * \{
 * The Validation Area observer observes different appearances of a distinct area depicted in the provided image data.
 */

/// \brief Configuration error for Validation Area creation with file config
VU_ENUM(VuValidationAreaFileCreationError)
{
    VU_VALIDATION_AREA_FILE_CREATION_ERROR_NONE = 0x0,                           ///< No error
    VU_VALIDATION_AREA_FILE_CREATION_ERROR_INTERNAL = 0x1,                       ///< An internal error occurred while creating the observer
    VU_VALIDATION_AREA_FILE_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2,          ///< An error occurred while auto-activating the observer
    VU_VALIDATION_AREA_FILE_CREATION_ERROR_FEATURE_NOT_SUPPORTED_PLATFORM = 0x3, ///< Feature not supported on the current platform
    VU_VALIDATION_AREA_FILE_CREATION_ERROR_FEATURE_NOT_SUPPORTED_LICENSE =
        0x4,                                                        ///< Feature not supported without an appropriate Vuforia license
    VU_VALIDATION_AREA_FILE_CREATION_ERROR_PLUGIN_LOAD_ERROR = 0x5, ///< Failed to load plugin necessary for this feature
    VU_VALIDATION_AREA_FILE_CREATION_ERROR_INVALID_AREA_NAME = 0x6, ///< Invalid Validation Area name
    VU_VALIDATION_AREA_FILE_CREATION_ERROR_FILE_LOAD_ERROR =
        0x7 ///< Could not find image file or read data from it (potentially unknown or corrupted file)
};

/// \brief Configuration for creating a Validation Area observer using an image file
/**
 * The Validation Area is defined by an opaque mask stored in the alpha channel of the image file.
 */
typedef struct VuValidationAreaFileConfig
{
    /// \brief Pointer to a device pose observer
    /**
     * The Validation Area observer will only report observations with pose to the state while there
     * is an active device pose observer. Set devicePoseObserver to nullptr if poses are not required.
     * Pose status will then constantly be reported as \ref VU_OBSERVATION_POSE_STATUS_NO_POSE.
     *
     * \note On platforms other than iOS, pose status is constantly reported as \ref VU_OBSERVATION_POSE_STATUS_NO_POSE,
     * no matter if devicePoseObserver is set to an active device pose observer or not.
     */
    VuObserver* devicePoseObserver;

    /// \brief Path to the image file. The image stored in the file needs to contain an alpha channel, e.g., specify RGBA pixel data.
    /**
     * Supported file extensions are "png", "webp" and "avif".
     *
     * \note "avif" files are only supported on iOS platform.
     */
    const char* path;

    /// \brief Label for the image
    const char* label;

    /// \brief Validation Area name
    const char* validationAreaName;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;

    /// \brief Offset from the origin of the Validation Area to the pose reported by an observation, relative
    /// to the area's frame of reference
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     * The default value is an identity matrix.
     */
    VuMatrix44F poseOffset;
} VuValidationAreaFileConfig;

/// \brief Default Validation Area from image file configuration
/**
 * \note Use this function to initialize the VuValidationAreaFileConfig data structure with default values.
 */
VU_API VuValidationAreaFileConfig VU_API_CALL vuValidationAreaFileConfigDefault();

/// \brief Create Validation Area observer from file configuration
/**
 * \note Images with width or height bigger than 4096 pixels are not supported.
 */
VU_API VuResult VU_API_CALL vuEngineCreateValidationAreaObserverFromFileConfig(VuEngine* engine, VuObserver** observer,
                                                                               const VuValidationAreaFileConfig* config,
                                                                               VuValidationAreaFileCreationError* errorCode);

/// \brief Configuration error for Validation Area creation with buffer config
VU_ENUM(VuValidationAreaBufferCreationError)
{
    VU_VALIDATION_AREA_BUFFER_CREATION_ERROR_NONE = 0x0,                           ///< No error
    VU_VALIDATION_AREA_BUFFER_CREATION_ERROR_INTERNAL = 0x1,                       ///< An internal error occurred during observer creation
    VU_VALIDATION_AREA_BUFFER_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2,          ///< Observer auto-activation failed
    VU_VALIDATION_AREA_BUFFER_CREATION_ERROR_FEATURE_NOT_SUPPORTED_PLATFORM = 0x3, ///< Feature not supported on the current platform
    VU_VALIDATION_AREA_BUFFER_CREATION_ERROR_FEATURE_NOT_SUPPORTED_LICENSE =
        0x4,                                                          ///< Feature not supported without an appropriate Vuforia license
    VU_VALIDATION_AREA_BUFFER_CREATION_ERROR_PLUGIN_LOAD_ERROR = 0x5, ///< Failed to load plugin necessary for this feature
    VU_VALIDATION_AREA_BUFFER_CREATION_ERROR_INVALID_AREA_NAME = 0x6, ///< Invalid Validation Area name
    VU_VALIDATION_AREA_BUFFER_CREATION_ERROR_INVALID_DATA = 0x7,      ///< Invalid pixel data buffer pointer
    VU_VALIDATION_AREA_BUFFER_CREATION_ERROR_INVALID_SIZE = 0x8       ///< Invalid pixel buffer size
};

/// \brief Configuration for creating a Validation Area observer using an image buffer
/**
 * The Validation Area is defined by an opaque mask stored in the alpha channel of the image buffer.
 */
typedef struct VuValidationAreaBufferConfig
{
    /// \brief Pointer to a device pose observer
    /**
     * The Validation Area observer will only report observations with pose to the state while there
     * is an active device pose observer. Set devicePoseObserver to nullptr if poses are not required.
     * Pose status will then constantly be reported as \ref VU_OBSERVATION_POSE_STATUS_NO_POSE.
     *
     * \note On platforms other than iOS, pose status is constantly reported as \ref VU_OBSERVATION_POSE_STATUS_NO_POSE,
     * no matter if devicePoseObserver is set to an active device pose observer or not.
     */
    VuObserver* devicePoseObserver;

    /// \brief Pointer to the image buffer. The buffer needs to contain RGBA pixel data without padding.
    const void* pixelBuffer;

    /// \brief Array of sizes of each buffer (width, height)
    VuVector2I bufferSize;

    /// \brief Label for the image
    const char* label;

    /// \brief Validation Area name
    const char* validationAreaName;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;

    /// \brief Offset from the origin of the Validation Area to the pose reported by an observation, relative
    /// to the area's frame of reference
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     * The default value is an identity matrix.
     */
    VuMatrix44F poseOffset;
} VuValidationAreaBufferConfig;

/// \brief Configuration for an Validation Area from image buffer
/**
 * \note Use this function to initialize the VuValidationAreaBufferConfig data structure with default values.
 */
VU_API VuValidationAreaBufferConfig VU_API_CALL vuValidationAreaBufferConfigDefault();

/// \brief Create Validation Area observer from buffer configuration
/**
 * \note Images with width or height bigger than 4096 pixels are not supported.
 */
VU_API VuResult VU_API_CALL vuEngineCreateValidationAreaObserverFromBufferConfig(VuEngine* engine, VuObserver** observer,
                                                                                 const VuValidationAreaBufferConfig* config,
                                                                                 VuValidationAreaBufferCreationError* errorCode);

/*! \var VU_OBSERVER_VALIDATION_AREA_TYPE VU_OBSERVER_VALIDATION_AREA_TYPE
 * \brief Type identifier for Validation Area observers
 */
VU_CONST_INT(VU_OBSERVER_VALIDATION_AREA_TYPE, 0x0E);

/*! \var VU_OBSERVATION_VALIDATION_AREA_TYPE VU_OBSERVATION_VALIDATION_AREA_TYPE
 * \brief Type identifier for Validation Area observations
 */
VU_CONST_INT(VU_OBSERVATION_VALIDATION_AREA_TYPE, 0x0E);

/// \brief Validation Area info from its respective observation
typedef struct VuValidationAreaObservationAreaInfo
{
    /// \brief Persistent system-wide unique ID associated with the Validation Area
    /**
     * \note The unique ID can't be changed.
     */
    const char* uniqueId;

    /// \brief Validation Area name
    const char* name;

    /// \brief Size (dimensions) of the Validation Area in meters
    VuVector3F size;

    /// \brief Pose offset used with the Validation Area
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     */
    VuMatrix44F poseOffset;
} VuValidationAreaObservationAreaInfo;

/// \brief Status info for Validation Area observations
/**
 * Provides further information on the pose status reported as part of VuPoseInfo. The status info is retrieved with \ref
 * vuValidationAreaObservationGetStatusInfo.
 *
 * \note All enum values defined by \ref VuObservationPoseStatus may be reported as part of a Validation Area observation.
 *
 * \see VuPoseInfo
 * \see VuObservationPoseStatus
 * \see vuObservationHasPoseInfo
 * \see vuObservationGetPoseInfo
 * \see vuStateGetObservationsWithPoseInfo
 * \see vuStateGetObservationsWithPoseInfo
 */
VU_ENUM(VuValidationAreaObservationStatusInfo)
{
    VU_VALIDATION_AREA_OBSERVATION_STATUS_INFO_NORMAL =
        0x1, ///< Tracking is working normally. Reported for \ref VU_OBSERVATION_POSE_STATUS_EXTENDED_TRACKED.
    VU_VALIDATION_AREA_OBSERVATION_STATUS_INFO_NOT_OBSERVED =
        0x2, ///< Validation Area is not observed. Reported for \ref VU_OBSERVATION_POSE_STATUS_NO_POSE.
    VU_VALIDATION_AREA_OBSERVATION_STATUS_INFO_RELOCALIZING =
        0x3 ///< The tracking system is currently relocalizing. Reported for \ref VU_OBSERVATION_POSE_STATUS_LIMITED.
};

/// \brief Validation status for Validation Area observation
VU_ENUM(VuValidationAreaObservationValidationStatus)
{
    VU_VALIDATION_AREA_OBSERVATION_VALIDATION_STATUS_NORMAL =
        0x1, ///< Validation Area appearance was distinguished, label name associated with the observed appearance is reported
    VU_VALIDATION_AREA_OBSERVATION_VALIDATION_STATUS_NOT_VISIBLE =
        0x3, ///< Validation Area is not visible (outside camera view, too small, slanted viewpoint) and its appearance is not observable
    VU_VALIDATION_AREA_OBSERVATION_VALIDATION_STATUS_UNDECIDABLE =
        0x4 ///< Validation Area appearance could not be distinguished (Validation Area is occluded, image is too blurry, low confidence)
};


/// \brief Validation info of a Validation Area observation
typedef struct VuValidationAreaObservationValidationInfo
{
    /// \brief Validation status
    /**
     * \note The values of labelName and confidence are valid only when validation status is
     * VU_VALIDATION_AREA_OBSERVATION_VALIDATION_STATUS_NORMAL.
     */
    VuValidationAreaObservationValidationStatus validationStatus;

    /// \brief The label name of the observed Validation Area appearance
    /**
     * \note The lifetime of the returned string is bound to the lifetime of the observation.
     */
    const char* labelName;

    /// \brief The confidence of the validation between 0 and 1, higher is better
    float confidence;

    /// \brief Recommended direction in which the viewer should position its viewpoint to observe the Validation Area
    /**
     * The direction is in the coordinate system of the Validation Area, from the Validation Area origin towards the viewer.
     *
     * The direction will always be a unit vector or (0, 0, 0) if no direction is available.
     */
    VuVector3F recommendedViewerDirection;

    /// \brief Angular guidance around recommendedViewerDirection in degrees
    /**
     * This angle around recommendedViewerDirection delimits the set of possible viewpoints.
     *
     * The angle will be 0 if there is no recommendedViewerDirection.
     */
    float recommendedViewerAngle;

    /// \brief Distance guidance for recommendedViewerDirection in meters
    /**
     * This is the recommended viewer distance when observing the Validation Area from recommendedViewerDirection.
     *
     * The distance will be 0 if there is no recommendedViewerDirection.
     */
    float recommendedViewerDistance;

    /// \brief The timestamp of the camera frame the validation was performed on (in nanoseconds)
    int64_t validationTimestamp;

    /// \brief Vertices of the Validation Area bounds in camera image space (normalized coordinates)
    VuVector2F vertices[4];
} VuValidationAreaObservationValidationInfo;

/// \brief Get all Validation Area observers
VU_API VuResult VU_API_CALL vuEngineGetValidationAreaObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Get the unique ID associated to the area from a Validation Area observer
/**
 * \note The lifetime of the returned string is bound to the lifetime of the observer.
 */
VU_API VuResult VU_API_CALL vuValidationAreaObserverGetUniqueId(const VuObserver* observer, const char** areaId);

/// \brief Get the name associated to the area from a Validation Area observer
/**
 * \note The lifetime of the returned string is bound to the lifetime of the observer.
 */
VU_API VuResult VU_API_CALL vuValidationAreaObserverGetAreaName(const VuObserver* observer, const char** areaName);

/// \brief Get the pose transformation offset associated to the area from a Validation Area observer
/**
 * \note The offset is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuValidationAreaObserverGetPoseOffset(const VuObserver* observer, VuMatrix44F* poseOffset);

/// \brief Set the pose transformation offset associated to the area from a Validation Area observer
/**
 * \note The offset is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuValidationAreaObserverSetPoseOffset(VuObserver* observer, const VuMatrix44F* poseOffset);

/// \brief Get area info associated with a Validation Area observation
VU_API VuResult VU_API_CALL vuValidationAreaObservationGetAreaInfo(const VuObservation* observation,
                                                                   VuValidationAreaObservationAreaInfo* areaInfo);

/// \brief Get status info associated to the pose status of a Validation Area observation.
/**
 * The status info is intended to be used in combination with \ref VuObservationPoseStatus retrieved via \ref vuObservationGetPoseInfo.
 */
VU_API VuResult VU_API_CALL vuValidationAreaObservationGetStatusInfo(const VuObservation* observation,
                                                                     VuValidationAreaObservationStatusInfo* statusInfo);

/// \brief Get validation info associated with a Validation Area observation
/**
 * \note Prediction for the Validation Area is typically performed at a lower frequency than the actual camera framerate. Validation info
 * returned by this method is the result of the latest validation attempt.
 */
VU_API VuResult VU_API_CALL vuValidationAreaObservationGetValidationInfo(const VuObservation* observation,
                                                                         VuValidationAreaObservationValidationInfo* validationInfo);

/// \brief Get all Validation Area observations
VU_API VuResult VU_API_CALL vuStateGetValidationAreaObservations(const VuState* state, VuObservationList* list);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_VALIDATIONAREAOBSERVER_H_
