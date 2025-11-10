/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_ANCHOROBSERVER_H_
#define _VU_ANCHOROBSERVER_H_

/**
 * \file AnchorObserver.h
 * \brief Observer to create and manage anchors
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup AnchorObserverGroup Anchor Feature
 * \{
 */

/// \brief Configuration error for anchor creation
VU_ENUM(VuAnchorCreationError)
{
    VU_ANCHOR_CREATION_ERROR_NONE = 0x0,                        ///< No error
    VU_ANCHOR_CREATION_ERROR_INTERNAL = 0x1,                    ///< An internal error occurred while creating the observer
    VU_ANCHOR_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2,       ///< Observer auto-activation failed
    VU_ANCHOR_CREATION_ERROR_INVALID_DEVICE_POSE_OBSERVER = 0x3 ///< Device pose observer is null or invalid
};

/// \brief Configuration for creating an anchor observer
typedef struct VuAnchorObserverConfig
{
    /// \brief Pointer to a device pose observer
    /**
     * \note The Anchor observer will only report observations to the state while there
     * is an active device pose observer.
     */
    VuObserver* devicePoseObserver;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;
} VuAnchorObserverConfig;

/// \brief Default anchor observer configuration
/**
 * \note Use this function to initialize the VuAnchorObserverConfig data structure with default values.
 */
VU_API VuAnchorObserverConfig VU_API_CALL vuAnchorObserverConfigDefault();

/// \brief Anchor list
typedef struct VuAnchorList_ VuAnchorList;

/// \brief Hit-test list
typedef struct VuHitTest_ VuHitTest;

/// \brief Hit-test list
typedef struct VuHitTestList_ VuHitTestList;

/// \brief Info for an anchor observation
typedef struct VuAnchorObservationInfo
{
    /// \brief Anchor ID
    /**
     * \note The ID is a positive number and is unique within a Vuforia session. It is generated at
     * runtime and is not persistent across Vuforia sessions.
     */
    int32_t anchorId;
} VuAnchorObservationInfo;

/// \brief Status info for the pose of anchor observations
/**
 * Provides further information on the pose status reported as part of VuPoseInfo. The status info is retrieved with \ref
 * vuAnchorObservationGetStatusInfo.
 *
 * \note All enum values defined by \ref VuObservationPoseStatus may be reported as part of an Anchor observation, except \ref
 * VU_OBSERVATION_POSE_STATUS_TRACKED.
 *
 * \see VuPoseInfo
 * \see VuObservationPoseStatus
 * \see vuObservationHasPoseInfo
 * \see vuObservationGetPoseInfo
 * \see vuStateGetObservationsWithPoseInfo
 * \see vuAnchorObservationGetStatusInfo
 */
VU_ENUM(VuAnchorObservationStatusInfo)
{
    VU_ANCHOR_OBSERVATION_STATUS_INFO_NORMAL =
        0x1, ///< Tracking is working normally. Reported for \ref VU_OBSERVATION_POSE_STATUS_EXTENDED_TRACKED.
    VU_ANCHOR_OBSERVATION_STATUS_INFO_NOT_OBSERVED = 0x2, ///< Anchor is not observed. Reported for \ref VU_OBSERVATION_POSE_STATUS_NO_POSE.
    VU_ANCHOR_OBSERVATION_STATUS_INFO_RELOCALIZING =
        0x3 ///< The tracking system is currently relocalizing. Reported for \ref VU_OBSERVATION_POSE_STATUS_LIMITED.
};

/// \brief Configuration for creating an anchor with pose
typedef struct VuAnchorCreationConfig
{
    /// \brief Pose matrix representing a pose at which the anchor is created
    /**
     * \note The pose is represented as a pose matrix using the OpenGL convention.
     * The default value is an identity matrix.
     */
    VuMatrix44F pose;
} VuAnchorCreationConfig;

/// \brief Default anchor with pose creation configuration
/**
 * \note Use this function to initialize the VuAnchorCreationConfig data structure with default values.
 */
VU_API VuAnchorCreationConfig VU_API_CALL vuAnchorCreationConfigDefault();

