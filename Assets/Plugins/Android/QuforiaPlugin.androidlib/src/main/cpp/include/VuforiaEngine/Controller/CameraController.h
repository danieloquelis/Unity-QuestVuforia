/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_CAMERACONTROLLER_H_
#define _VU_CAMERACONTROLLER_H_

/**
 * \file CameraController.h
 * \brief Controller to access camera-specific functionality in the Vuforia Engine
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup CameraControllerGroup Camera Controller
 * \{
 * This controller allows the control of camera features such as configuring the video mode,
 * focus mode, exposure mode, flash mode or accessing advanced camera properties.
 */

/// \brief Supported camera video mode presets
VU_ENUM(VuCameraVideoModePreset)
{
    VU_CAMERA_VIDEO_MODE_PRESET_DEFAULT = 0x1,         ///< Default camera mode
    VU_CAMERA_VIDEO_MODE_PRESET_OPTIMIZE_SPEED = 0x2,  ///< Fast camera mode. Camera mode that reduces
                                                       ///< the system resource impact of Vuforia Engine
                                                       ///< at the cost of lower image and/or tracking
                                                       ///< quality.
    VU_CAMERA_VIDEO_MODE_PRESET_OPTIMIZE_QUALITY = 0x3 ///< High-quality camera mode. Camera mode that
                                                       ///< maximizes image and tracking quality at the
                                                       ///< cost of higher system resource impact.
};

/// \brief Supported camera focus modes
VU_ENUM(VuCameraFocusMode)
{
    VU_CAMERA_FOCUS_MODE_UNKNOWN = 0x1,        ///< Unknown focus mode
    VU_CAMERA_FOCUS_MODE_TRIGGERAUTO = 0x3,    ///< Focus mode to trigger a single auto-focus operation
    VU_CAMERA_FOCUS_MODE_CONTINUOUSAUTO = 0x4, ///< Continuous auto-focus mode
    VU_CAMERA_FOCUS_MODE_INFINITY = 0x5,       ///< Focus set to infinity
    VU_CAMERA_FOCUS_MODE_MACRO = 0x6,          ///< Macro mode for close-up focus
    VU_CAMERA_FOCUS_MODE_FIXED = 0x7           ///< Fixed focus mode
};

/// \brief Supported camera exposure modes
VU_ENUM(VuCameraExposureMode)
{
    VU_CAMERA_EXPOSURE_MODE_UNKNOWN = 0x1,        ///< Unknown exposure mode
    VU_CAMERA_EXPOSURE_MODE_TRIGGERAUTO = 0x2,    ///< Exposure mode to trigger a single auto-exposure operation
    VU_CAMERA_EXPOSURE_MODE_CONTINUOUSAUTO = 0x3, ///< Continuous auto-exposure mode
    VU_CAMERA_EXPOSURE_MODE_FIXED = 0x4           ///< Fixed exposure mode
};

/// \brief Data structure for setting and getting focus and exposure regions of interest in the camera image
typedef struct VuCameraRegionOfInterest
{
    /// \brief Point in the normalized coordinate space of the camera frame to use as the center of the region
    /**
     * \note Top left = (0.0f, 0.0f), bottom right = (1.0f, 1.0f)
     */
    VuVector2F center;

    /// \brief Extent of the region as a percentage of the camera frame width and height
    /**
     * \note (0.0f) = single pixel, (1.0f) = full width and height of the camera frame
     *
     * \note Setting the extent smaller than 1% is not supported on some platforms
     *
     * \note This property is ignored on iOS
     */
    float extent;
} VuCameraRegionOfInterest;

/// \brief Camera video mode description
typedef struct VuCameraVideoMode
{
    /// \brief Camera video mode preset mode
    VuCameraVideoModePreset presetMode;

    /// \brief Video frame resolution
    VuVector2I resolution;

    /// \brief Video frame rate
    float frameRate;

    /// \brief Video frame format
    VuImagePixelFormat format;
} VuCameraVideoMode;

