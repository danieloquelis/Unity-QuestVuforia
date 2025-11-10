/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_SESSIONRECORDERCONTROLLER_H_
#define _VU_SESSIONRECORDERCONTROLLER_H_

/**
 * \file SessionRecorderController.h
 * \brief Controller to access the session recording functionality in the Vuforia Engine
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup SessionRecorderControllerGroup Session Recorder Controller
 * \{
 * This controller facilitates recording data from data sources such as on-device camera and sensors
 * while running Vuforia.
 *
 * \par Permissions
 *
 * In order to support audio recording capabilities in the Session Recorder Controller, your App must fulfill audio-related,
 * platform-specific permission / configuration requirements. Your App is responsible for executing the following platform-specific actions
 * prior to invoking Session Recorder Controller API functions relying on information whether audio recording is enabled in the app:
 *
 * - Android:
 *     - Add the "android.permission.RECORD_AUDIO" permission to the AndroidManifest.xml and also request it at run-time in your App's code.
 *
 * - iOS:
 *     - Microphone usage description: Add the key "NSMicrophoneUsageDescription" to the Info.plist with an explanation why your
 *       App requires microphone access, otherwise the App will be rejected by the App Store submission process.
 *     - Microphone access: Request access to the microphone by calling the "AVCaptureDevice requestAccessForMediaType" API with the
 *       media type "AVMediaTypeAudio".
 *
 * - UWP:
 *     - Add the following snippet to the \b \<Capabilities\> tag in your App's Package.appxmanifest to enable audio recording capabilities:
 *
 *       \code{.xml}
 *         <Capabilities>
 *             <DeviceCapability Name="microphone" />
 *         </Capabilities>
 *       \endcode
 *
 * - Lumin:
 *     - Add the key "AudioCaptureMic" to the manifest.xml to enable audio recording capabilities.
 *
 * If the aforementioned platform-specific permission / configuration requirements are not fulfilled by the App, then
 *     - the vuSessionRecorderControllerGetSupportedRecordingDataFlags call won't report audio support
 *       (i.e. VU_RECORDING_DATA_AUDIO_BIT will be missing)
 *     - the vuSessionRecorderControllerCreateRecording call will fail with VU_RECORDING_START_ERROR_DATA_SOURCE if
 *       VuRecordingConfig.dataFlags is configured to record audio
 */

/// \brief Recording creation error
VU_ENUM(VuRecordingCreationError)
{
    VU_RECORDING_CREATION_ERROR_NONE = 0x0, ///< No error
    VU_RECORDING_CREATION_ERROR_DATA_FLAGS_NOT_SUPPORTED =
        0x1,                                            ///< One or more requested data flags are not supported on the current device
    VU_RECORDING_CREATION_ERROR_AUTOSTART_FAILED = 0x2, ///< Auto-start of the recording failed
};

/// \brief Recording start error
VU_ENUM(VuRecordingStartError)
{
    VU_RECORDING_START_ERROR_NONE = 0x0, ///< No error
    VU_RECORDING_START_ERROR_INTERNAL =
        0x1, ///< Starting failed due to an internal error. Please check the device logs for additional information.
    VU_RECORDING_START_ERROR_ANOTHER_RECORDING_RUNNING = 0x2, ///< Failed to start a recording because another recording is already in
                                                              ///< progress. Stop the other recording before starting a new one.
    VU_RECORDING_START_ERROR_ENGINE_NOT_RUNNING = 0x3,        ///< Recording cannot be started as the engine is not running
    VU_RECORDING_START_ERROR_INVALID_STATUS =
        0x4, ///< Cannot start a recording that is not in the initialized state. The user should create a new recording.
    VU_RECORDING_START_ERROR_FILE_CREATION = 0x5,       ///< Cannot start the recording because the output file could not be created
    VU_RECORDING_START_ERROR_UNKNOWN_ORIENTATION = 0x6, ///< Device is in an unknown orientation
    VU_RECORDING_START_ERROR_DATA_SOURCE =
        0x7, ///< Failed to start recording from some of the selected data sources. This could happen if e.g.
             ///< another application has exclusive access to a source, or required platform-specific permission / configuration
             ///< requirements haven't been fulfilled by the App
    VU_RECORDING_START_ERROR_INSUFFICIENT_FREE_SPACE =
        0x8, ///< Cannot start a recording as there isn't sufficient free space on the device to store it
};

