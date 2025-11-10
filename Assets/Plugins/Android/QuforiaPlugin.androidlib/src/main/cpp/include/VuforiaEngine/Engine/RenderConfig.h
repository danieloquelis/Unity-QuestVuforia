/*===============================================================================
Copyright (c) 2024 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_RENDERCONFIG_H_
#define _VU_RENDERCONFIG_H_

/**
 * \file RenderConfig.h
 * \brief Rendering-specific configuration data for the Vuforia Engine
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup EngineRenderConfigGroup
 * \{
 */

/// \brief Rendering configuration error code type for errors occurring when creating a Vuforia Engine instance
/**
 * \note The error code is reported via the \p errorCode parameter of the vuEngineCreate() function if an error
 * related to the rendering configuration occurs while initializing the new Engine instance.
 */
VU_ENUM(VuRenderConfigError)
{
    VU_ENGINE_CREATION_ERROR_RENDER_CONFIG_UNSUPPORTED_BACKEND = 0x300,            ///< Unsupported render backend
    VU_ENGINE_CREATION_ERROR_RENDER_CONFIG_FAILED_TO_SET_VIDEO_BG_VIEWPORT = 0x301 ///< Failed to set video background viewport.
                                                                                   ///< This is currently never reported.
};


/// \brief Render video background backend configuration
VU_ENUM(VuRenderVBBackendType)
{
    VU_RENDER_VB_BACKEND_DEFAULT = 0x1,  ///< Select default rendering backend for each platform. Currently:
                                         ///< Android: OpenGL ES 3.x
                                         ///< iOS: Metal
                                         ///< UWP: DirectX 11
                                         ///< This is the default video background configuration.
    VU_RENDER_VB_BACKEND_HEADLESS = 0x2, ///< Deactivate usage of video background rendering support. Supported on
                                         ///< all platforms.
    VU_RENDER_VB_BACKEND_GLES3 = 0x4,    ///< OpenGL ES 3.x. Supported on Android and iOS.
    VU_RENDER_VB_BACKEND_DX11 = 0x5,     ///< DirectX 11. Supported on UWP.
    VU_RENDER_VB_BACKEND_METAL = 0x6     ///< Metal. Supported on iOS.
};

/// \brief Supported video background viewport modes
/**
 * If the aspect ratio and resolution of the native video frames (see \ref VuCameraVideoMode) and the render view (see \ref
 * VuRenderViewConfig) differ, Vuforia needs to know how it should adjust the video background image rendering with respect to the render
 * view. The different modes control if and how Vuforia should scale the video background image and adjust the video background viewport
 * inside the render view. The video background image will thereby always be centered.
 *
 * \note The aspect ratio of the video image is always preserved, only adjustments to the video background image scale and viewport are
 * applied.
 *
 * \note If the aspect ratio of the render view is the same as the aspect ratio of the native video then the modes \ref
 * VU_VIDEOBG_VIEWPORT_MODE_SCALE_TO_FILL and \ref VU_VIDEOBG_VIEWPORT_MODE_SCALE_TO_FIT will have the same result.
 *
 * \note If the aspect ratio of the render view is the same as the aspect ratio of the native video and additionally also the resolutions
 * are the same then all three modes will have the same result.
 */
VU_ENUM(VuVideoBackgroundViewportMode)
{
    VU_VIDEOBG_VIEWPORT_MODE_SCALE_TO_FILL = 0x1, ///< Scales the video background to fill the whole render view. This can crop the video
                                                  ///< background image (either top and bottom or left and right). This is the default.
    VU_VIDEOBG_VIEWPORT_MODE_SCALE_TO_FIT = 0x2,  ///< Scales the video background to show the full video image in the render view.
                                                  ///< The video background viewport is adjusted accordingly to the size of the video image
                                                  ///< in the render view which might show a letter box around the image.
    VU_VIDEOBG_VIEWPORT_MODE_NATIVE_VIDEO =
        0x3 ///< No scaling will be applied to the video background image, it has the same resolution as the
            ///< native video image. If the render view has a lower resolution than the native video, only the
            ///< fraction of the image that fits into the render view will be visible. If the render view resolution
            ///< is larger than the native video the whole video will be visible and the video background viewport
            ///< inside the render view will be adjusted accordingly to cover the video image.
};

/// \brief Render configuration data structure
typedef struct VuRenderConfig
{
    ///\brief Choice of video background rendering backend type
    /**
     * \note Selecting a video background render backend can only be done at Engine creation time. To select a different backend type you
     * need to destroy the Engine instance again and create a new instance with the desired backend type.
     *
     * \note Default value is \ref VU_RENDER_VB_BACKEND_DEFAULT which represents a different concrete render backend type depending on
     * the underlying platform. See \ref VuRenderVBBackendType for the supported backend types and their default values per platform.
     */
    VuRenderVBBackendType vbRenderBackend;

    ///\brief Configure the video background viewport mode
    /**
     * \note The video background viewport mode can be changed after Engine creation by calling \ref
     * vuRenderControllerSetVideoBackgroundViewportMode.
     *
     * \note The render view configuration is also used in the calculation of the viewport if no custom viewport has been set. See \ref
     * vuRenderControllerGetVideoBackgroundViewport for details.
     *
     * \note This setting will only be applied if Engine can setup default render view information on Engine creation, otherwise
     * Engine will continue to use the default value. You can use \ref vuRenderControllerGetRenderViewConfig after Engine creation to
     * determine if a default render view configuration was set. To ensure cross-platform that your desired \ref
     * VuVideoBackgroundViewportMode is applied use \ref vuRenderControllerSetVideoBackgroundViewportMode after Engine creation. See \ref
     * vuRenderControllerSetRenderViewConfig for more details.
     *
     * \warning The behaviour of the default render view configuration will change in an upcoming release. See \ref
     * vuRenderControllerSetRenderViewConfig for details.
     *
     * \note Default value is \ref VU_VIDEOBG_VIEWPORT_MODE_SCALE_TO_FILL
     */
    VuVideoBackgroundViewportMode vbViewportMode;
} VuRenderConfig;

/// \brief Default render configuration
/**
 * \note Use this function to initialize the VuRenderConfig data structure with default values.
 */
VU_API VuRenderConfig VU_API_CALL vuRenderConfigDefault();

/// \brief Add a render configuration to the engine config
VU_API VuResult VU_API_CALL vuEngineConfigSetAddRenderConfig(VuEngineConfigSet* configSet, const VuRenderConfig* config);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_RENDERCONFIG_H_