/// \brief List of video modes available for a camera
typedef struct VuCameraVideoModeList_ VuCameraVideoModeList;

/// \brief Retrieve Camera Controller to get access to camera-specific functionality in Engine
VU_API VuResult VU_API_CALL vuEngineGetCameraController(const VuEngine* engine, VuController** controller);

/// \brief Get all the supported video modes for the camera
/**
 * \note If this is called before the engine is started, the camera will be accessed
 * which may be a longer-running operation on some platforms
 */
VU_API VuResult VU_API_CALL vuCameraControllerGetVideoModes(const VuController* controller, VuCameraVideoModeList* cameraVideoModeList);

/// \brief Create a camera video mode list
VU_API VuResult VU_API_CALL vuCameraVideoModeListCreate(VuCameraVideoModeList** list);

/// \brief Get number of elements in a camera video mode list
VU_API VuResult VU_API_CALL vuCameraVideoModeListGetSize(const VuCameraVideoModeList* list, int32_t* listSize);

/// \brief Get an element in a camera video mode list
VU_API VuResult VU_API_CALL vuCameraVideoModeListGetElement(const VuCameraVideoModeList* list, int32_t element,
                                                            VuCameraVideoMode* videoMode);

/// \brief Destroy a camera video mode list
VU_API VuResult VU_API_CALL vuCameraVideoModeListDestroy(VuCameraVideoModeList* list);

/// \brief Get the the currently active video mode of the camera
VU_API VuResult VU_API_CALL vuCameraControllerGetActiveVideoMode(const VuController* controller,
                                                                 VuCameraVideoModePreset* cameraVideoModePreset);

/// \brief Set the current video mode of the camera from the list of supported presets
/**
 * \note This function can only be called before the engine is started. To change the video mode after the engine is started,
 * stop the engine, then change the video mode and restart it again.
 */
VU_API VuResult VU_API_CALL vuCameraControllerSetActiveVideoMode(VuController* controller, VuCameraVideoModePreset cameraVideoModePreset);

