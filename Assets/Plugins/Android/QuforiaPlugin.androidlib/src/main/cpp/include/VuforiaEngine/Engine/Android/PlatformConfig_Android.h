/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_PLATFORMCONFIG_ANDROID_H_
#define _VU_PLATFORMCONFIG_ANDROID_H_

/**
 * \file PlatformConfig_Android.h
 * \brief Android-specific configuration for the Vuforia Engine
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup PlatformAndroidEngineConfigGroup Android-specific Engine Configuration
 * \ingroup EngineConfigGroup
 * \{
 */

/// \brief Android-specific configuration error code type for errors occurring when creating a Vuforia Engine instance
/**
 * \note The error code is reported via the \p errorCode parameter of the vuEngineCreate() function if an error
 * related to applying Android-specific configuration occurs while initializing the new Engine instance.
 */
VU_ENUM(VuPlatformAndroidConfigError)
{
    VU_ENGINE_CREATION_ERROR_PLATFORM_ANDROID_CONFIG_INITIALIZATION_ERROR =
        0x510, ///< An error occurred during initialization of the platform
    VU_ENGINE_CREATION_ERROR_PLATFORM_ANDROID_CONFIG_INVALID_ACTIVITY =
        0x511, ///< Invalid Android Activity jobject passed to the configuration
    VU_ENGINE_CREATION_ERROR_PLATFORM_ANDROID_CONFIG_INVALID_JAVA_VM = 0x512 ///< Invalid Java VM (JavaVM*) passed to the configuration
};

/// \brief Android-specific platform configuration data structure
typedef struct VuPlatformAndroidConfig
{
    /// \brief Pointer to the client app's Android Activity of type "jobject"
    void* activity;

    /// \brief Pointer to the client app's JVM of type "JavaVM*"
    void* javaVM;
} VuPlatformAndroidConfig;

/// \brief Default Android-specific configuration
VU_API VuPlatformAndroidConfig VU_API_CALL vuPlatformAndroidConfigDefault();

/// \brief Add an Android-specific configuration to the engine config
VU_API VuResult VU_API_CALL vuEngineConfigSetAddPlatformAndroidConfig(VuEngineConfigSet* configSet, const VuPlatformAndroidConfig* config);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_PLATFORMCONFIG_ANDROID_H_