/// \brief Session recording status
VU_ENUM(VuRecordingStatus)
{
    VU_RECORDING_STATUS_INITIALIZED = 0x1, ///< The recording has been created and can be started
    VU_RECORDING_STATUS_RUNNING = 0x2,     ///< The recording is in progress
    VU_RECORDING_STATUS_STOPPED = 0x3, ///< The recording has stopped. It cannot be started again. Check the status info for error details.
};

/// \brief Session recording status info
VU_ENUM(VuRecordingStatusInfo)
{
    VU_RECORDING_STATUS_INFO_NORMAL = 0x0,  ///< Normal operation. No error has occurred.
    VU_RECORDING_STATUS_INFO_SUCCESS = 0x1, ///< The recording completed successfully
    VU_RECORDING_STATUS_INFO_ERROR_INTERNAL =
        0x2, ///< Recording was aborted due to an internal error. Please check the device logs for additional information.
    VU_RECORDING_STATUS_INFO_ERROR_ORIENTATION_CHANGED = 0x3, ///< The device orientation changed during recording
    VU_RECORDING_STATUS_INFO_ERROR_DATA_SOURCE = 0x4,         ///< Recording from some of the selected data sources failed
    VU_RECORDING_STATUS_INFO_ERROR_INSUFFICIENT_FREE_SPACE =
        0x5, ///< The recording stopped as there isn't sufficient free space on the device to store it
};

/// \brief Recording instance
typedef struct VuRecording_ VuRecording;

/// \brief Recording list handle
typedef struct VuRecordingList_ VuRecordingList;

/// \brief Bitflag defining a collection of data flags
typedef VuFlags VuRecordingDataFlags;

/// \brief Flags that specify which data should be recorded
/**
 * \note Use vuSessionRecorderControllerGetDefaultDataFlags() to get the default
 * data flags that are appropriate for the current device to create a recording
 * that can be used for AR session playback.
 */
VU_ENUM(VuRecordingDataFlagBits)
{
    VU_RECORDING_DATA_VIDEO_BIT = 0x01,           ///< Camera images
    VU_RECORDING_DATA_CAMERA_METADATA_BIT = 0x02, ///< Camera metadata required for AR session playback
    VU_RECORDING_DATA_SENSORS_BIT = 0x04,         ///< Data from device sensors (accelerometer, gyroscope and magnetometer) depending
                                                  ///< on which are available on the current device
    VU_RECORDING_DATA_DEVICE_POSE_BIT = 0x08,     ///< Device poses generated by the platform-provided Vuforia Fusion provider
    VU_RECORDING_DATA_AUDIO_BIT = 0x10            ///< Audio recording from the default audio input device
};

/// \brief The frame rate at which camera images are recorded
VU_ENUM(VuRecordingFrameRate)
{
    VU_RECORDING_FRAME_RATE_AUTO = 0x1, ///< Let Vuforia automatically choose the option that will provide
                                        ///< the best performance during recording. This is the default
    VU_RECORDING_FRAME_RATE_FULL = 0x2, ///< Record camera images at the full frame rate of the camera
    VU_RECORDING_FRAME_RATE_HALF = 0x3  ///< Record camera images at half the frame rate of the camera
};

/// \brief The scale factor to be applied to camera images before recording
VU_ENUM(VuRecordingImageScale)
{
    VU_RECORDING_IMAGE_SCALE_AUTO = 0x1, ///< Let Vuforia automatically choose the option that will provide the best
                                         ///< performance during recording. This is the default
    VU_RECORDING_IMAGE_SCALE_FULL = 0x2, ///< Record camera images at full resolution
    VU_RECORDING_IMAGE_SCALE_HALF = 0x3  ///< Downsample camera images to half size along both dimensions
};