/// \brief Get the current flash mode of the camera
/**
 * \note This function can only be called while the engine is running
 *
 * \note This function will output the current value as it is reported by the device
 *
 * \returns VU_SUCCESS on success or VU_FAILED if this operation is not supported on the current device or Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerGetFlashMode(const VuController* controller, VuBool* flashMode);

/// \brief Set the flash mode of the camera
/**
 * \note This function can only be called while the engine is running
 *
 * \note Setting the flash mode might not take effect immediately. Depending on the platform it may
 * take up to a few hundred milliseconds until the change is applied after this function returns. Use
 * \ref vuCameraControllerGetFlashMode to query the current state as it is reported by the device.
 *
 * \note The flash mode is retained across Vuforia Engine stop/start, for instance when an App is paused
 * and then resumed.
 *
 * \returns VU_SUCCESS on success or VU_FAILED if this operation is not supported on the current device or Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerSetFlashMode(VuController* controller, VuBool flashMode);

/// \brief Check if setting the flash mode is supported on the current device and camera
/**
 * \note This function can only be called while Vuforia Engine is running
 *
 * \note On Android devices flash mode support requires ARCore version 1.45 or newer
 *
 * \note When this function returns VU_FALSE in 'isFlashModeSupported', calls to setting and getting the respective flash mode will fail.
 *
 * \returns VU_SUCCESS on success or VU_FAILED if Vuforia Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerIsFlashModeSupported(VuController* controller, VuBool* isFlashModeSupported);

/// \brief Get the current focus mode of the camera
/**
 * \note This function can only be called while Vuforia Engine is running
 *
 * \note This function will output the current value as it is reported by the device
 *
 * \returns VU_SUCCESS on success or VU_FAILED if this operation is not supported on the current device or Vuforia
 * Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerGetFocusMode(const VuController* controller, VuCameraFocusMode* focusMode);

/// \brief Set the focus mode of the camera
/**
 * \note This function can only be called while Vuforia Engine is running
 *
 * \note Setting the focus mode might not take effect immediately. Depending on the platform it may
 * take up to a few hundred milliseconds until the change is applied after this function returns. Use
 * \ref vuCameraControllerGetFocusMode to query the current state as it is reported by the device.
 *
 * \note Changing the focus might have a negative effect on the performance of Vuforia tracking, in particular when
 * applying more extreme changes.
 *
 * \note The focus mode is retained across Vuforia Engine stop/start, for instance when an App is paused and then resumed,
 * EXCEPT for the case where the focus mode has changed to VU_CAMERA_FOCUS_MODE_FIXED as a result of VU_CAMERA_FOCUS_MODE_TRIGGERAUTO,
 * in which case the focus mode will be set to VU_CAMERA_FOCUS_MODE_CONTINUOUSAUTO on resume.
 *
 * \returns VU_SUCCESS on success or VU_FAILED if this operation or the requested focus mode is not supported on
 * the current device, or Vuforia Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerSetFocusMode(VuController* controller, VuCameraFocusMode focusMode);

/// \brief Get the current exposure mode of the camera
/**
 * \note This function can only be called while Vuforia Engine is running
 *
 * \note This function will output the current value as it is reported by the device
 *
 * \returns VU_SUCCESS on success or VU_FAILED if this operation is not supported on the current device or Vuforia
 * Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerGetExposureMode(const VuController* controller, VuCameraExposureMode* exposureMode);

/// \brief Set the exposure mode of the camera
/**
 * \note This function can only be called while Vuforia Engine is running
 *
 * \note Setting the exposure mode might not take effect immediately. Depending on the platform it may
 * take up to a few hundred milliseconds until the change is applied after this function returns. Use
 * \ref vuCameraControllerGetExposureMode to query the current state as it is reported by the device.
 *
 * \note Changing the exposure might have a negative effect on the performance of Vuforia tracking, in particular when
 * applying more extreme changes.
 *
 * \note Setting the exposure mode is not supported on all platforms and fusion providers,
 * see \ref vuCameraControllerIsExposureModeSupported.
 *
 * \note The exposure mode is NOT retained across Vuforia Engine stop/start: for instance when an App is paused and then resumed,
 * it will revert back to the default exposure mode.
 *
 * \returns VU_SUCCESS on success or VU_FAILED if this operation or the requested exposure mode is not supported on
 * the current device, or Vuforia Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerSetExposureMode(VuController* controller, VuCameraExposureMode exposureMode);

/// \brief Check if setting a specific focus mode is supported on the current device
/**
 * \note This function can only be called while Vuforia Engine is running
 *
 * \note When this function returns VU_FALSE in 'isFocusModeSupported' calls to setting the respective focus mode will fail.
 *
 * \returns VU_SUCCESS on success or VU_FAILED if Vuforia Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerIsFocusModeSupported(VuController* controller, VuCameraFocusMode focusMode,
                                                                   VuBool* isFocusModeSupported);

/// \brief Check if setting a specific exposure mode is supported on the current device
/**
 * \note This function can only be called while Vuforia Engine is running
 *
 * \note When this function returns VU_FALSE in 'isExposureModeSupported' calls to setting the respective exposure mode will fail.
 *
 * \returns VU_SUCCESS on success or VU_FAILED if Vuforia Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerIsExposureModeSupported(VuController* controller, VuCameraExposureMode exposureMode,
                                                                      VuBool* isExposureModeSupported);

/// \brief Get the region of interest currently active for camera focus control
/**
 * \note This function can only be called while Vuforia Engine is running
 *
 * \returns VU_SUCCESS on success or VU_FAILED if this operation is not supported on the current device or
 * Vuforia Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerGetFocusRegion(const VuController* controller, VuCameraRegionOfInterest* focusROI);

/// \brief Set the active region of interest for camera focus control
/**
 * \note This will replace any previously set region
 *
 * \note This function can only be called while Vuforia Engine is running
 *
 * \note To restore the focus settings to the state before explicitly setting any focus region pass in a \ref VuCameraRegionOfInterest
 * with center = (0.5f, 0.5f) and extent = 1.0f
 *
 * \note Setting the focus region is not supported on all platforms and fusion providers,
 * see \ref vuCameraControllerIsFocusRegionSupported.
 *
 * \note If the region defined through \ref VuCameraRegionOfInterest exceeds the bounds of the camera frame it will be clamped.
 * The center point will also be adjusted accordingly to the centre of the clamped region. Example: Setting a region of
 * center = (1.0f, 1.0f) and extent = 1.0f will result in a clamped region of center = (0.75f, 0.75f) and extent = 0.5f.
 *
 * \note The focus region is NOT retained across Vuforia Engine stop/start: for instance when an App is paused and then resumed,
 * it will revert back to the default focus region.
 *
 * \returns VU_SUCCESS on success or VU_FAILED if this operation is not supported on the current device or if the specified region is not in
 * a valid range, or Vuforia Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerSetFocusRegion(VuController* controller, VuCameraRegionOfInterest focusROI);

/// \brief Get the region of interest currently active for camera exposure control
/**
 * \note This function can only be called while Vuforia Engine is running
 *
 * \returns VU_SUCCESS on success or VU_FAILED if this operation is not supported on the current device or
 * Vuforia Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerGetExposureRegion(const VuController* controller, VuCameraRegionOfInterest* exposureROI);

/// \brief Set the active region of interest for camera exposure control
/**
 * \note This will replace any previously set region
 *
 * \note This function can only be called while Vuforia Engine is running
 *
 * \note To restore the exposure settings to the state before explicitly setting any focus region pass in a \ref VuCameraRegionOfInterest
 * with center = (0.5f, 0.5f) and extent = 1.0f
 *
 * \note Setting the exposure region is not supported on all platforms and fusion providers,
 * see \ref vuCameraControllerIsExposureRegionSupported.
 *
 * \note If the region defined through \ref VuCameraRegionOfInterest exceeds the bounds of the camera frame it will be clamped.
 * The center point will also be adjusted accordingly to the centre of the clamped region. Example: Setting a region of
 * center = (1.0f, 1.0f) and extent = 1.0f will result in a clamped region of center = (0.75f, 0.75f) and extent = 0.5f.
 *
 * \note The exposure region is NOT retained across Vuforia Engine stop/start: for instance when an App is paused and then resumed,
 * it will revert back to the default exposure region.
 *
 * \returns VU_SUCCESS on success or VU_FAILED if this operation is not supported on the current device or if the specified region is not in
 * a valid range, or Vuforia Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerSetExposureRegion(VuController* controller, VuCameraRegionOfInterest exposureROI);

/// \brief Check if setting a focus region is supported on the current device
/**
 * \note This function can only be called while Vuforia Engine is running
 *
 * \note When this function returns VU_FALSE in 'isFocusRegionSupported' calls to getting and setting the
 * focus region will fail.
 *
 * \returns VU_SUCCESS on success or VU_FAILED if Vuforia Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerIsFocusRegionSupported(const VuController* controller, VuBool* isFocusRegionSupported);

/// \brief Check if setting an exposure region is supported on the current device
/**
 * \note This function can only be called while Vuforia Engine is running
 *
 * \note When this function returns VU_FALSE in 'isExposureRegionSupported' calls to getting and setting the
 * exposure region will fail.
 *
 * \returns VU_SUCCESS on success or VU_FAILED if Vuforia Engine is not running
 */
