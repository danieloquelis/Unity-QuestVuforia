/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_LICENSECONFIG_H_
#define _VU_LICENSECONFIG_H_

/**
 * \file LicenseConfig.h
 * \brief License configuration data for the Vuforia Engine
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup EngineLicenseConfigGroup
 * \{
 */

/// \brief License configuration error code type for errors occurring when creating a Vuforia Engine instance
/**
 * \note The error code is reported via the \p errorCode parameter of the vuEngineCreate() function if an error
 * related to applying the license configuration occurs while initializing the new Engine instance.
 *
 * \note The Vuforia Engine checks during the creation of the Engine instance whether the license key is valid
 * both with a synchronous and an asynchronous mechanism. If the synchronous mechanism finds a license-related
 * error, then vuEngineCreate() will fail and report one of the VuLicenseConfigError error codes. If only the
 * asynchronous mechanism reports a license-related error, then Engine initialization will finish without a
 * license-related error, but Engine will stop soon afterwards, and any subsequent attempt to call vuEngineStart()
 * will fail.
 */
VU_ENUM(VuLicenseConfigError)
{
    VU_ENGINE_CREATION_ERROR_LICENSE_CONFIG_MISSING_KEY = 0x200,          ///< License key is missing
    VU_ENGINE_CREATION_ERROR_LICENSE_CONFIG_INVALID_KEY = 0x201,          ///< Invalid license key passed to SDK
    VU_ENGINE_CREATION_ERROR_LICENSE_CONFIG_NO_NETWORK_PERMANENT = 0x202, ///< Unable to verify license key due to network (Permanent error)
    VU_ENGINE_CREATION_ERROR_LICENSE_CONFIG_NO_NETWORK_TRANSIENT = 0x203, ///< Unable to verify license key due to network (Transient error)
    VU_ENGINE_CREATION_ERROR_LICENSE_CONFIG_BAD_REQUEST =
        0x204, ///< Malformed request sent to license server. Please ensure your app has valid name and version fields
    VU_ENGINE_CREATION_ERROR_LICENSE_CONFIG_KEY_CANCELED = 0x205,          ///< Provided key is no longer valid
    VU_ENGINE_CREATION_ERROR_LICENSE_CONFIG_PRODUCT_TYPE_MISMATCH = 0x206, ///< Provided key is not valid for current product
    VU_ENGINE_CREATION_ERROR_LICENSE_CONFIG_UNKNOWN = 0x207                ///< Unknown error
};

/// \brief License configuration data structure
typedef struct VuLicenseConfig
{
    /// \brief License key
    /**
     * \note The key shall not be a null pointer.
     */
    const char* key;
} VuLicenseConfig;

/// \brief Default license configuration
/**
 * \note Use this function to initialize the VuLicenseConfig data structure with default values.
 */
VU_API VuLicenseConfig VU_API_CALL vuLicenseConfigDefault();

/// \brief Add a license configuration to the engine configuration
VU_API VuResult VU_API_CALL vuEngineConfigSetAddLicenseConfig(VuEngineConfigSet* configSet, const VuLicenseConfig* config);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_LICENSECONFIG_H_
