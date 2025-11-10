/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_IMAGETARGETOBSERVER_H_
#define _VU_IMAGETARGETOBSERVER_H_

/**
 * \file ImageTargetObserver.h
 * \brief Observer for the Image Target feature
 */

#include <VuforiaEngine/Core/Geometry.h>
#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup ImageTargetObserverGroup Image Target Feature
 * \{
 */

/// \brief Configuration error for Image Target creation
VU_ENUM(VuImageTargetCreationError)
{
    VU_IMAGE_TARGET_CREATION_ERROR_NONE = 0x0,                  ///< No error
    VU_IMAGE_TARGET_CREATION_ERROR_INTERNAL = 0x1,              ///< An internal error occurred during observer creation
    VU_IMAGE_TARGET_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2, ///< Observer auto-activation failed
    VU_IMAGE_TARGET_CREATION_ERROR_DATABASE_LOAD_ERROR = 0x3,   ///< Could not find database file or read data from it
                                                                ///< (potentially unknown or corrupted file)
    VU_IMAGE_TARGET_CREATION_ERROR_INVALID_TARGET_NAME = 0x4,   ///< Invalid target name
    VU_IMAGE_TARGET_CREATION_ERROR_TARGET_NOT_FOUND =
        0x5, ///< Target with the specified name AND matching target type for this observer not found in database
    VU_IMAGE_TARGET_CREATION_ERROR_INVALID_SCALE = 0x6, ///< Invalid value passed to the scale parameter
    VU_IMAGE_TARGET_CREATION_ERROR_DATABASE_XML_REQUIRED =
        0x7 ///< The Image Target could not be loaded from the DAT file directly, because the database only
            ///< supports loading from the database XML file. Only Image Target databases created for Vuforia 10.15
            ///< and newer support loading from the DAT file directly. Older databases must still be loaded
            ///< via the database XML file.
};

/// \brief Configuration error for Image Target creation with buffer config
VU_ENUM(VuImageTargetBufferCreationError)
{
    VU_IMAGE_TARGET_BUFFER_CREATION_ERROR_NONE = 0x0,                  ///< No error
    VU_IMAGE_TARGET_BUFFER_CREATION_ERROR_INTERNAL = 0x1,              ///< An internal error occurred during observer creation
    VU_IMAGE_TARGET_BUFFER_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2, ///< Observer auto-activation failed
    VU_IMAGE_TARGET_BUFFER_CREATION_ERROR_FEATURE_NOT_SUPPORTED = 0x3, ///< Feature not supported without a Vuforia license
    VU_IMAGE_TARGET_BUFFER_CREATION_ERROR_INVALID_DATA = 0x4,          ///< Invalid pixel data buffer pointer
    VU_IMAGE_TARGET_BUFFER_CREATION_ERROR_INVALID_FORMAT =
        0x5, ///< Invalid pixel format. Supported formats are VU_IMAGE_PIXEL_FORMAT_GRAYSCALE,
             ///< VU_IMAGE_PIXEL_FORMAT_RGB888 and VU_IMAGE_PIXEL_FORMAT_RGBA8888
    VU_IMAGE_TARGET_BUFFER_CREATION_ERROR_INVALID_SIZE = 0x6,         ///< Invalid pixel buffer size
    VU_IMAGE_TARGET_BUFFER_CREATION_ERROR_INVALID_TARGET_WIDTH = 0x7, ///< Invalid target width value
    VU_IMAGE_TARGET_BUFFER_CREATION_ERROR_INVALID_TARGET_NAME = 0x8   ///< Invalid target name
};

/// \brief Configuration error for Image Target creation with file config
VU_ENUM(VuImageTargetFileCreationError)
{
    VU_IMAGE_TARGET_FILE_CREATION_ERROR_NONE = 0x0,                  ///< No error
    VU_IMAGE_TARGET_FILE_CREATION_ERROR_INTERNAL = 0x1,              ///< An internal error occurred while creating the observer
    VU_IMAGE_TARGET_FILE_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2, ///< An error occurred while auto-activating the observer
    VU_IMAGE_TARGET_FILE_CREATION_ERROR_FEATURE_NOT_SUPPORTED = 0x3, ///< Feature not supported without a Vuforia license
    VU_IMAGE_TARGET_FILE_CREATION_ERROR_FILE_LOAD_ERROR = 0x4,       ///< Could not find image file or read data from it
                                                                     ///< (potentially unknown or corrupted file)
    VU_IMAGE_TARGET_FILE_CREATION_ERROR_INVALID_TARGET_WIDTH = 0x5,  ///< Invalid target width value
    VU_IMAGE_TARGET_FILE_CREATION_ERROR_INVALID_TARGET_NAME = 0x6    ///< Invalid target name
};

/// \brief Configuration for Image Target using a database target
typedef struct VuImageTargetConfig
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

    /// \brief Scale multiplication factor
    /**
     * \note The default value is 1.0f.
     */
    float scale;

    /// \brief Offset from the origin of the target to the pose reported by an observation, relative to the target's frame of reference
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     * The default value is an identity matrix.
     */
    VuMatrix44F poseOffset;
} VuImageTargetConfig;