VU_API VuResult VU_API_CALL vuCameraControllerIsExposureRegionSupported(const VuController* controller, VuBool* isExposureRegionSupported);

/// \brief Get list of image formats registered to be returned with the camera frame
VU_API VuResult VU_API_CALL vuCameraControllerGetRegisteredImageFormats(const VuController* controller, VuImagePixelFormatList* list);

/// \brief Register a video pixel format to be delivered in the list of images of a camera frame
/**
 * For a registered video pixel format an image will be converted from the native video pixel format
 * of the camera on each state update and delivered in the list of images of the \ref VuCameraFrame. The
 * list of images can be queried via \ref vuCameraFrameGetImages.
 *
 * By default only the image with the native video pixel format of the camera will be delivered. The native
 * video pixel format can be queried via \ref vuCameraControllerGetVideoModes in field \ref
 * VuCameraVideoMode#format.
 *
 * \note Please note that not all video pixel formats can be registered as the supported conversions depend
 * on the native pixel format. Registration will fail if the conversion from the native pixel format to the
 * requested video pixel format is not supported.
 *
 * \note This function supports only the registration of video pixel formats. Registration of depth pixel
 * formats (see \ref VuImagePixelFormat) will fail. Please refer to \ref vuCameraControllerEnableDepth
 * for enabling the delivery of depth frames.
 *
 * \note This function can only be called while Engine is running.
 *
 * \note Will fail if the given video pixel format is already registered.
 *
 * \see vuCameraControllerUnregisterImageFormat
 */
