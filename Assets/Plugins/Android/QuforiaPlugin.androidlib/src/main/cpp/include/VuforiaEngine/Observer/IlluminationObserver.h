/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_ILLUMINATIONOBSERVER_H_
#define _VU_ILLUMINATIONOBSERVER_H_

/**
 * \file IlluminationObserver.h
 * \brief Observer for illumination information
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup IlluminationObserverGroup Illumination Feature
 * \{
 */

/// \brief Configuration error for Illumination observer creation
VU_ENUM(VuIlluminationCreationError)
{
    VU_ILLUMINATION_CREATION_ERROR_NONE = 0x0,                 ///< No error
    VU_ILLUMINATION_CREATION_ERROR_INTERNAL = 0x1,             ///< An internal error occurred while creating the observer
    VU_ILLUMINATION_CREATION_ERROR_AUTOACTIVATION_FAILED = 0x2 ///< An error occurred while auto-activating the observer
};

/// \brief Configuration for illumination observer creation
typedef struct VuIlluminationConfig
{
    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;
} VuIlluminationConfig;

/// \brief Default illumination observer configuration
/**
 * \note Use this function to initialize the VuIlluminationConfig data structure with default values.
 */
VU_API VuIlluminationConfig VU_API_CALL vuIlluminationConfigDefault();

/*! \var VU_ILLUMINATION_AMBIENT_INTENSITY_UNAVAILABLE VU_ILLUMINATION_AMBIENT_INTENSITY_UNAVAILABLE
 * \brief Value used in the VuIlluminationObservationInfo data structure to indicate that scene ambient intensity is not supported on the
 * current platform
 */
VU_CONST_INT(VU_ILLUMINATION_AMBIENT_INTENSITY_UNAVAILABLE, -1);

/*! \var VU_ILLUMINATION_AMBIENT_COLOR_TEMPERATURE_UNAVAILABLE VU_ILLUMINATION_AMBIENT_COLOR_TEMPERATURE_UNAVAILABLE
 * \brief Value used in the VuIlluminationObservationInfo data structure to indicate that scene ambient color temperature is not supported
 * on the current platform
 */
VU_CONST_INT(VU_ILLUMINATION_AMBIENT_COLOR_TEMPERATURE_UNAVAILABLE, -1);

/// \brief Illumination information for rendering
typedef struct VuIlluminationObservationInfo
{
    /// Scene ambient intensity in lumen
    /**
     * Set to VU_ILLUMINATION_AMBIENT_INTENSITY_UNAVAILABLE if not supported on the current platform
     */
    int32_t ambientIntensity;

    /// Scene ambient color temperature in Kelvin
    /**
     * Set to VU_ILLUMINATION_AMBIENT_COLOR_TEMPERATURE_UNAVAILABLE if not supported on the current platform
     */
    int32_t ambientColorTemperature;

    /// Scene intensity correction values
    /**
     * A floating point intensity value which can be applied to a
     * shader when rendering the color for an augmentation to reflect the
     * ambient light in the scene.
     *
     * Intensity correction usage is described here:
     * https://library.vuforia.com/content/vuforia-library/en/articles/Solution/using-vuforia-fusion-illumination.html
     *
     * Values are in the range (0.0, 1.0), zero is black and 1.0 is white.
     *
     * If rendering in gamma space divide by 0.466 (middle grey in gamma) and multiply
     * by the final rendered color.
     *
     * In a linear color space use pow(intensityCorrection, 2.2)/0.18
     * pow(intensityCorrection, 2.2) converts to linear space and then the value is
     * normalized by dividing by 0.18 middle grey in linear space.
     *
     * The value will always be valid for use. 0.466 (middle grey) is used in cases where
     * the platform does not supply a value.
     */
    float intensityCorrection;

    /// Scene color correction values
    /**
     * A Vector4 which contains RGBA color correction
     * values which can be applied to a shader when rendering the color for an
     * augmentation to reflect the ambient light in the scene.
     *
     * Color correction usage is described here:
     * https://library.vuforia.com/content/vuforia-library/en/articles/Solution/using-vuforia-fusion-illumination.html
     *
     * All values are reported in gamma space where gamma is 2.2.
     *
     * When used in a gamma space component-wise multiply the values with the final
     * calculated color.
     *
     * In a linear color space convert to linear using pow(colorCorrection[i], 2.2) and
     * then component-wise multiply the values with the final calculated color component.
     *
     * The values will always be valid for use. 1.0 is used in cases where
     * the platform does not supply values.
     */
    VuVector4F colorCorrection;
} VuIlluminationObservationInfo;

/*! \var VU_OBSERVER_ILLUMINATION_TYPE VU_OBSERVER_ILLUMINATION_TYPE
 * \brief Type identifier for illumination observers
 */
VU_CONST_INT(VU_OBSERVER_ILLUMINATION_TYPE, 0xB);

/*! \var VU_OBSERVATION_ILLUMINATION_TYPE VU_OBSERVATION_ILLUMINATION_TYPE
 * \brief Type identifier for illumination observations
 */
VU_CONST_INT(VU_OBSERVATION_ILLUMINATION_TYPE, 0xB);

/// \brief Create an illumination observer
VU_API VuResult VU_API_CALL vuEngineCreateIlluminationObserver(VuEngine* engine, VuObserver** observer, const VuIlluminationConfig* config,
                                                               VuIlluminationCreationError* errorCode);

/// \brief Get all target illumination observers
VU_API VuResult VU_API_CALL vuEngineGetIlluminationObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Get all illumination observations
VU_API VuResult VU_API_CALL vuStateGetIlluminationObservations(const VuState* state, VuObservationList* observationList);

/// \brief Get info associated with an illumination observation
VU_API VuResult VU_API_CALL vuIlluminationObservationGetInfo(const VuObservation* observation,
                                                             VuIlluminationObservationInfo* illuminationInfo);

/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_ILLUMINATIONOBSERVER_H_
