/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_DEVICECALIBRATIONCONFIG_H_
#define _VU_DEVICECALIBRATIONCONFIG_H_

/**
 * \file DeviceCalibrationConfig.h
 * \brief Device calibration-specific configuration data for the Vuforia Engine
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \ingroup EngineDeviceCalibrationConfigGroup
 * \{
 */

/// \brief Device calibration configuration data structure
typedef struct VuDeviceCalibrationConfig
{
    ///\brief Control the way the latest device calibration is fetched from the server during engine initialization
    /**
     * VU_TRUE  = fetch asynchronously without blocking the initialization call
     * VU_FALSE = fetch synchronously while blocking the initialization call, causing it to take longer but guaranteeing latest calibration
     * (default value) By default the mode is set to VU_FALSE.
     */
    VuBool asyncFetchLatestCalibration;
} VuDeviceCalibrationConfig;

/// \brief Default device calibration configuration
/**
 * \note Use this function to initialize the VuDeviceCalibrationConfig data structure with default values.
 */
VU_API VuDeviceCalibrationConfig VU_API_CALL vuDeviceCalibrationConfigDefault();

/// \brief Add a device calibration configuration to the engine config
VU_API VuResult VU_API_CALL vuEngineConfigSetAddDeviceCalibrationConfig(VuEngineConfigSet* configSet,
                                                                        const VuDeviceCalibrationConfig* config);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_DEVICECALIBRATIONCONFIG_H_
