/*===============================================================================
Copyright (c) 2024 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_PLATFORMCONTROLLER_ANDROID_H_
#define _VU_PLATFORMCONTROLLER_ANDROID_H_

/**
 * \file PlatformController_Android.h
 * \brief Android-specific functionality for the Vuforia Engine
 */

#include <VuforiaEngine/Core/Basic.h>
#include <VuforiaEngine/Core/System.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup PlatformAndroidControllerGroup Android-specific Platform Controller
 * \ingroup PlatformControllerGroup
 * \{
 * Android platform-specific functionality accessed via the PlatformController
 */

/// \brief ARCore-specific info for the platform-based Vuforia Fusion Provider
/**
 * \note The pointers contained in this data structure are owned by Vuforia Engine and should
 * be used with caution by the developer. For example do not release the session, do not pause
 * the session, do not reconfigure it, doing so will cause Vuforia Engine's handling of the information
 * from the provider to fail in undefined ways.
 *
 * Valid values for the pointers will be available only after Vuforia Engine has been started and
 * the Vuforia State contains camera frame data.
 *
 * The values for the pointers will remain valid until Vuforia Engine is stopped, either by calling
 * \ref vuEngineStop explicitly or when an asynchronous life-cycle error is reported via the
 * \ref VuErrorHandler callback with error codes \ref VU_ENGINE_ERROR_INVALID_LICENSE and
 * \ref VU_ENGINE_ERROR_CAMERA_DEVICE_LOST.
 *
 * On receiving a \ref VuErrorHandler callback with either of the errors \ref VU_ENGINE_ERROR_INVALID_LICENSE
 * and \ref VU_ENGINE_ERROR_CAMERA_DEVICE_LOST, the pointers may already be invalid inside the callback.
 * The App must therefore not make use of the pointers inside the callback, and return the control to
 * Vuforia Engine without delay. The pointers can be re-requested after Vuforia Engine has been
 * (re-)started.
 *
 * On ARCore the pointers might additionally be invalidated without Vuforia Engine being stopped.
 * This will be reported through the error code \ref VU_ENGINE_ERROR_PLATFORM_FUSION_PROVIDER_INFO_INVALIDATED
 * of the \ref VuErrorHandler callback.
 *
 * Users are advised to always register for the \ref VuErrorHandler via the \ref VuErrorHandlerConfig
 * when using the Fusion Provider pointers and handle potential asynchronous invalidation of these pointers
 * appropriately.
 */
typedef struct VuPlatformARCoreInfo
{
    /// \brief ARCore session, pointer of type "ArSession"
    /**
     * The caller needs to cast the arSession pointer to the appropriate type as follows:
     * ArSession* session = static_cast<ArSession*>(info.arSession);
     */
    void* arSession;

    /// \brief ARCore frame, pointer of type "ArFrame"
    /**
     * The caller needs to cast the arFrame pointer to the appropriate type as follows:
     * ArFrame* frame = static_cast<ArFrame*>(info.arFrame);
     *
     * Do not update the ArSession to get the ArFrame, doing so will cause
     * Vuforia Engine to enter an undefined state.
     */
    void* arFrame;
} VuPlatformARCoreInfo;

/// \brief Get information about the ARCore Fusion Provider Platform
/**
 * The information contained in the returned struct can be used to allow applications to interact with
 * the underlying ARCore session to gain access to functionality not currently available through the
 * Vuforia API. For example additional lighting information or plane boundaries.
 *
 * \note Call this function after Vuforia Engine has been started and the Vuforia State
 * contains a camera frame.
 * \note If a \ref VuErrorHandler callback (passed via \ref VuErrorHandlerConfig) has been received
 * with the error code \ref VU_ENGINE_ERROR_PLATFORM_FUSION_PROVIDER_INFO_INVALIDATED, the App must
 * poll this API until it successfully returns the new ARCore pointers. However, this polling must not
 * be done inside the \ref VuErrorHandler callback itself, but rather after the callback returns (i.e.
 * the control is passed back to Vuforia Engine).
 *
 * \param controller Platform controller retrieved from Engine (see \ref vuEngineGetPlatformController)
 * \param arCoreInfo ARCore-specific info for the platform-based Vuforia Fusion Provider
 * \returns VU_FAILED if Vuforia is not running, is not using the ARCore Fusion Provider Platform,
 * or if the ARCore pointers are not ready to be retrieved yet, VU_SUCCESS otherwise
 */
VU_API VuResult VU_API_CALL vuPlatformControllerGetARCoreInfo(const VuController* controller, VuPlatformARCoreInfo* arCoreInfo);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_PLATFORMCONTROLLER_ANDROID_H_