/// \brief Default Image Target configuration
/**
 * \note Use this function to initialize the VuImageTargetConfig data structure with default values.
 */
VU_API VuImageTargetConfig VU_API_CALL vuImageTargetConfigDefault();

/// \brief Configuration for Image Target using a File input
typedef struct VuImageTargetFileConfig
{
    /// \brief Path to the image file
    /**
     * Supported file extensions are "jpg", "jpeg", "png" and "pgm".
     */
    const char* path;

    /// \brief Target name
    const char* targetName;

    /// \brief Target width in meters
    float targetWidth;

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
} VuImageTargetFileConfig;

/// \brief Default Image Target file configuration
/**
 * \note Use this function to initialize the VuImageTargetFileConfig data structure with default values.
 */
VU_API VuImageTargetFileConfig VU_API_CALL vuImageTargetFileConfigDefault();

/// \brief Configuration for creating an Image Target using an image buffer
typedef struct VuImageTargetBufferConfig
{
    /// \brief Pointer to the Image Target buffer
    void* pixelBuffer;

    /// \brief Image Target pixel format
    VuImagePixelFormat bufferFormat;

    /// \brief Size of the buffer
    VuVector2I bufferSize;

    /// \brief Target name
    const char* targetName;

    /// \brief Target width in meters
    float targetWidth;

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
} VuImageTargetBufferConfig;

/// \brief Configuration for an Image Target using a Cloud Image Target observation
/**
 * \note Use this function to initialize the VuImageTargetBufferConfig data structure with default values.
 */
VU_API VuImageTargetBufferConfig VU_API_CALL vuImageTargetBufferConfigDefault();

/// \brief Target info for an Image Target from its respective observation
typedef struct VuImageTargetObservationTargetInfo
{
    /// \brief Persistent system-wide unique ID associated with the Image Target
    /**
     * \note The unique ID can't be changed.
     */
    const char* uniqueId;

    /// \brief Target name
    const char* name;

    /// \brief Size (dimensions) of the Image Target in meters
    VuVector2F size;

    /// \brief Axis-aligned bounding box of the observed target, relative to the target's frame of reference
    VuAABB bbox;

    /// \brief Pose offset used with the Image Target
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     */
    VuMatrix44F poseOffset;
} VuImageTargetObservationTargetInfo;

/// \brief Status info for the pose of Image Target observations
/**
 * Provides further information on the pose status reported as part of VuPoseInfo. The status info is retrieved with \ref
 * vuImageTargetObservationGetStatusInfo.
 *
 * \note All enum values defined by \ref VuObservationPoseStatus may be reported as part of an Image Target observation.
 *
 * \see VuPoseInfo
 * \see VuObservationPoseStatus
 * \see vuObservationHasPoseInfo
 * \see vuObservationGetPoseInfo
 * \see vuStateGetObservationsWithPoseInfo
 * \see vuImageTargetObservationGetStatusInfo
 */
VU_ENUM(VuImageTargetObservationStatusInfo)
{
    VU_IMAGE_TARGET_OBSERVATION_STATUS_INFO_NORMAL =
        0x1, ///< Tracking is working normally. Reported for \ref VU_OBSERVATION_POSE_STATUS_TRACKED or \ref
             ///< VU_OBSERVATION_POSE_STATUS_EXTENDED_TRACKED.
    VU_IMAGE_TARGET_OBSERVATION_STATUS_INFO_NOT_OBSERVED =
        0x2, ///< Target is not observed. Reported for \ref VU_OBSERVATION_POSE_STATUS_NO_POSE.
    VU_IMAGE_TARGET_OBSERVATION_STATUS_INFO_RELOCALIZING =
        0x3 ///< The tracking system is currently relocalizing. Reported for \ref VU_OBSERVATION_POSE_STATUS_LIMITED.
};

/*! \var VU_OBSERVER_IMAGE_TARGET_TYPE VU_OBSERVER_IMAGE_TARGET_TYPE
 * \brief Type identifier for Image Target observers
 */
VU_CONST_INT(VU_OBSERVER_IMAGE_TARGET_TYPE, 0x1);

/*! \var VU_OBSERVATION_IMAGE_TARGET_TYPE VU_OBSERVATION_IMAGE_TARGET_TYPE
 * \brief Type identifier for Image Target observations
 */
VU_CONST_INT(VU_OBSERVATION_IMAGE_TARGET_TYPE, 0x1);

/// \brief Create an Image Target observer from database
/**
 * \note Note that loading the database may take a significant amount of time, it is therefore recommended that this method is not called on
 * the main/UI thread.
 */
VU_API VuResult VU_API_CALL vuEngineCreateImageTargetObserver(VuEngine* engine, VuObserver** observer, const VuImageTargetConfig* config,
                                                              VuImageTargetCreationError* errorCode);

/// \brief Create an Image Target observer from file configuration
/**
 * \note Images with width or height bigger than 2048 pixels are not supported.
 */
