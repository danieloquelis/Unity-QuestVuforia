/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_MODELTARGETOBSERVER_H_
#define _VU_MODELTARGETOBSERVER_H_

/**
 * \file ModelTargetObserver.h
 * \brief Observer for the Model Target feature
 */

#include <VuforiaEngine/Core/Geometry.h>
#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup ModelTargetObserverGroup Model Target Feature
 * \{
 */

/// \brief Configuration error for Model Target creation
VU_ENUM(VuModelTargetCreationError)
{
    VU_MODEL_TARGET_CREATION_ERROR_NONE = 0x0,                  ///< No error
    VU_MODEL_TARGET_CREATION_ERROR_INTERNAL = 0x1,              ///< An internal error occurred while creating the observer
    VU_MODEL_TARGET_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2, ///< Observer auto-activation failed
    VU_MODEL_TARGET_CREATION_ERROR_DATABASE_LOAD_ERROR = 0x3,   ///< Database file not found or an error occurred when reading data from it
                                                                ///< (potentially unknown or corrupted file)
    VU_MODEL_TARGET_CREATION_ERROR_INVALID_TARGET_NAME = 0x4,   ///< Invalid target name
    VU_MODEL_TARGET_CREATION_ERROR_TARGET_NOT_FOUND =
        0x5, ///< Target with the specified name AND matching target type for this observer not found in database
    VU_MODEL_TARGET_CREATION_ERROR_INVALID_SCALE = 0x6, ///< Invalid value passed to the scale parameter
    VU_MODEL_TARGET_CREATION_ERROR_INVALID_GUIDE_VIEW_NAME =
        0x7 ///< Invalid value passed to the parameter indicating the default-active Guide View
};

/// \brief Configuration for creating a Model Target observer
typedef struct VuModelTargetConfig
{
    /// \brief Path to database containing targets
    const char* databasePath;

    /// \brief Target name
    const char* targetName;

    /// \brief Name of the Guide View to be active
    /**
     * Set to NULL to keep the default Guide View defined for this Model Target in the database activated
     *
     * \note Advanced Model Targets do not support Guide Views and creation will fail if the value is not
     * set to NULL.
     */
    const char* activeGuideViewName;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     * \note If the Model Target observer was successfully activated, the active guide view's image is generated as well.
     *
     * \note Model Target observers from different databases cannot be active at the same time. Observer creation will fail if "activate" is
     * set to VU_TRUE while a Model Target observer from another database is active.
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

    /// \brief Enhance detection performance by caching local detection data for the model target
    /**
     * If this option is enabled, then local detection data captured during each tracking session
     * of the target will be cached locally to enhance detection performance in future sessions.
     *
     * If exists, the cached data is loaded into memory when the Model Target observer is created to enhance the observer's detection
     * performance.
     *
     * The cached data is updated when the Model Target observer is deactivated.
     *
     * Calling \ref vuModelTargetObserverReset - e.g. in case of tracking issues - will remove any existing cached data for a target.
     * The caching will automatically restart if the the target is detected again.
     * It is also possible to clear the cached data for all previously tracked Model Target observers to free up disk space. This can be
     * done using the provided \ref vuEngineClearModelTargetObserverDetectionCache function.
     *
     * \note The cached data is stored in the private storage location of the application in a folder
     * named ModelTargetDetectionCache. The data for a specific target can be found in a subfolder
     * named with the target unique ID. This can be acquired using the \ref vuModelTargetObserverGetTargetUniqueId
     * function.
     *
     * \note While this feature primarily targets enhanced detection performance for untrained Model Targets, it
     * is also available for Advanced Model Targets. Note that the effects of the enhanced detection performance
     * might not be as prominent for Advanced Model Targets as for untrained Model Targets.
     *
     * \note The cached data is updated asynchronuously when the Model Target observer is deactivated.
     * An immediate call to \ref vuObserverActivate must await any pending cache updates of the Model Target observer, potentially
     * causing a delay during observer activation.
     *
     * \note If a cache update fails due to insufficient free space, an error message will be logged. Vuforia Engine
     * will not clear the Model Target detection cache automatically and it is up to the user to free up disk space on the device.
     *
     * \note If the tracking optimization of the Model Target observer is set to
     * \ref VU_TRACKING_OPTIMIZATION_LOW_FEATURE_OBJECTS, then the target does not use or store detection
     * data and will not benefit from enabling this option.
     *
     * \note The default value is \ref VU_FALSE.
     */
    VuBool enhanceRuntimeDetection;

} VuModelTargetConfig;

/// \brief Default Model Target configuration
/**
 * \note Use this function to initialize the VuModelTargetConfig data structure with default values.
 */
VU_API VuModelTargetConfig VU_API_CALL vuModelTargetConfigDefault();

/// \brief Configuration for creating a Model Target observer from an in-memory buffer
typedef struct VuModelTargetBufferConfig
{
    /// \brief Pointer to the start of the memory buffer
    /**
     * The buffer must contain the contents of the Model Target database .dat file, using .xml files is not supported. The buffer must be
     * valid for the duration of the observer creation and can be freed once the observer is created.
     */
    const void* buffer;

    /// \brief Size of the memory buffer in bytes
    /**
     * This must correspond to the size of the Model Target database .dat file.
     */
    uint32_t bufferSize;

    /// \brief Target name
    const char* targetName;

    /// \brief Name of the Guide View to be active
    /**
     * Set to NULL to keep the default Guide View defined for this Model Target in the database activated
     *
     * \note Advanced Model Targets do not support Guide Views and creation will fail if the value is not
     * set to NULL.
     */
    const char* activeGuideViewName;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     * \note If the Model Target observer was successfully activated, the active guide view's image is generated as well.
     *
     * \note Model Target observers from different databases cannot be active at the same time. Observer creation will fail if "activate" is
     * set to VU_TRUE while a Model Target observer from another database is active.
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

    /// \brief Enhance detection performance by caching local detection data for the model target
    /**
     * Refer to \ref VuModelTargetConfig::enhanceRuntimeDetection for more details.
     */
    VuBool enhanceRuntimeDetection;

} VuModelTargetBufferConfig;

/// \brief Default Model Target configuration
/**
 * \note Use this function to initialize the VuModelTargetConfig data structure with default values.
 */
VU_API VuModelTargetBufferConfig VU_API_CALL vuModelTargetBufferConfigDefault();

/// \brief Target info for a Model Target from its respective observation
typedef struct VuModelTargetObservationTargetInfo
{
    /// \brief Persistent system-wide unique ID associated with the Model Target
    /**
     * \note The unique ID can't be changed.
     */
    const char* uniqueId;

    /// \brief Target name
    const char* name;

    /// \brief Size (dimensions) of the Model Target in meters
    VuVector3F size;

    /// \brief Axis-aligned bounding box of the observed Model Target, relative to the target's frame of reference
    VuAABB bbox;

    /// \brief Name of the active Guide View
    /**
     * Set to NULL for Advanced Model Targets.
     */
    const char* activeGuideViewName;

    /// \brief Tracking optimization
    VuTrackingOptimization trackingOptimization;

    /// \brief Pose offset used with the Model Target
    /**
     * \note The pose offset is represented as a pose matrix using the OpenGL convention.
     */
    VuMatrix44F poseOffset;

    /// \brief Name of the active Model Target state
    const char* activeStateName;
} VuModelTargetObservationTargetInfo;

/// \brief Status info for the pose of Model Target observations
/**
 * Provides further information on the pose status reported as part of VuPoseInfo. The status info is retrieved with \ref
 * vuModelTargetObservationGetStatusInfo.
 *
 * \note All enum values defined by \ref VuObservationPoseStatus may be reported as part of a Model Target observation.
 *
 * \see VuPoseInfo
 * \see VuObservationPoseStatus
 * \see vuObservationHasPoseInfo
 * \see vuObservationGetPoseInfo
 * \see vuStateGetObservationsWithPoseInfo
 * \see vuModelTargetObservationGetStatusInfo
 */
VU_ENUM(VuModelTargetObservationStatusInfo)
{
    VU_MODEL_TARGET_OBSERVATION_STATUS_INFO_NORMAL =
        0x1, ///< Tracking is working normally. Reported for \ref VU_OBSERVATION_POSE_STATUS_TRACKED or \ref
             ///< VU_OBSERVATION_POSE_STATUS_EXTENDED_TRACKED.
    VU_MODEL_TARGET_OBSERVATION_STATUS_INFO_NOT_OBSERVED =
        0x2, ///< Target is not observed. Reported for \ref VU_OBSERVATION_POSE_STATUS_NO_POSE.
    VU_MODEL_TARGET_OBSERVATION_STATUS_INFO_INITIALIZING =
        0x3, ///< The tracking system is currently initializing. Reported for \ref VU_OBSERVATION_POSE_STATUS_NO_POSE.
    VU_MODEL_TARGET_OBSERVATION_STATUS_INFO_RELOCALIZING =
        0x4, ///< The tracking system is currently relocalizing. Reported for \ref VU_OBSERVATION_POSE_STATUS_LIMITED.
    VU_MODEL_TARGET_OBSERVATION_STATUS_INFO_NO_DETECTION_RECOMMENDING_GUIDANCE =
        0x5, ///< Could not snap to the target. Recommend to show a Guide View overlay. Reported for \ref
             ///< VU_OBSERVATION_POSE_STATUS_NO_POSE.
    VU_MODEL_TARGET_OBSERVATION_STATUS_INFO_WRONG_SCALE =
        0x6 ///< The target scale does not match the physical scale of the object. Reported for \ref VU_OBSERVATION_POSE_STATUS_TRACKED,
            ///< \ref VU_OBSERVATION_POSE_STATUS_EXTENDED_TRACKED or \ref VU_OBSERVATION_POSE_STATUS_LIMITED.
};

/*! \var VU_OBSERVER_MODEL_TARGET_TYPE VU_OBSERVER_MODEL_TARGET_TYPE
 * \brief Type identifier for Model Target observers
 */
VU_CONST_INT(VU_OBSERVER_MODEL_TARGET_TYPE, 0x6);

/*! \var VU_OBSERVATION_MODEL_TARGET_TYPE VU_OBSERVATION_MODEL_TARGET_TYPE
 * \brief Type identifier for Model Target observations
 */
VU_CONST_INT(VU_OBSERVATION_MODEL_TARGET_TYPE, 0x6);

/// \brief Guide View
typedef struct VuGuideView_ VuGuideView;

/// \brief Guide View List
typedef struct VuGuideViewList_ VuGuideViewList;

/// \brief Create a guide view list
VU_API VuResult VU_API_CALL vuGuideViewListCreate(VuGuideViewList** list);

/// \brief Get the number of elements in the guide view list
VU_API VuResult VU_API_CALL vuGuideViewListGetSize(const VuGuideViewList* list, int32_t* listSize);

/// \brief Get the element at the specified index from the guide view list
VU_API VuResult VU_API_CALL vuGuideViewListGetElement(const VuGuideViewList* list, int32_t element, VuGuideView** guideView);

/// \brief Destroy the guide view list
VU_API VuResult VU_API_CALL vuGuideViewListDestroy(VuGuideViewList* list);

/// \brief Model Target State
/**
 * \deprecated This struct has been deprecated. It will be removed in an upcoming %Vuforia release. Please use \ref VuModelTargetStateInfo
 * instead.
 */
typedef struct VuModelTargetState_ VuModelTargetState;

/// \brief Model Target State List
/**
 * \deprecated This struct has been deprecated. It will be removed in an upcoming %Vuforia release. Please use \ref
 * VuModelTargetStateInfoList instead.
 */
typedef struct VuModelTargetStateList_ VuModelTargetStateList;

/// \brief Create a Model Target state list
/**
 * \deprecated This function has been deprecated. It will be removed in an upcoming %Vuforia release. Please use \ref
 * vuModelTargetStateInfoListCreate instead.
 */
VU_API VuResult VU_API_CALL vuModelTargetStateListCreate(VuModelTargetStateList** list);

/// \brief Destroys a Model Target state list
/**
 * \deprecated This function has been deprecated. It will be removed in an upcoming %Vuforia release. Please use \ref
 * vuModelTargetStateInfoListDestroy instead.
 */
VU_API VuResult VU_API_CALL vuModelTargetStateListDestroy(VuModelTargetStateList* list);

/// \brief Get the number of elements in the Model Target state list
/**
 * \deprecated This function has been deprecated. It will be removed in an upcoming %Vuforia release. Please use \ref
 * vuModelTargetStateInfoListGetSize instead.
 */
VU_API VuResult VU_API_CALL vuModelTargetStateListGetSize(const VuModelTargetStateList* list, int32_t* listSize);

/// \brief Get the element at the specified index from the Model Target state list
/**
 * \deprecated This function has been deprecated. It will be removed in an upcoming %Vuforia release. Please use \ref
 * vuModelTargetStateInfoListGetElement instead.
 */
VU_API VuResult VU_API_CALL vuModelTargetStateListGetElement(const VuModelTargetStateList* list, int32_t element,
                                                             const VuModelTargetState** state);

/// \brief Info for a Model Target state
typedef struct VuModelTargetStateInfo
{
    /// \brief The name of the Model Target state
    /**
     * The lifetime of the string is bound to the lifetime of the observer.
     */
    const char* stateName;
} VuModelTargetStateInfo;

/// \brief Model Target State Info List
typedef struct VuModelTargetStateInfoList_ VuModelTargetStateInfoList;

/// \brief Create a Model Target state info list
VU_API VuResult VU_API_CALL vuModelTargetStateInfoListCreate(VuModelTargetStateInfoList** list);

/// \brief Destroys a Model Target state info list
VU_API VuResult VU_API_CALL vuModelTargetStateInfoListDestroy(VuModelTargetStateInfoList* list);

/// \brief Get the number of elements in the Model Target state info list
VU_API VuResult VU_API_CALL vuModelTargetStateInfoListGetSize(const VuModelTargetStateInfoList* list, int32_t* listSize);

/// \brief Get the element at the specified index from the Model Target state info list
VU_API VuResult VU_API_CALL vuModelTargetStateInfoListGetElement(const VuModelTargetStateInfoList* list, int32_t element,
                                                                 VuModelTargetStateInfo* stateInfo);

/// \brief Create a Model Target observer from database
/**
 * \note Note that loading the database may take a significant amount of time, it is therefore recommended that this method is not called on
 * the main/UI thread.
 */
VU_API VuResult VU_API_CALL vuEngineCreateModelTargetObserver(VuEngine* engine, VuObserver** observer, const VuModelTargetConfig* config,
                                                              VuModelTargetCreationError* errorCode);

/// \brief Create a Model Target observer from memory buffer
/**
 * \note Note that loading the database may take a significant amount of time, it is therefore recommended that this method is not called on
 * the main/UI thread.
 */
VU_API VuResult VU_API_CALL vuEngineCreateModelTargetObserverFromBufferConfig(VuEngine* engine, VuObserver** observer,
                                                                              const VuModelTargetBufferConfig* config,
                                                                              VuModelTargetCreationError* errorCode);

/// \brief Get all Model Target observers
VU_API VuResult VU_API_CALL vuEngineGetModelTargetObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Remove all detection data cached by Model Target observers
/**
 * Use this function to free up disk space usage by removing all detection data cached by Model Target observers.
 *
 * \note If there is any Model Target observer in the given Vuforia Engine instance this function will fail.
 *
 * \note Note that clearing the cache may take a significant amount of time, it is therefore recommended that this
 * method is not called on the main/UI thread.
 */
VU_API VuResult VU_API_CALL vuEngineClearModelTargetObserverDetectionCache(VuEngine* engine);

/// \brief Reset tracking of this Model Target observer
/**
 * \note This will stop any ongoing tracking of this Model Target including extended tracking. The tracking will automatically restart if
 * the target is recognized again.
 *
 * \note If enhanced runtime detection is enabled, calling this function will remove any existing cached data for the Model Target.
 * The caching will automatically restart if the Model Target is detected again.
 */
VU_API VuResult VU_API_CALL vuModelTargetObserverReset(VuObserver* observer);

/// \brief Get the unique ID associated to the target from a Model Target observer
/**
 * \note The lifetime of the returned string is bound to the lifetime of the observer.
 */
VU_API VuResult VU_API_CALL vuModelTargetObserverGetTargetUniqueId(const VuObserver* observer, const char** targetId);

/// \brief Get the name associated to the target from a Model Target observer
/**
 * \note The lifetime of the returned string is bound to the lifetime of the observer.
 */
VU_API VuResult VU_API_CALL vuModelTargetObserverGetTargetName(const VuObserver* observer, const char** targetName);

/// \brief Get the size in meters associated to the target from a Model Target observer
VU_API VuResult VU_API_CALL vuModelTargetObserverGetTargetSize(const VuObserver* observer, VuVector3F* size);

/// \brief Re-scale the target size associated to a Model Target observer
VU_API VuResult VU_API_CALL vuModelTargetObserverSetTargetScale(VuObserver* observer, float scale);

/// \brief Get the pose transformation offset associated to the target from a Model Target observer
/**
 * \note The pose offset is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuModelTargetObserverGetTargetPoseOffset(const VuObserver* observer, VuMatrix44F* poseOffset);

/// \brief Set the pose transformation offset associated to the target from a Model Target observer
/**
 * \note The pose offset is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuModelTargetObserverSetTargetPoseOffset(VuObserver* observer, const VuMatrix44F* poseOffset);

/// Set the tracking optimization of the target associated to the Model Target observer.
/**
 * This setting modifies the internal target tracking parameters to optimize the tracking quality and robustness.
 *
 * \note This operation will reset any tracking operation for the Model Target observer.
 * It is recommended to use this function before starting the Vuforia Engine.
 *
 * \note Enhanced runtime detection is incompatible with VU_TRACKING_OPTIMIZATION_LOW_FEATURE_OBJECTS and will be disabled until a different
 * tracking optimization is set.
 */
VU_API VuResult VU_API_CALL vuModelTargetObserverSetTrackingOptimization(VuObserver* observer, VuTrackingOptimization optimization);

/// Get the tracking optimization of the target associated to the Model Target observer.
VU_API VuResult VU_API_CALL vuModelTargetObserverGetTrackingOptimization(const VuObserver* observer, VuTrackingOptimization* optimization);

/// \brief Get the axis-aligned bounding box associated to the target from a Model Target observer, relative to the target's frame of
/// reference
VU_API VuResult VU_API_CALL vuModelTargetObserverGetAABB(const VuObserver* observer, VuAABB* bbox);

/// \brief Get a list of the guide views defined for a Model Target observer
/**
 * Returns all guide views associated with the Model Target observer
 *
 * \note The user has to make sure that observer and list are valid during the duration of the call,
 * otherwise the behavior is undefined.
 *
 * \note Any previous content of the given list will be removed if the operation is successful.
 *  On failure the list will not be modified.
 *
 * \note The content of the list is bound to the lifetime of the observer. Accessing the list elements after the
 * observer has been destroyed results in undefined behavior.
 *
 * \note Getting the list of Guide Views of an Advanced Model Target is not possible and the function will
 * return VU_FAILED.
 *
 * \param observer The Model Target observer
 * \param list The list to fill with the guide views.
 */
VU_API VuResult VU_API_CALL vuModelTargetObserverGetGuideViews(const VuObserver* observer, VuGuideViewList* list);

/// \brief Get the name of the currently active guide view
/**
 * \note The lifetime of the returned string is bound to the lifetime of the observer.
 *
 * \note Getting the active Guide View of an Advanced Model Target is not possible and the function will
 * return VU_FAILED.
 */
VU_API VuResult VU_API_CALL vuModelTargetObserverGetActiveGuideViewName(const VuObserver* observer, const char** name);

/// \brief Set the guide view you want to be active by name
/**
 * \note Setting an active Guide View of an Advanced Model Target is not possible and the function will
 * return VU_FAILED.
 */
VU_API VuResult VU_API_CALL vuModelTargetObserverSetActiveGuideViewName(VuObserver* observer, const char* name);

/// \brief Get if the observed Model Target is advanced
/**
 * Advanced Model Targets can be detected from more than one position without the need for Guide Views.
 * Therefore, Guide-View related API functions return VU_FAILED for Advanced Model Targets. This includes
 * vuModelTargetObserverGetActiveGuideViewName() vuModelTargetObserverSetActiveGuideViewName()
 * and vuModelTargetObserverGetGuideViews(). The guide view of an Advanced Model Target can be retrieved
 * via vuModelTargetObserverGetGuideViewForAdvanced().
 */
VU_API VuBool VU_API_CALL vuModelTargetObserverIsAdvanced(const VuObserver* observer);

/// \brief Get the intrinsic parameters of the camera used to render the Guide View image.
/**
 * \note This function will return VU_FAILED if the Guide View is not active.
 */
VU_API VuResult VU_API_CALL vuGuideViewGetIntrinsics(const VuGuideView* guideView, VuCameraIntrinsics* cameraIntrinsics);

/// \brief Get the Guide View pose with respect to the Model Target
/**
 * Return the pose of the Guide View camera with respect to the Model Target's coordinate system.
 * The pose is represented as a pose matrix using the OpenGL convention.
 * The Guide View pose determines the position and orientation of the device where tracking can be initiated.
 */
VU_API VuResult VU_API_CALL vuGuideViewGetPose(const VuGuideView* guideView, VuMatrix44F* pose);

/// \brief Set the Guide View pose with respect to the Model Target
/**
 * Set the pose of the Guide View camera with respect to the Model Target's coordinate system.
 * The pose is represented as a pose matrix using the OpenGL convention.
 * The Guide View pose determines the position and orientation of the device where tracking can be initiated.
 *
 * \note Calling this function causes the Guide View's image to be outdated and a subsequent call to
 * vuGuideViewGetImage() will return a new image with the updated pose.
 */
VU_API VuResult VU_API_CALL vuGuideViewSetPose(VuGuideView* guideView, const VuMatrix44F* pose);

/// \brief Get the Guide View image
/**
 * The image returned is a simplified representation of the Model Target object at the pose returned by
 * getPose().
 *
 * The image is rendered with the latest available camera intrinsics or default intrinsics if no camera is
 * available. During the lifetime of a Guide View, rendering parameters of the Guide View image such as
 * camera intrinsics, device orientation or the Guide View pose can change. Thus, the representation of the
 * Model Target object is not up-to-date anymore and the Guide View image is marked as 'outdated'. In this
 * case, a subsequent call to vuGuideViewGetImage() will return a new image containing the latest
 * representation of the Model Target object.
 *
 * \note Whether a previously obtained Guide View image is still up-to-date can be checked by calling
 * the vuGuideViewIsImageOutdated() function.
 *
 * \note The image is destroyed if the Guide View is deactivated, or if it has been marked 'outdated' and a
 * subsequent call to this function returns a new VuImage.
 *
 * \note This is a potentially long running operation. Therefore, it is recommended to not call this function
 * from the main/UI thread.
 *
 * \note This function will return VU_FAILED if the Guide View is not active.
 *
 * \note On iOS rendering might fail while the app is in background due to OS limitations. If this is the case this function will return
 * VU_FAILED and the Guide View image remains outdated.
 */
VU_API VuResult VU_API_CALL vuGuideViewGetImage(const VuGuideView* guideView, VuImage** image);

/// \brief Flag that indicates if a previous Guide View image is outdated
/**
 * If the camera intrinsics, device orientation or Guide View pose change, any previously obtained Guide View
 * images will not depict the latest representation of the Model Target object anymore and therefore are
 * marked 'outdated'.
 *
 * If the returned value is VU_TRUE, it is recommended to call vuGuideViewGetImage() again to obtain the
 * latest representation of the Model Target object rendered with the latest camera intrinsics, device
 * orientation and Guide View pose.
 *
 * \note This function fails if the guide view is trained or not active.
 */
VU_API VuResult VU_API_CALL vuGuideViewIsImageOutdated(const VuGuideView* guideView, VuBool* outdated);

/// \brief Get the name of a Guide View
/**
 * \note The lifetime of the returned string is bound to the lifetime of the guide view.
 */
VU_API VuResult VU_API_CALL vuGuideViewGetName(const VuGuideView* guideView, const char** name);

/// \brief Turn on recognition engine for an Advanced (360) Model Target database while extended-tracking a model
/**
 * This setting enables the recognition engine when extended-tracking an existing target.
 * When set to VU_FALSE, the recognition engine is stopped for Advanced (360) databases after
 * a Model Target has been found and is never turned on again automatically. The Model Target observer
 * needs to be deactivated and re-activated in order to turn on recognition again.
 * When set to VU_TRUE, recognition is turned on as soon as an existing target is only extended-tracked.
 * If the recognition engine finds a new target in the image frame, tracking will be switched to the newly
 * identified target, resulting in tracking loss of the prior Model Target.
 * The default value is VU_TRUE.
 *
 * \note This can only be set when Vuforia is not running.
 */
VU_API VuResult VU_API_CALL vuEngineSetModelTargetRecoWhileExtendedTracked(VuEngine* engine, VuBool enable);

/// \brief Get the current setting for recognizing Advanced (360) Model Target databases while extended-tracking a model
VU_API VuResult VU_API_CALL vuEngineGetModelTargetRecoWhileExtendedTracked(VuEngine* engine, VuBool* enabled);

/// \brief Get all Model Target observations
VU_API VuResult VU_API_CALL vuStateGetModelTargetObservations(const VuState* state, VuObservationList* list);

/// \brief Get status info associated to the pose status of a Model Target observation.
/**
 * The status info is intended to be used in combination with \ref VuObservationPoseStatus retrieved via \ref vuObservationGetPoseInfo.
 */
VU_API VuResult VU_API_CALL vuModelTargetObservationGetStatusInfo(const VuObservation* observation,
                                                                  VuModelTargetObservationStatusInfo* statusInfo);

/// \brief Get target info associated with a Model Target observation
VU_API VuResult VU_API_CALL vuModelTargetObservationGetTargetInfo(const VuObservation* observation,
                                                                  VuModelTargetObservationTargetInfo* targetInfo);

/// \brief Describes the Model Target state associated with a Model Target observation
typedef struct VuModelTargetObservationStateInfo
{
    /// \brief Name of the state the observation is based on
    const char* stateName;
} VuModelTargetObservationStateInfo;

/// \brief Get state info associated with a Model Target observation
VU_API VuResult VU_API_CALL vuModelTargetObservationGetStateInfo(const VuObservation* observation,
                                                                 VuModelTargetObservationStateInfo* stateInfo);

/// \brief Set the active Model Target state by name
/**
 *  \param observer The Model Target observer to set the state of
 *  \param stateName The name of the state to activate
 *
 * \note Calling this function causes the Guide View's image to be outdated and a subsequent call to
 * vuGuideViewGetImage() will return a new image with the updated Model Target state.
 */
VU_API VuResult VU_API_CALL vuModelTargetObserverSetActiveStateName(VuObserver* observer, const char* stateName);

/// \brief Get the name of the Model Target's active state
/**
 * \note The lifetime of the returned string is bound to the lifetime of the observer.
 */
VU_API VuResult VU_API_CALL vuModelTargetObserverGetActiveStateName(const VuObserver* observer, const char** stateName);

/// \brief Get a list of all possible states of the Model Target
/**
 * \deprecated This function has been deprecated. It will be removed in an upcoming %Vuforia release. Please use \ref
 * vuModelTargetObserverGetAvailableStateInfos instead.
 */
VU_API VuResult VU_API_CALL vuModelTargetObserverGetAvailableStates(const VuObserver* observer, VuModelTargetStateList* list);

/// \brief Get a list of all state infos of all possible states of the Model Target
/**
 * The order of the available states is consistent with the order during authoring of the Model Target.
 */
VU_API VuResult VU_API_CALL vuModelTargetObserverGetAvailableStateInfos(const VuObserver* observer, VuModelTargetStateInfoList* list);

/// \brief Get the name of the Model Target state
/**
 * \note The lifetime of the returned string is bound to the lifetime of the Model Target state.
 *
 * \deprecated This function has been deprecated. It will be removed in an upcoming %Vuforia release. Please use \ref VuModelTargetStateInfo
 * instead.
 *
 * \param state The Model Target state to get the name from
 * \param name Output parameter for the name of the Model Target state
 */
VU_API VuResult VU_API_CALL vuModelTargetStateGetName(const VuModelTargetState* state, const char** name);

/// \brief Get a Guide View for an Advanced Model Target
/**
 * This function returns a VuGuideView that can be used to display a Guide View image to motivate users
 * to point the camera at the object.
 *
 * \note This Guide View has no name and cannot be activated. It's lifetime is bound to the lifetime of the observer.
 *
 * \note Changing the pose of the Guide View only has an effect on the rendered image but will not influence the
 * detection of the Model Target - this is exclusively determined by the views that are defined for training.
 *
 * \note This function only succeeds for Advanced Model Targets and will fail for untrained Model Targets.
 *
 * \note This function requires the Model Target observer to be active. Otherwise it will fail.
 *
 * \note This is a potentially long running operation. Therefore, it is recommended to not call this function
 * from the main/UI thread.
 */
VU_API VuResult VU_API_CALL vuModelTargetObserverGetGuideViewForAdvanced(const VuObserver* observer, VuGuideView** guideView);

/** \} */

/** \addtogroup MeshObserverGroup Mesh Feature
 * \{
 */

/// \brief Configuration error for Mesh observer creation with Model Target observer
VU_ENUM(VuMeshModelTargetCreationError)
{
    VU_MESH_MODEL_TARGET_CREATION_ERROR_NONE = 0x00,                  ///< No error
    VU_MESH_MODEL_TARGET_CREATION_ERROR_INTERNAL = 0x01,              ///< An internal error occurred while creating the observer
    VU_MESH_MODEL_TARGET_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x02, ///< An error occurred while auto-activating the observer
    VU_MESH_MODEL_TARGET_CREATION_ERROR_INVALID_OBSERVER = 0x03,      ///< Model Target observer is NULL or invalid
    VU_MESH_MODEL_TARGET_CREATION_ERROR_SAME_SOURCE_NOT_SUPPORTED =
        0x05 ///< A mesh observer is already attached to the Model Target observer
};

/// \brief Configuration for creating a Mesh observer associated with a Model Target observer
typedef struct VuMeshModelTargetConfig
{
    /// \brief Pointer to Model Target observer
    /**
     * The Model Target observer is the exclusive source of the Mesh observations that are reported by a
     * Mesh observer created with this configuration. The reported Mesh observations provide the latest 3D
     * geometry of the Model Target at the position of the tracked Model Target.
     */
    VuObserver* modelTargetObserver;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;
} VuMeshModelTargetConfig;

/// \brief Default Mesh observer configuration with a Model Target
/**
 * \note Use this function to initialize the VuMeshModelTargetConfig data structure with default values.
 */
VU_API VuMeshModelTargetConfig VU_API_CALL vuMeshModelTargetConfigDefault();

/// \brief Create a Mesh observer with a Model Target as source
/**
 * The observed mesh is the 3D representation of the Model Target currently observed by the associated
 * source observer. In case the associated source observer has not observed the Model Target, the mesh
 * observer will publish the last known mesh but with an identity pose and pose status
 * VU_OBSERVATION_POSE_STATUS_NO_POSE.
 * Because the Mesh observer is dependent on the existence of the Model Target observer, one has to make
 * sure to destroy the Mesh observer before the associated Model Target observer is destroyed.
 */
VU_API VuResult VU_API_CALL vuEngineCreateMeshObserverFromModelTargetConfig(VuEngine* engine, VuObserver** observer,
                                                                            const VuMeshModelTargetConfig* config,
                                                                            VuMeshModelTargetCreationError* errorCode);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_MODELTARGETOBSERVER_H_