/// \brief Quality settings used for encoding the recorded video
VU_ENUM(VuRecordingVideoEncodingQuality)
{
    VU_RECORDING_VIDEO_ENCODING_QUALITY_HIGH = 0x1,  ///< High quality video encoding with minimal compression artifacts. Recordings with
                                                     ///< this setting are suitable for playback as AR sessions, but will have an increased
                                                     ///< file size. This is the default. (approx. 15MBit/s video bitrate for 720p@30 video)
    VU_RECORDING_VIDEO_ENCODING_QUALITY_MEDIUM = 0x2 ///< Medium quality video encoding for general purpose video recording.
                                                     ///< (approx. 5MBit/s video bitrate for 720p@30 video)
};

/// \brief Session recording format
/**
 * \note Only one format is currently supported. Others might be added in the future.
 */
VU_ENUM(VuRecordingFormat)
{
    VU_RECORDING_FORMAT_DEFAULT = 0x1 ///< Default format. MP4 file with H.264 compressed video.
};

/// \brief Configuration options for a recording session
typedef struct VuRecordingConfig
{
    /// \brief Bitmask specifying data to be recorded
    /**
     * The bitmask should use the enum values from VuRecordingDataFlagBits as bit flags.
     *
     * \note vuRecordingConfigDefault() will set this flag to zero. It has to be
     * populated explicitly with the desired data flags, otherwise starting the recording will fail.
     *
     * \note Use vuSessionRecorderControllerGetDefaultDataFlags() to get the default
     * data flags that are appropriate for the current device. The flags returned by that function
     * will lead to a recording that can be used for playing back AR sessions.
     */
    VuRecordingDataFlags dataFlags;

    /// \brief The frame rate to record camera images at
    /**
     * \note vuRecordingConfigDefault() will set this to VU_RECORDING_FRAME_RATE_AUTO and the appropriate
     * value will be determined automatically. To retrieve the actual value that is used for recordings on the
     * current device, use vuSessionRecorderControllerGetDefaultRecordingFrameRate().
     *
     * \note If overriding the default recording frame rate, please note that this feature is for advanced users who are sure
     * that their devices are powerful enough to handle recording frame rates that are higher than the default.
     */
    VuRecordingFrameRate frameRate;

    /// \brief The scale applied to the camera images when recorded
    /**
     * \note vuRecordingConfigDefault() will set this to VU_RECORDING_IMAGE_SCALE_AUTO and the appropriate
     * value will be determined automatically. To retrieve the actual value that is used for recordings on the
     * current device, use vuSessionRecorderControllerGetDefaultRecordingImageScale().
     *
     * \note If overriding the default image scale, please note that this feature is for advanced users who are sure that their devices
     * are powerful enough to handle recording camera images at a scale that is higher than the default.
     */
    VuRecordingImageScale scale;

    /// \brief Recording format
    /**
     * \note The default value is VU_RECORDING_FORMAT_DEFAULT.
     */
    VuRecordingFormat format;

    /// \brief Path to the directory where the recording should be stored.
    /**
     * \note This parameter is optional. The default value is NULL. The directory must
     * exist and be writable throughout the lifetime of the recording. If no path is
     * provided, Session Recorder will store the recording in an automatically determined
     * location.
     *
     * \note The provided string is copied, and can be freed, after the recording is created.
     */
    const char* outputDirectory;

    /// \brief Flag to control whether a recording should start immediately on creation
    /**
     * \note The default value is VU_FALSE.
     */
    VuBool start;

    /// \brief Video encoding quality
    /**
     * \note The default value is VU_RECORDING_VIDEO_ENCODING_QUALITY_HIGH.
     */
    VuRecordingVideoEncodingQuality videoEncodingQuality;
} VuRecordingConfig;

/// \brief Default configuration for a recording session
/**
 * \note Use this function to initialize the VuRecordingConfig data structure with default values.
 */