/// \brief Configuration for creatig an anchor with a hit-test result
typedef struct VuAnchorCreationHitTestConfig
{
    /// \brief Hit-test result used for creating the anchor
    VuHitTest* hitTest;
} VuAnchorCreationHitTestConfig;

/// \brief Default anchor from hit test creation configuration
/**
 * \note Use this function to initialize the VuAnchorCreationHitTestConfig data structure with default values.
 */
VU_API VuAnchorCreationHitTestConfig VU_API_CALL vuAnchorCreationHitTestConfigDefault();

/*! \var VU_OBSERVER_ANCHOR_TYPE VU_OBSERVER_ANCHOR_TYPE
 * \brief Type identifier for anchor observers
 */
VU_CONST_INT(VU_OBSERVER_ANCHOR_TYPE, 0x9);

/*! \var VU_OBSERVATION_ANCHOR_TYPE VU_OBSERVATION_ANCHOR_TYPE
 * \brief Type identifier for anchor observations
 */
VU_CONST_INT(VU_OBSERVATION_ANCHOR_TYPE, 0x9);

/// \brief Hints for hit-tests
VU_ENUM(VuHitTestHint)
{
    VU_HIT_TEST_HINT_NONE = 0x1,             ///< No hint
    VU_HIT_TEST_HINT_HORIZONTAL_PLANE = 0x2, ///< Hit-test is performed on a horizontal plane
    VU_HIT_TEST_HINT_VERTICAL_PLANE = 0x3    ///< Hit-test is performed on a vertical plane (not supported yet)
};

/// \brief Configuration for performing a hit-test
/**
 * Hit-test functionality performs a hit test between a ray and a plane in the
 * world as understood by the Vuforia Engine. The ray is cast from the given
 * point on the device's screen (typically from a user tapping the screen).
 * An approximate device height above an assumed infinite ground plane may be
 * used to provide an estimated ground plane intersection when the available
 * data on the environment is limited. A single hit test may generate multiple
 * results if the ray intersects more than one detected plane.
 *
 * Specific behavior of the hit test depends on the active Fusion provider
 * (see vuPlatformControllerGetFusionProviderType()).
 *
 * - When using VU_FUSION_PROVIDER_TYPE_SENSOR_FUSION, \p deviceHeight specifies
 * the device's approximate height above an assumed infinite ground plane
 * on which the hit test result may lie; however it will be ignored if an
 * existing (finite, localized) plane from a previous hit test is found
 * along or near to the ray path. Additionally, Vuforia will not return hit
 * test results when the device pose observer for positional device tracking is
 * initializing, as indicated by its status info (see VuDevicePoseObservationStatusInfo).
 *
 * - When using VU_FUSION_PROVIDER_TYPE_PLATFORM_SENSOR_FUSION, \p deviceHeight is
 * ignored and Vuforia will only return results when the hit test ray
 * intersects actual detected planes in the user's environment.
 *
 * \note A hit test is bound to a specific camera frame retrieved from the Vuforia State.
 */
typedef struct VuHitTestConfig
{
    /// \brief Point in the normalized coordinate space of the camera frame to use as the origin of the ray
    /**
     * \note Top left = (0,0), bottom right = (1,1))
     */
    VuVector2F point;

    /// \brief Camera frame retrieved from the Vuforia State that the hit-test should be performed on
    VuCameraFrame* frame;

    /// \brief A hint about the orientation of the plane in the scene.
    /// Default value is VU_HIT_TEST_HINT_NONE.
    /**
     * \note This hint may be ignored internally by Vuforia if the specified hit test
     * orientation is not yet supported.
     */
    VuHitTestHint hint;

    /// \brief Height of the device center above ground plane in meters.
    /// Default value is 1.4 meters
    /**
     * \note This value may be ignored internally by Vuforia if it already has a good
     * estimate of the relative height of the planar surface. Vuforia will also attempt
     * to refine this height estimate over time as the device moves, so the value here
     * does not need to be exact.
     */
    float deviceHeight;
} VuHitTestConfig;

/// \brief Default hit test configuration
/**
 * \note Use this function to initialize the VuHitTestConfig data structure with default values.
 */
VU_API VuHitTestConfig VU_API_CALL vuHitTestConfigDefault();


