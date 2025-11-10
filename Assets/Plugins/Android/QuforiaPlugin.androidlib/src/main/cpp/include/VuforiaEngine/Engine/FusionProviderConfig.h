/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_FUSIONPROVIDERCONFIG_H_
#define _VU_FUSIONPROVIDERCONFIG_H_

/**
 * \file FusionProviderConfig.h
 * \brief Vuforia Fusion Provider-specific configuration data for the Vuforia Engine
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup EngineFusionProviderConfigGroup
 * \{
 */

/// \brief Vuforia Fusion provider configuration data structure
typedef struct VuFusionProviderConfig
{
    ///\brief Enable / disable the usage of platform-based Vuforia fusion provider (if available). Default value is VU_TRUE.
    VuBool usePlatformFusionProvider;
} VuFusionProviderConfig;

/// \brief Default Vuforia Fusion provider configuration
/**
 * \note Use this function to initialize the VuFusionProviderConfig data structure with default values.
 */
VU_API VuFusionProviderConfig VU_API_CALL vuFusionProviderConfigDefault();

/// \brief Add a Vuforia Fusion provider configuration to the engine config
/**
 * \note If we cannot set the platform fusion provider, Engine will fall back to using a non-platform-based Vuforia fusion provider
 * You can query the Vuforia fusion provider actually used by Engine via vuPlatformControllerGetFusionProviderType.
 */
VU_API VuResult VU_API_CALL vuEngineConfigSetAddFusionProviderConfig(VuEngineConfigSet* configSet, const VuFusionProviderConfig* config);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_FUSIONPROVIDERCONFIG_H_