VU_API VuResult VU_API_CALL vuCameraControllerRegisterImageFormat(VuController* controller, VuImagePixelFormat format);

/// \brief Unregister a video pixel format from being delivered in the list of images of a camera frame
/**
 * Unregistering a video pixel format will stop Engine from delivering (and potentially converting to) an
 * image of the given video pixel format on each state update in the list of images of the \ref VuCameraFrame.
 *
 * \note This function supports only the deregistration of video pixel formats. Deregistration of depth pixel
 * formats (see \ref VuImagePixelFormat) will fail. Please refer to \ref vuCameraControllerDisableDepth for
 * disabling the delivery of depth frames.
 *
 * \note This function can only be called while Engine is running.
 *
 * \note Will fail if the given video pixel format is not registered.
 *
 * \see vuCameraControllerRegisterImageFormat
 */
VU_API VuResult VU_API_CALL vuCameraControllerUnregisterImageFormat(VuController* controller, VuImagePixelFormat format);

/// \brief Check if depth frames are supported on this device
/**
 * \note This function is part of an API in beta and may change from release-to-release without notice.
 */
VU_API VuResult VU_API_CALL vuCameraControllerIsDepthSupported(const VuController* controller, VuBool* supported);

/// \brief Enable depth frames
/**
 * \note This will fail if depth frames are not supported on this device (see \ref vuCameraControllerIsDepthSupported)
 * or depth frames have already been enabled (see \ref vuCameraControllerIsDepthEnabled).
 *
 * \note Enabling depth frames may impact the performance of your App and may also increase the power consumption
 * of your device.
 *
 * \note The depth frames setting is retained across Vuforia Engine stop/start, for instance when an App is paused
 * and then resumed.
 *
 * \parblock
 * \note This function is part of a feature in beta and may change from release to release without notice.
 * \endparblock
 *
 * \return VU_SUCCESS if call was successful, otherwise VU_FAILED
 */
VU_API VuResult VU_API_CALL vuCameraControllerEnableDepth(VuController* controller);

/// \brief Disable depth frames
/**
 * \note This will fail if depth frames are not currently enabled (see \ref vuCameraControllerIsDepthEnabled).
 *
 * \parblock
 * \note This function is part of a feature in beta and may change from release to release without notice.
 * \endparblock
 *
 * \return VU_SUCCESS if call was successful, otherwise VU_FAILED
 */
VU_API VuResult VU_API_CALL vuCameraControllerDisableDepth(VuController* controller);

/// \brief Check if depth frames are currently enabled
/**
 * \note This function is part of a feature in beta and may change from release to release without notice.
 */
VU_API VuBool VU_API_CALL vuCameraControllerIsDepthEnabled(const VuController* controller);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_CAMERACONTROLLER_H_