VU_API VuResult VU_API_CALL vuEngineCreateImageTargetObserverFromFileConfig(VuEngine* engine, VuObserver** observer,
                                                                            const VuImageTargetFileConfig* config,
                                                                            VuImageTargetFileCreationError* errorCode);

/// \brief Create an Image Target observer from buffer configuration
/**
 * \note Images with width or height bigger than 2048 pixels are not supported.
 */
VU_API VuResult VU_API_CALL vuEngineCreateImageTargetObserverFromBufferConfig(VuEngine* engine, VuObserver** observer,
                                                                              const VuImageTargetBufferConfig* config,
                                                                              VuImageTargetBufferCreationError* errorCode);

/// \brief Get all Image Target observers
VU_API VuResult VU_API_CALL vuEngineGetImageTargetObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Get the unique ID associated to an Image Target from its respective observer
/**
 * \note The lifetime of the returned string is bound to the lifetime of the observer.
 */
VU_API VuResult VU_API_CALL vuImageTargetObserverGetTargetUniqueId(const VuObserver* observer, const char** targetId);

/// \brief Get the name associated to an Image Target from its respective observer
/**
 * \note The lifetime of the returned string is bound to the lifetime of the observer.
 */
VU_API VuResult VU_API_CALL vuImageTargetObserverGetTargetName(const VuObserver* observer, const char** targetName);

/// \brief Get the axis-aligned bounding box associated to an Image Target from its respective observer, relative to the target's frame of
/// reference
VU_API VuResult VU_API_CALL vuImageTargetObserverGetAABB(const VuObserver* observer, VuAABB* bbox);

/// \brief Get the size in meters associated with an Image Target from its respective observer
VU_API VuResult VU_API_CALL vuImageTargetObserverGetTargetSize(const VuObserver* observer, VuVector2F* size);

/// \brief Re-scale the size of an Image Target via its respective observer
VU_API VuResult VU_API_CALL vuImageTargetObserverSetTargetScale(VuObserver* observer, float scale);

/// \brief Get the pose transformation offset associated with an Image Target from its respective observer
/**
 * \note The pose offset is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuImageTargetObserverGetTargetPoseOffset(const VuObserver* observer, VuMatrix44F* poseOffset);

/// \brief Set the pose transformation offset associated with an Image Target via its respective observer
/**
 * \note The pose offset is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuImageTargetObserverSetTargetPoseOffset(VuObserver* observer, const VuMatrix44F* poseOffset);

/// Set the tracking optimization of the target associated to the Image Target observer.
/**
 * This setting modifies the internal target tracking parameters to optimize the tracking quality and robustness.
 *
 * \note Only VU_TRACKING_OPTIMIZATION_DEFAULT and VU_TRACKING_OPTIMIZATION_AR_CONTROLLER
 * are allowed. This function will fail if VU_TRACKING_OPTIMIZATION_LOW_FEATURE_OBJECTS is provided.
 *
 * \note This operation will reset any tracking operation for the Image Target observer.
 * It is recommended to use this function before starting the Vuforia Engine.
 */
VU_API VuResult VU_API_CALL vuImageTargetObserverSetTrackingOptimization(VuObserver* observer, VuTrackingOptimization optimization);

/// Get the tracking optimization of the target associated to the Image Target observer.
VU_API VuResult VU_API_CALL vuImageTargetObserverGetTrackingOptimization(const VuObserver* observer, VuTrackingOptimization* optimization);

/// \brief Set the maximum number of image-based targets tracked at the same time
/**
 * \note This setting tells Vuforia how many targets based on image tracking shall be processed at most
 * at the same time. For instance, if an app never requires tracking more than 2 targets, then this value
 * should be set to 2. This setting controls Image Targets, Cylinder Targets, Multi-Targets and VuMarks.
 * The default value is 1.
 */
VU_API VuResult VU_API_CALL vuEngineSetMaximumSimultaneousTrackedImages(VuEngine* engine, int32_t maxNumberOfTargets);

/// \brief Get the maximum number of image-based targets tracked at the same time
VU_API VuResult VU_API_CALL vuEngineGetMaximumSimultaneousTrackedImages(const VuEngine* engine, int32_t* maxNumberOfTargets);

/// \brief Get all Image Target observations
VU_API VuResult VU_API_CALL vuStateGetImageTargetObservations(const VuState* state, VuObservationList* list);

/// \brief Get target info associated with an Image Target observation
VU_API VuResult VU_API_CALL vuImageTargetObservationGetTargetInfo(const VuObservation* observation,
                                                                  VuImageTargetObservationTargetInfo* targetInfo);

/// \brief Get status info associated to the pose status of an Image Target observation.
/**
 * The status info is intended to be used in combination with \ref VuObservationPoseStatus retrieved via \ref vuObservationGetPoseInfo.
 */
VU_API VuResult VU_API_CALL vuImageTargetObservationGetStatusInfo(const VuObservation* observation,
                                                                  VuImageTargetObservationStatusInfo* statusInfo);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_IMAGETARGETOBSERVER_H_
