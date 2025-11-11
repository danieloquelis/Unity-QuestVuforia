/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_DRIVERCONFIG_H_
#define _VU_DRIVERCONFIG_H_

/**
 * \file DriverConfig.h
 * \brief Vuforia Driver-specific configuration data for the Vuforia Engine
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup EngineDriverConfigGroup
 * \{
 */

/// \brief Vuforia Driver configuration error code when creating a Vuforia Engine instance
VU_ENUM(VuDriverConfigError)
{
    VU_ENGINE_CREATION_ERROR_DRIVER_CONFIG_LOAD_ERROR =
        0x100, ///< An error occurred while loading the driver (library not found or
               ///< could not be loaded due to missing entry points, incompatible ABI format, etc.)
    VU_ENGINE_CREATION_ERROR_DRIVER_CONFIG_FEATURE_NOT_SUPPORTED = 0x101 ///< Vuforia Driver is not supported by the current license
};

/// \brief Vuforia Driver configuration data structure
typedef struct VuDriverConfig
{
    /// \brief Vuforia Driver library name
    const char* driverName;

    /// \brief User client data that can be passed to your driver
    void* userData;
} VuDriverConfig;

/// \brief Default Vuforia Driver Platform configuration
/**
 * \note Use this function to initialize the VuDriverConfig data structure with default values.
 */
VU_API VuDriverConfig VU_API_CALL vuDriverConfigDefault();

/// \brief Add a Vuforia Driver configuration to the engine configuration
VU_API VuResult VU_API_CALL vuEngineConfigSetAddDriverConfig(VuEngineConfigSet* configSet, const VuDriverConfig* config);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_DRIVERCONFIG_H_
