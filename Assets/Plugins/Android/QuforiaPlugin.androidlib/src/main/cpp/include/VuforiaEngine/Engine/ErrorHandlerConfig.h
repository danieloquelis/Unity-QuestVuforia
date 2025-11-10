/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_ERRORHANDLERCONFIG_H_
#define _VU_ERRORHANDLERCONFIG_H_

/**
 * \file ErrorHandlerConfig.h
 * \brief Configuration for handling errors that impact the Engine lifecycle and occur asynchronously after the Engine instance has been
 * created
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup EngineErrorHandlerConfigGroup
 * \{
 */

/// \brief Error codes for Engine lifecycle-related errors reported via the VuErrorHandler error handler function
VU_ENUM(VuEngineError)
{
    VU_ENGINE_ERROR_INVALID_LICENSE = 0x600,                           ///< License key validation has failed, Engine has stopped
    VU_ENGINE_ERROR_CAMERA_DEVICE_LOST = 0x601,                        ///< The operating system has reported that the camera device
                                                                       ///< has become unavailable to Vuforia and therefore Engine has
                                                                       ///< stopped. This may be because of a device error or another App or
                                                                       ///< user action has caused the operating system to close Engine's
                                                                       ///< connection to the camera.
    VU_ENGINE_ERROR_PLATFORM_FUSION_PROVIDER_INFO_INVALIDATED = 0x602, ///< This error can only happen on Android when using ARCore.
                                                                       ///< <p> If ARCore Fusion Provider info was retrieved previously via
                                                                       ///< \ref vuPlatformControllerGetARCoreInfo, this will be invalidated
                                                                       ///< (including all platform pointers) after the callback has
                                                                       ///< returned and should not be accessed any more after that (as this
                                                                       ///< might lead to undefined behavior). The info must be retrieved
                                                                       ///< again.
                                                                       ///<
                                                                       ///< \note All the ARCore entities previously created by using these
                                                                       ///< pointers directly (such as anchors, planes) will also become
                                                                       ///< invalid and so must be created again.
                                                                       ///<
                                                                       ///< \see The \ref VuPlatformARCoreInfo documentation.
};

/// \brief Handler function type to report an Engine lifecycle-related error asynchronously
typedef void(VU_API_CALL VuErrorHandler)(VuEngineError errorCode, void* clientData);

/// \brief Data structure to configure the handling of Engine lifecycle-related errors via a callback after the Engine instance has been
/// created
typedef struct VuErrorHandlerConfig
{
    /// \brief Error handler function to report Engine lifecycle-related errors
    /**
     * \note The parameter is ignored if set to NULL. In this case Engine does not have a way to notify
     * its client about errors after Engine instance creation. The default value is NULL.
     *
     * The parameter \p errorCode in the \p errorHandler function reports the code of an error,
     * which occurs after the Engine instance has been already created and has a fundamental impact on the Engine
     * lifecycle (e.g. Engine stopped internally due to receiving a report about an invalid license key or the
     * platform-specific camera resource dropped by the operating system). The thread on which Engine invokes
     * the handler function depends on the platform and the error (may or may not be the camera thread), there
     * is no harmonized, cross-platform behavior.
     *
     * See also the general documentation on callbacks and reentrancy in Engine.h.
     */
    VuErrorHandler* errorHandler;

    /// \brief Client data to pass back when the error handler function is called
    /**
     * \note Default value is NULL.
     */
    void* clientData;
} VuErrorHandlerConfig;

/// \brief Default error handler configuration
/**
 * \note Use this function to initialize the VuErrorHandlerConfig data structure with default values.
 */
VU_API VuErrorHandlerConfig VU_API_CALL vuErrorHandlerConfigDefault();

/// \brief Add error handler configuration to the engine configuration to handle errors that impact the Engine lifecycle and occur
/// asynchronously after the Engine instance has been created
VU_API VuResult VU_API_CALL vuEngineConfigSetAddErrorHandlerConfig(VuEngineConfigSet* configSet, const VuErrorHandlerConfig* config);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_ERRORHANDLERCONFIG_H_
