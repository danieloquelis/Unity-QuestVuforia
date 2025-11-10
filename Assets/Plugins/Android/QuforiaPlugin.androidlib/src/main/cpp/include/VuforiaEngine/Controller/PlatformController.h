/*===============================================================================
Copyright (c) 2024 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_PLATFORMCONTROLLER_H_
#define _VU_PLATFORMCONTROLLER_H_

/**
 * \file PlatformController.h
 * \brief Controller to access platform-specific functionality in the Vuforia Engine
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup PlatformControllerGroup Platform Controller
 * \{
 * This controller allows the control of platform-specific features on certain platforms.
 * Check whether a platform-specific header called {PLATFORM_NAME}/Platfrom_{PLATFORM_NAME}.h exists
 * for the given platform, and look up options there. If no platform-specific header exists, then
 * the Vuforia Engine does not support any platform-specific functionality on the given platform.
 */

/// \brief The view orientation describes how the application is oriented, or how the user "views" the camera image
VU_ENUM(VuViewOrientation)
{
    VU_VIEW_ORIENTATION_LANDSCAPE_LEFT = 0x1,     ///< The app is viewed as wider than it is tall with the device rotated by 90
                                                  ///< degrees COUNTER - CLOCKWISE from regular portrait. Note that on iOS this
                                                  ///< corresponds to "Landscape Right"
    VU_VIEW_ORIENTATION_PORTRAIT = 0x2,           ///< The app is viewed as taller than it is wide
    VU_VIEW_ORIENTATION_LANDSCAPE_RIGHT = 0x3,    ///< The app is viewed in a landscape orientation BUT in the opposite direction
                                                  ///< from the landscape orientation as defined above. In other words, the device
                                                  ///< is rotated by 90 degrees CLOCKWISE from regular portrait. Note that on iOS
                                                  ///< this corresponds to "Landscape Left"
    VU_VIEW_ORIENTATION_PORTRAIT_UPSIDEDOWN = 0x4 ///< The app is viewed in a portrait orientation BUT in the opposite direction
                                                  ///< from the regular portrait orientation
};

/// \brief Vuforia Fusion provider types
VU_ENUM(VuFusionProviderType)
{
    VU_FUSION_PROVIDER_TYPE_UNKNOWN = 0x1,               ///< Fusion provider is unknown
    VU_FUSION_PROVIDER_TYPE_VISION_ONLY = 0x2,           ///< Use vision-based Fusion only
                                                         ///< Please note that not all Vuforia Engine features are
                                                         ///< supported when using this provider.
    VU_FUSION_PROVIDER_TYPE_SENSOR_FUSION = 0x3,         ///< Use Vuforia-provided technology for Fusion. Vuforia Engine will make
                                                         ///< use of camera and IMU information to try to create a tracking
                                                         ///< experience that, when compared to vision-only, is more robust
                                                         ///< to erratic motions and sparse or featureless environments.
    VU_FUSION_PROVIDER_TYPE_PLATFORM_SENSOR_FUSION = 0x4 ///< Use platform-provided technology for Fusion. Vuforia Engine
                                                         ///< will make use of tracking services offered by the underlying
                                                         ///< platform, such as ARKit on iOS, ARCore on Android, or
                                                         ///< Windows Holographic on UWP.
};

/// \brief Vuforia Fusion Provider Platform type
VU_ENUM(VuFusionProviderPlatformType)
{
    VU_FUSION_PROVIDER_PLATFORM_TYPE_UNKNOWN = 0x1,             ///< Provider platform is unknown or unavailable
    VU_FUSION_PROVIDER_PLATFORM_TYPE_ARKIT = 0x2,               ///< ARKit platform
    VU_FUSION_PROVIDER_PLATFORM_TYPE_ARCORE = 0x3,              ///< ARCore platform
    VU_FUSION_PROVIDER_PLATFORM_TYPE_WINDOWS_HOLOGRAPHIC = 0x4, ///< Windows Holographic platform
    VU_FUSION_PROVIDER_PLATFORM_TYPE_LUMIN = 0x5,               ///< Lumin platform
    VU_FUSION_PROVIDER_PLATFORM_TYPE_EXTERNAL = 0x6             ///< External (Vuforia Driver)
};

/// \brief Retrieve Platform Controller to get access to platform-specific functionality and settings in Engine
VU_API VuResult VU_API_CALL vuEngineGetPlatformController(const VuEngine* engine, VuController** controller);

/// \brief Set the current view orientation
/**
 * \note The view orientation must be set to be able to use Vuforia Engine; otherwise device poses may be incorrectly reported (the default
 * value is platform dependent). Even if an application is not doing any rendering (e.g. robotics, CV), it should still correctly set the
 * view orientation to match the way the device is oriented. Furthermore, Vuforia needs to be notified of any changes in orientation.
 * For more information on the platform-specific view orientation descriptors, please see the conversion utility
 * \ref vuPlatformControllerConvertPlatformViewOrientation described below.
 *
 * \note On Android and Lumin, Vuforia does internal view orientation consistency checking. Therefore calls to this API will fail on these
 * platforms if the provided view orientation does not align with the actual view orientation.
 *
 * \note On iOS and UWP the view orientation should be provided in the \ref VuEngineConfigSet passed to \ref vuEngineCreate via
 * \ref VuPlatformiOSConfig or \ref VuPlatformUWPConfig respectively.
 *
 * \note The view orientation should be consistent with the width and height set in \ref vuRenderControllerSetRenderViewConfig. See
 * \ref vuRenderControllerSetRenderViewConfig for more details on the default render view configuration and the requirements regarding
 * updating the render view configuration.
 *
 * \warning The behaviour of the default render view configuration will change in an upcoming release. See \ref
 * vuRenderControllerSetRenderViewConfig for details.
 *
 * \returns VU_SUCCESS if the view orientation has been successfully set, VU_FAILED otherwise
 */