// HIT-TEST LIST SUPPORT
/// \brief Create a hit-test list
VU_API VuResult VU_API_CALL vuHitTestListCreate(VuHitTestList** list);

/// \brief Get number of elements in a hit-test list
VU_API VuResult VU_API_CALL vuHitTestListGetSize(const VuHitTestList* list, int32_t* listSize);

/// \brief Get an element in a hit-test list
VU_API VuResult VU_API_CALL vuHitTestListGetElement(const VuHitTestList* list, int32_t element, VuHitTest** hitTest);

/// \brief Destroy a hit-test list
VU_API VuResult VU_API_CALL vuHitTestListDestroy(VuHitTestList* list);

/// \brief Get hit-test pose
/**
 * \note The pose is represented as a pose matrix using the OpenGL convention.
 */
VU_API VuResult VU_API_CALL vuHitTestGetPose(const VuHitTest* hitTest, VuMatrix44F* pose);

/// \brief Create an anchor target observer
VU_API VuResult VU_API_CALL vuEngineCreateAnchorObserver(VuEngine* engine, VuObserver** observer, const VuAnchorObserverConfig* config,
                                                         VuAnchorCreationError* errorCode);

/// \brief Get all anchor target observers
VU_API VuResult VU_API_CALL vuEngineGetAnchorObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Perform a hit-test with an anchor observer
/**
 * \note Hit-test is not supported on Hololens
 *
 * \note The obtained VuHitTest results will only be valid until the next call to vuAnchorObserverHitTest()
 */
VU_API VuResult VU_API_CALL vuAnchorObserverHitTest(VuObserver* observer, const VuHitTestConfig* config, VuHitTestList* hittestList);

/// \brief Create an anchor with an anchor observer
VU_API VuResult VU_API_CALL vuAnchorObserverCreateAnchor(VuObserver* observer, const VuAnchorCreationConfig* config, int32_t* anchorId);

/// \brief Create an anchor from hit-test with an anchor observer
VU_API VuResult VU_API_CALL vuAnchorObserverCreateAnchorWithHitTest(VuObserver* observer, const VuAnchorCreationHitTestConfig* config,
                                                                    int32_t* anchorId);

/// \brief Destroy an anchor with an anchor observer
VU_API VuResult VU_API_CALL vuAnchorObserverDestroyAnchor(VuObserver* observer, int32_t anchorId);

/// \brief Destroy all anchors with an anchor observer
VU_API VuResult VU_API_CALL vuAnchorObserverDestroyAnchors(VuObserver* observer);

/// \brief Return list of all managed anchors from an anchor observer
VU_API VuResult VU_API_CALL vuAnchorObserverGetAnchors(const VuObserver* observer, VuAnchorList* anchorList);

/// \brief Get all anchor observations from the Vuforia State
VU_API VuResult VU_API_CALL vuStateGetAnchorObservations(const VuState* state, VuObservationList* observationList);

/// \brief Get status info associated to the pose status of an anchor observation.
/**
 * The status info is intended to be used in combination with \ref VuObservationPoseStatus retrieved via \ref vuObservationGetPoseInfo.
 */
VU_API VuResult VU_API_CALL vuAnchorObservationGetStatusInfo(const VuObservation* observation, VuAnchorObservationStatusInfo* statusInfo);

/// \brief Get info associated with an anchor observation
VU_API VuResult VU_API_CALL vuAnchorObservationGetInfo(const VuObservation* observation, VuAnchorObservationInfo* anchorInfo);

/// \brief Create anchor list
VU_API VuResult VU_API_CALL vuAnchorListCreate(VuAnchorList** list);

/// \brief Get number of elements in an anchor list
VU_API VuResult VU_API_CALL vuAnchorListGetSize(const VuAnchorList* list, int32_t* listSize);

/// \brief Get an element in an anchor list
VU_API VuResult VU_API_CALL vuAnchorListGetElement(const VuAnchorList* list, int32_t element, int32_t* anchorId);

/// \brief Destroy an anchor list
VU_API VuResult VU_API_CALL vuAnchorListDestroy(VuAnchorList* list);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_ANCHOROBSERVER_H_