VU_API VuRecordingConfig VU_API_CALL vuRecordingConfigDefault();

/// \brief Retrieve Session Recorder Controller to get access to session recording-specific functionality in Engine
VU_API VuResult VU_API_CALL vuEngineGetSessionRecorderController(const VuEngine* engine, VuController** controller);

/// \brief Get the default recording data flags that are appropriate for the current device
/**
 * \note The flags returned by this function will configure a recording with all data
 * required for playing it back as an AR session.
 *
 * \note The flags returned by this function depend on the current VuFusionProviderType.
 *
 * \param controller Session Recorder Controller
 * \param dataFlags Output variable storing the default data flags bitmask
 * \returns VU_SUCCESS on successful retrieval of the data flags, VU_FAILED on failure
 */
VU_API VuResult VU_API_CALL vuSessionRecorderControllerGetDefaultRecordingDataFlags(const VuController* controller,
                                                                                    VuRecordingDataFlags* dataFlags);

/// \brief Get all recording data flags that are supported by the current device
/**
 * \param controller Session Recorder Controller
 * \param dataFlags Output variable storing the data flags bitmask
 * \returns VU_SUCCESS on successful retrieval of the data flags, VU_FAILED on failure
 */
VU_API VuResult VU_API_CALL vuSessionRecorderControllerGetSupportedRecordingDataFlags(const VuController* controller,
                                                                                      VuRecordingDataFlags* dataFlags);

/// \brief Get the default camera recording frame rate
/**
 * \note This function will return the recommended default recording frame rate setting.
 *
 * \note This method can only be called while the engine is running.
 *
 * The output value will always be an explicit one (i.e. never VU_RECORDING_FRAME_RATE_AUTO) so the caller can determine
 * the specific value being used for recording.
 */
VU_API VuResult VU_API_CALL vuSessionRecorderControllerGetDefaultRecordingFrameRate(const VuController* controller,
                                                                                    VuRecordingFrameRate* frameRate);

/// \brief Get the default recording image scale
/**
 * \note This function will return the recommended default recording image scale setting.
 *
 * \note This method can only be called while the engine is running.
 *
 * The output value will always be an explicit one (i.e. never VU_RECORDING_IMAGE_SCALE_AUTO) so the caller can determine
 * the specific value being used for recording.
 */
VU_API VuResult VU_API_CALL vuSessionRecorderControllerGetDefaultRecordingImageScale(const VuController* controller,
                                                                                     VuRecordingImageScale* scale);

/// \brief Create a new recording with the specified configuration
/**
 * \note If "start" is set to VU_TRUE in the recording configuration and another recording is
 * already running, this call will fail. Any previously started recording needs to be stopped first.
 *
 * \param controller Session Recorder Controller
 * \param config The configuration for the new recording
 * \param recording Output variable storing the newly created recording
 * \param error Optional output variable providing additional error information. Can be NULL.
 * \returns VU_SUCCESS on successfully creation of the recording, VU_FAILED on failure
 */
VU_API VuResult VU_API_CALL vuSessionRecorderControllerCreateRecording(VuController* controller, const VuRecordingConfig* config,
                                                                       VuRecording** recording, VuRecordingCreationError* error);

/// \brief Get a list of all recordings from the controller
/**
 * \param controller Session Recorder Controller
 * \param recordingList List that will be filled with the recordings
 * \returns VU_SUCCESS on success, or VU_FAILED otherwise
 */
VU_API VuResult VU_API_CALL vuSessionRecorderControllerGetRecordings(const VuController* controller, VuRecordingList* recordingList);

/// \brief Destroy all recordings
/**
 * This call will also stop any ongoing recording.
 *
 * \param controller Session Recorder Controller
 * \param deleteData Set to VU_TRUE to delete all data generated by the recording instances before their destruction
 * \returns VU_SUCCESS on success, or VU_FAILED otherwise
 */
VU_API VuResult VU_API_CALL vuSessionRecorderControllerDestroyRecordings(VuController* controller, VuBool deleteData);