VU_API VuResult VU_API_CALL vuPlatformControllerSetViewOrientation(VuController* controller, VuViewOrientation orientation);

/// \brief Utility function for converting a platform-specific orientation descriptor to a Vuforia view orientation value
/**
 * \param controller pointer to the Platform Controller
 *
 * \param platformOrientation must be of the following type on each respective platform (valid values are indicated in parentheses):
 *
 *    + Android: an array of 2 integer values where the first entry (array[0]) is the orientation value reported on an orientation change,
 *      i.e. config.orientation in onConfigurationChanged(Configuration config), and the second entry (array[1]) is the amount of rotation
 *      from the "natural" orientation of the device, which can be obtained by getWindowManager().getDefaultDisplay().getRotation(). This
 *      somewhat cumbersome set of parameters is required because the orientation value (the first entry) does not distinguish between the
 *      different landscape (left/right) and portrait (regular/upside down) orientations.
 *
 *    + iOS:     a UIInterfaceOrientation value (On Objective C: UIInterfaceOrientationPortrait, UIInterfaceOrientationPortraitUpsideDown,
 *      UIInterfaceOrientationLandscapeRight, UIInterfaceOrientationLandscapeLeft; On Swift: portrait, portraitUpsideDown, landscapeRight,
 *      landscapeLeft). Please note that, internally Vuforia relies on the corresponding entries having the same numeric value for all
 *      languages
 *
 *    + UWP:     a DisplayOrientations (Windows.Graphics.Display) value (Portrait, PortraitFlipped, Landscape, LandscapeFlipped). Depending
 *      on your build configuration, these enums will be defined in one of the following:
 *          - C++/CX:    Windows::Graphics::Display::DisplayOrientations
 *          - C++/WinRT: winrt::Windows::Graphics::Display::DisplayOrientations
 *          - Otherwise: ABI::Windows::Graphics::Display::DisplayOrientations (note that the enum entries bear the "DisplayOrientations_"
 *            prefix)
 *      Please note that, internally Vuforia relies on the corresponding entries having the same numeric value for all languages
 *
 * \param vuOrientation populated with the corresponding Vuforia view orientation value on success
 *
 * \returns VU_SUCCESS if the passed platform orientation indicates a valid (known) orientation, and can therefore be converted to a
 * valid Vuforia view orientation; VU_FAILED otherwise OR if this functionality is not supported on the current platform
 */
VU_API VuResult VU_API_CALL vuPlatformControllerConvertPlatformViewOrientation(const VuController* controller,
                                                                               const void* platformOrientation,
                                                                               VuViewOrientation* vuOrientation);

/// \brief Get the current fusion provider type
/**
 * \note If the \p fusionProvider parameter is reported to be VU_FUSION_PROVIDER_TYPE_PLATFORM_SENSOR_FUSION, then you
 * can call vuPlatformControllerGetFusionProviderPlatformType() to query the actual Vuforia Fusion provider's underlying platform's type.
 */
VU_API VuResult VU_API_CALL vuPlatformControllerGetFusionProviderType(const VuController* controller, VuFusionProviderType* fusionProvider);

/// \brief Get the current platform fusion provider type if Vuforia Fusion is using platform-provided technology
/**
 * \note The function vuPlatformControllerGetFusionProviderType() reports whether Vuforia Fusion is using platform-provided
 * technology by returning VU_FUSION_PROVIDER_TYPE_PLATFORM_SENSOR_FUSION as fusion provider. If another fusion provider is
 * reported, this call will fail.
 *
 * \returns VU_FAILED if not running on platform fusion provider, otherwise VU_SUCCESS
 */
VU_API VuResult VU_API_CALL vuPlatformControllerGetFusionProviderPlatformType(const VuController* controller,
                                                                              VuFusionProviderPlatformType* fusionProviderPlatformType);

/// \brief Retrieves the handle to the currently loaded Vuforia Driver library
/**
 * \note This function is only supported when a Vuforia Driver library was loaded during vuEngineCreate() and
 * vuPlatformControllerGetFusionProviderType() reports VU_FUSION_PROVIDER_PLATFORM_TYPE_EXTERNAL.
 *
 * \param controller pointer to the Platform Controller
 * \param handle will be populated with the native platform handle for the loaded library which can be used
 * for e.g. dlsym()/GetProcAddress() to load additional functions from the Driver.
 *
 * \returns VU_FAILED if no driver library is loaded, otherwise VU_SUCCESS
 */
VU_API VuResult VU_API_CALL vuPlatformControllerGetDriverLibraryHandle(const VuController* controller, void** handle);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_PLATFORMCONTROLLER_H_
