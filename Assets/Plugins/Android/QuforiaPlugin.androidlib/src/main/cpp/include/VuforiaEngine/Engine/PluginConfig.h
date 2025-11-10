/*===============================================================================
Copyright (c) 2024 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_PLUGINCONFIG_H_
#define _VU_PLUGINCONFIG_H_

/**
 * \file PluginConfig.h
 * \brief Plugin-specific configuration data for the Vuforia Engine
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup EnginePluginConfigGroup
 * \{
 */

/// \brief Plugin configuration data structure
typedef struct VuPluginConfig
{
    /// \brief Directory where to search for plugins
    /**
     * \note If no plugin directory is specified, plugins are searched in the directory containing the dataset when an observer is created
     * that requires a plugin.
     *
     * \note Use the "asset://" prefix to search plugins in the assets directory of the application, optionally followed by further
     * directory components.
     */
    const char* pluginDirectory;
} VuPluginConfig;

/// \brief Default plugin configuration
/**
 * \note Use this function to initialize the VuPluginConfig data structure with default values.
 */
VU_API VuPluginConfig VU_API_CALL vuPluginConfigDefault();

/// \brief Add a plugin configuration to the engine configuration
VU_API VuResult VU_API_CALL vuEngineConfigSetAddPluginConfig(VuEngineConfigSet* configSet, const VuPluginConfig* config);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_PLUGINCONFIG_H_