/// \brief Remove all previously recorded sequences from the device storage
/**
 * \note This call can only be used when no recording instances exist. Stop any running recording and
 * destroy all recording instances before calling this.
 *
 * \param controller Session Recorder Controller
 * \returns VU_SUCCESS if all recorded data was removed successfully, or VU_FAILED otherwise
 */
VU_API VuResult VU_API_CALL vuSessionRecorderControllerCleanRecordedData(VuController* controller);

/// \brief Start the recording
/**
 * \note If another recording is already running, this call will fail. Any previously started recording
 * needs to be stopped first.
 *
 * \note Session Recorder does not support changes in device orientation during recording. If the
 * device orientation changes while a recording is running, it will be automatically stopped.
 *
 * \note Calling this on a recording that is already running or has already been stopped will return VU_FAILED
 * with error VU_RECORDING_START_ERROR_INVALID_STATUS.
 *
 * \param recording The current recording
 * \param error Optional output variable providing additional error information. Can be NULL.
 * \returns VU_SUCCESS on successfully start of the recording, VU_FAILED on failure
 */
VU_API VuResult VU_API_CALL vuRecordingStart(VuRecording* recording, VuRecordingStartError* error);

/// \brief Stop the current recording
/**
 * \note The recording will be stopped automatically when vuEngineStop() is called or when it is destroyed.
 *
 * \note Calling this on a recording that is not running will return VU_FAILED
 *
 * \param recording The current recording
 * \returns VU_SUCCESS on successfully stopping the recording, VU_FAILED on failure
 */
VU_API VuResult VU_API_CALL vuRecordingStop(VuRecording* recording);

/// \brief Get the path where the data for this recording is stored
/**
 * \note The recording path is only available after the recording has been started successfully.
 *
 * \note The lifetime of the returned string is bound to the lifetime of the recording.
 *
 * \param recording The current recording
 * \param path The absolute path to where the recording is stored. The parameter will stay unmodified on failure.
 * \returns VU_SUCCESS on successful retrieval of the path, VU_FAILED if no recording path is available.
 */
VU_API VuResult VU_API_CALL vuRecordingGetPath(const VuRecording* recording, const char** path);

/// \brief Get the status of a recording
/**
 * \param recording The current recording
 * \param status Current recording status
 * \returns VU_SUCCESS on successfully getting status, VU_FAILED on failure
 */
VU_API VuResult VU_API_CALL vuRecordingGetStatus(const VuRecording* recording, VuRecordingStatus* status);

/// \brief Get additional status info about a recording
/**
 * \param recording The current recording
 * \param statusInfo Current recording status info
 * \returns VU_SUCCESS on successfully getting status info, VU_FAILED on failure
 */
VU_API VuResult VU_API_CALL vuRecordingGetStatusInfo(const VuRecording* recording, VuRecordingStatusInfo* statusInfo);

/// \brief Destroy a recording instance
/**
 * \note The recording will be stopped if it is running.
 *
 * \param recording Recording instance
 * \param deleteData If set to VU_TRUE, the recorded data in device storage will be deleted as well
 * \returns VU_SUCCESS on success, VU_FAILED on failure
 */
VU_API VuResult VU_API_CALL vuRecordingDestroy(VuRecording* recording, VuBool deleteData);

/// \brief Create a recording list
VU_API VuResult VU_API_CALL vuRecordingListCreate(VuRecordingList** list);

/// \brief Get the number of elements in a recording list
VU_API VuResult VU_API_CALL vuRecordingListGetSize(const VuRecordingList* list, int32_t* listSize);

/// \brief Get an element in a recording list
VU_API VuResult VU_API_CALL vuRecordingListGetElement(const VuRecordingList* list, int32_t element, VuRecording** recording);

/// \brief Destroy a recording list
VU_API VuResult VU_API_CALL vuRecordingListDestroy(VuRecordingList* list);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_SESSIONRECORDERCONTROLLER_H_
