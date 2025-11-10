/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_AREATARGETCAPTURECONTROLLER_H_
#define _VU_AREATARGETCAPTURECONTROLLER_H_

/**
 * \file AreaTargetCaptureController.h
 * \brief Controller to access the Area Target Capture functionality in the Vuforia Engine
 */

#include <VuforiaEngine/Engine/Engine.h>
#include <VuforiaEngine/Observer/MeshObserver.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup AreaTargetCaptureGroup Area Target Capture Controller
 * \{
 * This controller provides functionality to generate Area Targets by capturing all required data with Vuforia Engine.
 * \note This feature is supported only on iOS devices with a LiDAR sensor.
 */

/// \brief Area Target Capture creation error
VU_ENUM(VuAreaTargetCaptureCreationError)
{
    VU_AREA_TARGET_CAPTURE_CREATION_ERROR_NONE = 0x0,                  ///< No error
    VU_AREA_TARGET_CAPTURE_CREATION_ERROR_INTERNAL = 0x1,              ///< An internal error occurred while creating the capture
    VU_AREA_TARGET_CAPTURE_CREATION_ERROR_AUTOSTART_FAILED = 0x2,      ///< Auto-start of the capture failed
    VU_AREA_TARGET_CAPTURE_CREATION_ERROR_FEATURE_NOT_SUPPORTED = 0x3, ///< Feature is not supported on the current device
    VU_AREA_TARGET_CAPTURE_CREATION_ERROR_MULTIPLE_INSTANCES_NOT_SUPPORTED =
        0x4, ///< Multiple captures are not allowed to exist at the same time
    VU_AREA_TARGET_CAPTURE_CREATION_ERROR_INVALID_DEVICE_POSE_OBSERVER = 0x5, ///< Device pose observer is NULL or invalid
};

/// \brief Area Target Capture status
VU_ENUM(VuAreaTargetCaptureStatus)
{
    VU_AREA_TARGET_CAPTURE_STATUS_INITIALIZED = 0x1, ///< The capture was created and can be started
    VU_AREA_TARGET_CAPTURE_STATUS_PREPARING = 0x2,   ///< The capture has not collected sufficient data to create the initial reconstruction
                                                     ///< of the environment. A target cannot be generated yet
    VU_AREA_TARGET_CAPTURE_STATUS_CAPTURING = 0x3,   ///< The capture created an initial reconstruction of the environment. A target can now
                                                     ///< be generated, or continue to capture more data
    VU_AREA_TARGET_CAPTURE_STATUS_PAUSED = 0x4,      ///< The capture was paused
    VU_AREA_TARGET_CAPTURE_STATUS_STOPPED = 0x5,     ///< The capture was stopped
    VU_AREA_TARGET_CAPTURE_STATUS_GENERATING = 0x6,  ///< The capture is generating a target from the captured data
};

/// \brief Area Target Capture status info
VU_ENUM(VuAreaTargetCaptureStatusInfo)
{
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_NORMAL = 0x1,       ///< The capture is running normally
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_RELOCALIZING = 0x2, ///< The capture is relocalizing, the user should move around or return towards
                                                           ///< a previously mapped area in order to resume normal capturing
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_EXCESSIVE_MOTION = 0x3, ///< The user is moving too quickly
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_CAPACITY_WARNING =
        0x4, ///< The capture is removing old data to make room for new data and should be stopped soon
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_INTERRUPTED = 0x5, ///< The capture is unable to add new data, the user should stop the capture
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_SUSPENDED = 0x6,   ///< The capture is suspended until Engine is started
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_TRACKING_DATA_GENERATION = 0x7,  ///< The capture is processing the captured data
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_AUTHORING_DATA_GENERATION = 0x8, ///< The capture is generating the authoring files
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_DEVICE_DATABASE_GENERATION =
        0x9, ///< The capture is generating the Area Target database and the occlusion mesh
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_PACKAGE_GENERATION = 0xA,        ///< The capture is generating the package(s)
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_GENERATION_SUCCESS = 0xB,        ///< Generation successful
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_GENERATION_CANCELED = 0xC,       ///< Generation was canceled
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_GENERATION_ERROR_INTERNAL = 0xD, ///< Generation failed because an internal error occurred
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_GENERATION_ERROR_NO_NETWORK_CONNECTION =
        0xE, ///< Generation failed because the device has no network connection
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_GENERATION_ERROR_SERVICE_NOT_AVAILABLE =
        0xF, ///< Generation failed because the server was not found, is unreachable, or overloaded
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_GENERATION_ERROR_AUTHORIZATION_FAILED =
        0x10, ///< Generation failed because the credentials are wrong or outdated
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_GENERATION_ERROR_MISSING_ALIGNMENT_DATA =
        0x11, ///< Generation failed because of a problem during asynchronous loading of the alignment target data
    VU_AREA_TARGET_CAPTURE_STATUS_INFO_GENERATION_ERROR_ALIGNMENT_FAILED =
        0x12, ///< Generation failed because it was not possible to align the capture with the specified alignment target
};

/// \brief Area Target Capture generation error
/**
 * \note These errors report the cause for Area Target generation failing to **start**.
 * Errors that arise **during** Area Target generation are reported in \ref VuAreaTargetCaptureStatusInfo.
 */
VU_ENUM(VuAreaTargetCaptureGenerationError)
{
    VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_NONE = 0x0,               ///< No error
    VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_INTERNAL = 0x1,           ///< An internal error occurred
    VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_ENGINE_NOT_RUNNING = 0x2, ///< Generation failed because Engine is not running
    VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_INVALID_STATUS = 0x3,     ///< Generation failed because the capture is in the wrong status
    VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_INSUFFICIENT_DATA =
        0x4, ///< Generation failed because the capture has not yet created an initial reconstruction of the environment
    VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_MISSING_AUTHENTICATION =
        0x5, ///< Generation failed because authentication details are missing (user authentication or authentication secret)
    VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_INVALID_OUTPUT_DIRECTORY =
        0x6, ///< Generation failed because the output directory does not exist or can not be written to
    VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_INVALID_TARGET_NAME =
        0x7, ///< Generation failed because the target name does not satisfy requirements
    VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_AUTHORING_FILES_GENERATION_REQUIRED =
        0x8, ///< Generation failed because authoring files generation is required, but was not enabled
    VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_DATABASE_GENERATION_REQUIRED =
        0x9, ///< Generation failed because database generation is required, but was not enabled
    VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_ALIGNMENT_DATABASE_LOAD_ERROR =
        0xA, ///< Generation failed because the alignment database file was not found or an error occurred when reading data from it
             ///< (potentially unknown or corrupted file)
    VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_INVALID_ALIGNMENT_TARGET_NAME =
        0xB, ///< Generation failed because the alignment target name is invalid
    VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_ALIGNMENT_TARGET_NOT_FOUND =
        0xC, ///< Generation failed because the alignment target with the specified name AND target type matching Area Target was not found
             ///< in database
};

/// \brief Area Target Capture instance
typedef struct VuAreaTargetCapture_ VuAreaTargetCapture;

/// \brief Configuration options for Area Target Capture instance creation
typedef struct VuAreaTargetCaptureConfig
{
    /// \brief Device pose observer
    /**
     * \note The Area Target Capture instance can only capture data while the device pose observer is active.
     */
    VuObserver* devicePoseObserver;

    /// \brief Set to VU_TRUE to immediately start the capture after creation
    /**
     * \note Default value is VU_FALSE
     */
    VuBool start;
} VuAreaTargetCaptureConfig;

/// \brief Default capture configuration
/**
 * \note Use this function to initialize the VuAreaTargetCaptureConfig data structure with default values.
 */
VU_API VuAreaTargetCaptureConfig VU_API_CALL vuAreaTargetCaptureConfigDefault();

/// \brief Configuration options for Area Target generation
typedef struct VuAreaTargetCaptureGenerationConfig
{
    /// \brief User name for authentication with the Vuforia server
    /**
     * \note The provided string is copied by \ref vuAreaTargetCaptureGenerate and can be freed afterwards.
     */
    const char* userAuth;

    /// \brief Secret key for authentication with the Vuforia server
    /**
     * \note The provided string is copied by \ref vuAreaTargetCaptureGenerate and can be freed afterwards.
     */
    const char* secretAuth;

    /// \brief Output directory path
    /**
     * The output directory path can be absolute or relative. The capture will store the files it was configured to
     * generate to this path. The directory must exist and be writable throughout Area Target generation.
     *
     * \note The provided string is copied by \ref vuAreaTargetCaptureGenerate and can be freed afterwards.
     */
    const char* outputDirectory;

    /// \brief Target name
    /**
     * The name of the generated Area Target is required to respect the following restrictions:
     * - Length: 1 - 64 characters
     * - Encoding: ASCII
     * - Allowed characters: numerals (0-9), literals (a-zA-Z), dash (-), underscore (_)
     *
     * \note All generated files use the target name as first component for their respective filenames.
     * \note The provided string is copied by \ref vuAreaTargetCaptureGenerate and can be freed afterwards.
     */
    const char* targetName;

    /// \brief Generate authoring files
    /**
     * If set to VU_TRUE, the following files are generated:
     * - The authoring representation of the captured space as a 3DT Space file. The filename is prefixed with the
     * target name and ends with *authoring.3dt*.
     * - The navigation mesh source for the captured space as a binary GLTF file. The filename is prefixed with the
     * target name and ends with *navmesh.glb*.
     *
     * If set to VU_FALSE, the authoring representation and the navigation mesh source are not generated.
     *
     * The authoring 3DT Space file can be used as input to external tools that support the editing and generation
     * of Area Targets.
     * The navigation mesh source file can be used as input to external tools or libraries to create a navigation
     * or path finding representation for the captured space.
     *
     * \note Default value is VU_TRUE
     */
    VuBool generateAuthoringFiles;

    /// \brief Generate Area Target database
    /**
     * If set to VU_TRUE, the following files are generated:
     * - The Area Target database for the captured space. The database files use the target name as filename.
     * - The runtime occlusion mesh for the captured space. The filename is prefixed with the target name and
     * ends with *occlusion.3dt*.
     *
     * If set to VU_FALSE, the Area Target database and the runtime occlusion mesh are not generated.
     *
     * The Area Target database can be used to create an Area Target observer. The runtime occlusion mesh can be
     * used to create the matching Mesh observer for the Area Target observer.
     *
     * \note Area Target database generation requires a network connection and valid credentials for communication
     * with the Vuforia server.
     * \note Default value is VU_TRUE
     */
    VuBool generateDatabase;

    /// \brief Generate packages
    /**
     * If set to VU_TRUE, an Area Target Unity Asset package to use with the Vuforia Engine Extension for Unity is generated.
     * The package uses the target name as filename, and *unitypackage* as file extension.
     *
     * If set to VU_FALSE, the Area Target Unity Asset package is not generated.
     *
     * \note If set to VU_TRUE, \ref generateAuthoringFiles and \ref generateDatabase are required to be set to VU_TRUE,
     * otherwise Area Target generation will fail to start.
     * \note Default value is VU_FALSE.
     */
    VuBool generatePackages;

    /// \brief Optional path to database containing an Area Target to use for alignment of the generated Area Target Capture
    /**
     * If generation is successful, then the newly generated Area Target will be aligned to the alignment target coordinate system.
     *
     * \note The best results will be achieved using small alignment targets, i.e. up to 1000 square meters. Large alignment targets,
     * especially those larger than 4000 square meters, will require significantly longer to align and may struggle to compute an
     * accurate alignment if there are several areas in the target with similar appearance to the captured space.
     *
     * \note Both \ref alignmentDatabasePath and \ref alignmentTargetName must be set for alignment to succeed. If \ref
     * alignmentDatabasePath is missing, then generation will fail with \ref
     * VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_ALIGNMENT_DATABASE_LOAD_ERROR.
     *
     * \note This option defaults to NULL. If no path is provided, then alignment will not be performed.
     *
     * \note The provided string is copied by \ref vuAreaTargetCaptureGenerate and can be freed afterwards.
     */
    const char* alignmentDatabasePath;

    /// \brief Optional name of an Area Target to use for alignment
    /**
     * If generation is successful, then the newly generated Area Target will be aligned to the alignment target coordinate system.
     *
     * \note Both \ref alignmentDatabasePath and \ref alignmentTargetName must be set for alignment to succeed. If \ref alignmentTargetName
     * is missing, then generation will fail with \ref VU_AREA_TARGET_CAPTURE_GENERATION_ERROR_INVALID_ALIGNMENT_TARGET_NAME.
     *
     * \note This option defaults to NULL. If no name is provided, then alignment will not be performed.
     *
     * \note The provided string is copied by \ref vuAreaTargetCaptureGenerate and can be freed afterwards.
     */
    const char* alignmentTargetName;
} VuAreaTargetCaptureGenerationConfig;

/// \brief Default generation configuration
/**
 * \note Use this function to initialize the VuAreaTargetCaptureGenerationConfig data structure with default values.
 */
VU_API VuAreaTargetCaptureGenerationConfig VU_API_CALL vuAreaTargetCaptureGenerationConfigDefault();

/// \brief Retrieve Area Target Capture Controller to get access to Area Target Capture functionality in Engine
VU_API VuResult VU_API_CALL vuEngineGetAreaTargetCaptureController(const VuEngine* engine, VuController** controller);

/// \brief Create a new capture with the specified configuration
/**
 * Any previously created capture has to be destroyed before creating a new capture.
 *
 * This function creates an Area Target capture based on the specified configuration.
 * On successful creation the capture has status \ref VU_AREA_TARGET_CAPTURE_STATUS_INITIALIZED,
 * unless the configuration's start flag is VU_TRUE, then the capture has status
 * \ref VU_AREA_TARGET_CAPTURE_STATUS_PREPARING. If creation of the capture fails,
 * then VU_FAILED is returned and the optional error parameter will hold an error code
 * that reflects the cause of the error.
 *
 * \note If Engine is not running and the configuration's start flag is VU_TRUE,
 * then on successful creation the capture has status
 * \ref VU_AREA_TARGET_CAPTURE_STATUS_PAUSED and status info
 * \ref VU_AREA_TARGET_CAPTURE_STATUS_INFO_SUSPENDED. The capture
 * will then be started automatically during the next call to \ref vuEngineStart().
 *
 * \param controller Area Target Capture Controller
 * \param config The configuration for the new capture
 * \param capture Output variable storing the newly created capture
 * \param error Optional output variable providing additional error information. Can be NULL.
 * \returns VU_SUCCESS on successful creation of the capture, VU_FAILED otherwise
 */
VU_API VuResult VU_API_CALL vuAreaTargetCaptureControllerCreateAreaTargetCapture(VuController* controller,
                                                                                 const VuAreaTargetCaptureConfig* config,
                                                                                 VuAreaTargetCapture** capture,
                                                                                 VuAreaTargetCaptureCreationError* error);

/// \brief Start a capture
/**
 * The capture starts data acquisition and switches to status \ref VU_AREA_TARGET_CAPTURE_STATUS_PREPARING.
 *
 * \note If Engine is not running, then the capture switches to status
 * \ref VU_AREA_TARGET_CAPTURE_STATUS_PAUSED and status info
 * \ref VU_AREA_TARGET_CAPTURE_STATUS_INFO_SUSPENDED. The capture
 * will then be started automatically during the next call to \ref vuEngineStart().
 *
 * \returns VU_SUCCESS if the capture is in status \ref VU_AREA_TARGET_CAPTURE_STATUS_INITIALIZED
 * and start is successful, VU_FAILED otherwise
 */
VU_API VuResult VU_API_CALL vuAreaTargetCaptureStart(VuAreaTargetCapture* capture);

/// \brief Stop a running or paused capture
/**
 * The capture stops data acquisition, and switches to status \ref VU_AREA_TARGET_CAPTURE_STATUS_STOPPED.
 *
 * \note An Area Target can be generated only when the capture is stopped.
 * \note This function fails if Engine is not running.
 *
 * \returns VU_SUCCESS if the capture is in status \ref VU_AREA_TARGET_CAPTURE_STATUS_PREPARING,
 * \ref VU_AREA_TARGET_CAPTURE_STATUS_CAPTURING, or \ref VU_AREA_TARGET_CAPTURE_STATUS_PAUSED
 * and stop is successful, VU_FAILED otherwise
 */
VU_API VuResult VU_API_CALL vuAreaTargetCaptureStop(VuAreaTargetCapture* capture);

/// \brief Pause a running capture
/**
 * Data acquisition of the capture can be paused and resumed. \ref vuAreaTargetCapturePause()
 * pauses data acquisition and switches the capture to status \ref VU_AREA_TARGET_CAPTURE_STATUS_PAUSED.
 * \ref vuAreaTargetCaptureResume() resumes data acquisition and switches the capture to the status it
 * was in before the pause.

 * \note If Engine is stopped while a capture is running, then the capture will be automatically
 * paused during \ref vuEngineStop(), and it will be automatically resumed during the next call
 * to \ref vuEngineStart(). A capture that is automatically paused by Engine has status info
 * \ref VU_AREA_TARGET_CAPTURE_STATUS_INFO_SUSPENDED.
 * \note This function fails if Engine is not running.
 *
 * \returns VU_SUCCESS if the capture is in status \ref VU_AREA_TARGET_CAPTURE_STATUS_PREPARING or
 * \ref VU_AREA_TARGET_CAPTURE_STATUS_CAPTURING and pause is successful, VU_FAILED otherwise
 */
VU_API VuResult VU_API_CALL vuAreaTargetCapturePause(VuAreaTargetCapture* capture);

/// \brief Resume a paused capture
/**
 * The capture resumes data acquisition, and switches to the status it was in before the pause.
 *
 * \note This function fails if Engine is not running.
 *
 * \returns VU_SUCCESS if the capture is in status \ref VU_AREA_TARGET_CAPTURE_STATUS_PAUSED and resume is successful, VU_FAILED otherwise
 */
VU_API VuResult VU_API_CALL vuAreaTargetCaptureResume(VuAreaTargetCapture* capture);

/// \brief Start Area Target generation from a stopped capture
/**
 * This function starts Area Target generation based on the specified configuration. Once successfully started,
 * Area Target generation runs in the background. For Area Target generation to successfully start, the following
 * requirements have to be met:
 * - Engine is required to be running.
 * - The capture is required to have status \ref VU_AREA_TARGET_CAPTURE_STATUS_STOPPED.
 * - The capture is required to have accumulated sufficient data to switch from the
 *  \ref VU_AREA_TARGET_CAPTURE_STATUS_PREPARING status to the
 *  \ref VU_AREA_TARGET_CAPTURE_STATUS_CAPTURING status.
 * - The generation configuration is required to be valid. It has to provide the path to an existing output directory
 *  that can be written to, and it has to provide a valid target name. If database generation is enabled, then the
 *  configuration has to hold valid credentials for communication with the Vuforia server.
 * - For aligned generation, the generation configuration must also provide the path to a valid Area Target database
 *  and the name of the target that will be used for alignment. If generation is successful, then the newly generated
 *  Area Target will be aligned to the alignment target coordinate system.
 *
 * If all the requirements are met, this function starts Area Target generation in the background, switches the capture to status
 * \ref VU_AREA_TARGET_CAPTURE_STATUS_GENERATING, and returns VU_SUCCESS. If any of the requirements
 * are not met, VU_FAILED is returned and the optional error parameter will hold an error code that reflects the cause of the error.
 *
 * Area Target generation runs in the background. When it completes, the capture switches back
 * to status \ref VU_AREA_TARGET_CAPTURE_STATUS_STOPPED. On successful Area Target
 * generation, status info reports \ref VU_AREA_TARGET_CAPTURE_STATUS_INFO_GENERATION_SUCCESS.
 * If Area Target generation failed, then status info holds one of
 * the values prefixed with \ref VuAreaTargetCaptureStatusInfo "VU_AREA_TARGET_CAPTURE_STATUS_INFO_GENERATION_ERROR"
 * to report on the failure cause. If Area Target generation was canceled, status info holds the value
 * \ref VU_AREA_TARGET_CAPTURE_STATUS_INFO_GENERATION_CANCELED.
 *
 * \note If database generation is enabled, then a network connection is required.
 * \note If Engine is stopped while Area Target generation is running, then Area Target generation
 * is automatically canceled.
 *
 * \param capture The Area Target Capture instance
 * \param config The configuration for the Area Target generation
 * \param error Optional output variable providing additional error information. Can be NULL.
 *
 * \returns VU_SUCCESS if the Area Target generation has successfully started, VU_FAILED otherwise
 */
VU_API VuResult VU_API_CALL vuAreaTargetCaptureGenerate(VuAreaTargetCapture* capture, const VuAreaTargetCaptureGenerationConfig* config,
                                                        VuAreaTargetCaptureGenerationError* error);

/// \brief Get status of the capture
VU_API VuResult VU_API_CALL vuAreaTargetCaptureGetStatus(const VuAreaTargetCapture* capture, VuAreaTargetCaptureStatus* status);

/// \brief Get status info for the capture
VU_API VuResult VU_API_CALL vuAreaTargetCaptureGetStatusInfo(const VuAreaTargetCapture* capture, VuAreaTargetCaptureStatusInfo* statusInfo);

/// \brief Cancel Area Target generation
/**
 * A helper function to abort a running target generation.
 * If the target generation has been canceled successfully, then status info will hold the
 * value \ref VU_AREA_TARGET_CAPTURE_STATUS_INFO_GENERATION_CANCELED.
 *
 * \note This function blocks until target generation has aborted
 * \note This is a potentially long running operation
 *
 * \returns VU_SUCCESS if the capture is in status \ref VU_AREA_TARGET_CAPTURE_STATUS_GENERATING and cancelation is successful, VU_FAILED
 * otherwise
 */
VU_API VuResult VU_API_CALL vuAreaTargetCaptureCancelGeneration(VuAreaTargetCapture* capture);

/// \brief Get progress information of the current Area Target generation in the range [0.0f, 1.0f]
/**
 * \returns VU_SUCCESS if the capture is in status \ref VU_AREA_TARGET_CAPTURE_STATUS_GENERATING, VU_FAILED otherwise
 */
VU_API VuResult VU_API_CALL vuAreaTargetCaptureGetGenerationProgress(const VuAreaTargetCapture* capture, float* progress);

/// \brief Get estimated time remaining to complete the current Area Target generation in seconds
/**
 * It may take few seconds after the Area Target generation started for the time estimate to become available.
 *
 * \returns VU_SUCCESS if the capture is in status \ref VU_AREA_TARGET_CAPTURE_STATUS_GENERATING and the time estimate is available,
 * VU_FAILED otherwise
 */
VU_API VuResult VU_API_CALL vuAreaTargetCaptureGetGenerationTimeEstimate(const VuAreaTargetCapture* capture, int32_t* remainingTimeSeconds);

/// \brief Destroy the given capture instance
/**
 * If the capture is running data acquisition, it is automatically stopped. If the capture is running target generation,
 * it is automatically canceled.
 *
 * \note This is a potentially long running operation
 */
VU_API VuResult VU_API_CALL vuAreaTargetCaptureDestroy(VuAreaTargetCapture* capture);

/** \} */

/** \addtogroup MeshObserverGroup Mesh Feature
 * \{
 */

/// \brief Configuration error for Mesh observer creation with Area Target Capture
VU_ENUM(VuMeshAreaTargetCaptureCreationError)
{
    VU_MESH_AREA_TARGET_CAPTURE_CREATION_ERROR_NONE = 0x0,                  ///< No error
    VU_MESH_AREA_TARGET_CAPTURE_CREATION_ERROR_INTERNAL = 0x1,              ///< An internal error occurred while creating the observer
    VU_MESH_AREA_TARGET_CAPTURE_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2, ///< An error occurred while auto-activating the observer
    VU_MESH_AREA_TARGET_CAPTURE_CREATION_ERROR_FEATURE_NOT_SUPPORTED = 0x3, ///< Mesh functionality is not supported on the current device
    VU_MESH_AREA_TARGET_CAPTURE_CREATION_ERROR_INVALID_CAPTURE = 0x4,       ///< The provided capture is NULL or invalid
    VU_MESH_AREA_TARGET_CAPTURE_CREATION_ERROR_SAME_SOURCE_NOT_SUPPORTED =
        0x5 ///< A mesh observer is already attached to the Area Target Capture instance
};

/// \brief Configuration for creating a Mesh observer using an Area Target Capture instance
typedef struct VuMeshAreaTargetCaptureConfig
{
    /// \brief Pointer to the Area Target Capture instance
    /**
     * The Area Target Capture instance is the exclusive source of the Mesh observations
     * that are reported by a Mesh observer created with this configuration.
     */
    VuAreaTargetCapture* capture;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;
} VuMeshAreaTargetCaptureConfig;

/// \brief Default Mesh observer configuration with Area Target Capture source
/**
 * \note Use this function to initialize the VuMeshAreaTargetCaptureConfig data structure with default values.
 */
VU_API VuMeshAreaTargetCaptureConfig VU_API_CALL vuMeshAreaTargetCaptureConfigDefault();

/// \brief Create a Mesh observer with an Area Target Capture instance as source
/**
 * The Mesh observer will deliver Mesh observations that represent a mesh reconstruction of the environment
 * as provided by the specified Area Target Capture instance. Because the Mesh observer is dependent on the
 * existence of the Area Target Capture instance, one has to make sure to destroy the Mesh observer before
 * the Area Target Capture instance is destroyed.
 */
VU_API VuResult VU_API_CALL vuEngineCreateMeshObserverFromAreaTargetCaptureConfig(VuEngine* engine, VuObserver** observer,
                                                                                  const VuMeshAreaTargetCaptureConfig* config,
                                                                                  VuMeshAreaTargetCaptureCreationError* errorCode);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_AREATARGETCAPTURECONTROLLER_H_
