/*===============================================================================
Copyright (c) 2023 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_BARCODEOBSERVER_H_
#define _VU_BARCODEOBSERVER_H_

/**
 * \file BarcodeObserver.h
 * \brief Observer for the barcode feature
 */

#include <VuforiaEngine/Engine/Engine.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup BarcodeObserverGroup Barcode Feature
 * \{
 * An activated barcode observer will only output an observation when a barcode can be found
 * in the camera image.
 *
 * \note Only one observed barcode is reported in a barcode observation for a given frame, even
 * if multiple candidates can be seen by the camera in that frame. It is not guaranteed that the
 * same barcode is reported for consecutive frames under these circumstances.
 *
 * \note There may only be one barcode observer active at a time. Activating a barcode observer
 * while another one is already active will fail.
 */

/// \brief Configuration error for barcode creation
VU_ENUM(VuBarcodeCreationError)
{
    VU_BARCODE_CREATION_ERROR_NONE = 0x0,                ///< No error
    VU_BARCODE_CREATION_ERROR_INTERNAL = 0x1,            ///< An internal error occurred while creating the observer
    VU_BARCODE_CREATION_ERROR_OBSERVED_TYPES_EMPTY = 0x2 ///< The set of barcode types to observe is empty
};

/// \brief Type of barcode observed
VU_ENUM(VuBarcodeType)
{
    VU_BARCODE_TYPE_UPCA = 0x1,       ///< 1D barcode type, UPC-A
    VU_BARCODE_TYPE_UPCE = 0x2,       ///< 1D barcode type, UPC-E
    VU_BARCODE_TYPE_EAN8 = 0x3,       ///< 1D barcode type, EAN-8
    VU_BARCODE_TYPE_EAN13 = 0x4,      ///< 1D barcode type, EAN-13
    VU_BARCODE_TYPE_CODE39 = 0x5,     ///< 1D barcode type, CODE 39
    VU_BARCODE_TYPE_CODE93 = 0x6,     ///< 1D barcode type, CODE 93
    VU_BARCODE_TYPE_CODE128 = 0x7,    ///< 1D barcode type, CODE 128
    VU_BARCODE_TYPE_CODABAR = 0x8,    ///< 1D barcode type, Codabar
    VU_BARCODE_TYPE_ITF = 0x9,        ///< 1D barcode type, ITF
    VU_BARCODE_TYPE_QRCODE = 0xA,     ///< 2D barcode type, QR Code
    VU_BARCODE_TYPE_DATAMATRIX = 0xB, ///< 2D barcode type, Data Matrix
    VU_BARCODE_TYPE_AZTEC = 0xC,      ///< 2D barcode type, Aztec
    VU_BARCODE_TYPE_PDF417 = 0xD,     ///< 2D barcode type, PDF417
    VU_BARCODE_TYPE_MICROQRCODE = 0xE ///< 2D barcode type, Micro QR Code
};

/// \brief VuBarcodeType set
typedef struct VuBarcodeTypeSet_ VuBarcodeTypeSet;

/// \brief Barcode detection mode
VU_ENUM(VuBarcodeDetectionMode)
{
    VU_BARCODE_DETECTION_MODE_SINGLE = 0x1,   ///< The observer detects only a single barcode at a time. This mode
                                              ///< works best in scenarios where only one barcode is visible.
    VU_BARCODE_DETECTION_MODE_MULTIPLE = 0x2, ///< The observer detects multiple barcodes in parallel and produces
                                              ///< a separate obseration for each barcode found in the frame.
};

/// \brief Configuration for creating a barcode observer
typedef struct VuBarcodeConfig
{
    /// \brief Set of types to observe
    /**
     * A barcode observer has to be configured with the types it should observe.
     * You can provide NULL to indicate that all supported types should be observed.
     * An empty list will create an error as there should be at least one barcode
     * type to be observed by the observer.
     *
     * \note The default value is NULL.
     */
    const VuBarcodeTypeSet* observedTypes;

    /// \brief Barcode detection mode
    VuBarcodeDetectionMode detectionMode;

    /// \brief Observer activation
    /**
     * \note The default value is VU_TRUE.
     */
    VuBool activate;
} VuBarcodeConfig;

/// \brief Default barcode configuration
/**
 * \note Use this function to initialize the VuBarcodeConfig data structure with default values.
 */
VU_API VuBarcodeConfig VU_API_CALL vuBarcodeConfigDefault();

/// \brief Instance info for a barcode from its respective observation
typedef struct VuBarcodeObservationInstanceInfo
{
    /// \brief Barcode content buffer filled with length bytes representing the payload of the barcode.
    /**
     * Validity of the pointer is bound to the observation and therefore to the State.
     * \note Textual description, UTF-8 encoding.
     */
    const char* buffer;

    /// \brief Barcode content buffer size
    /**
     * Size of the buffer in bytes (payload).
     */
    int32_t length;

    /// \brief Barcode type (see VuBarcodeType)
    VuBarcodeType type;

    /// \brief Vertices of the barcode's bounds in camera image space (normalized coordinates)
    VuVector2F vertices[4];
} VuBarcodeObservationInstanceInfo;

/// \brief Info for a barcode from its respective observation
typedef struct VuBarcodeObservationInfo
{
    /// \brief A unique runtime ID for a given barcode
    /**
     * The runtime ID is assigned when the barcode is first detected, and is consistent while the Barcode
     * observer remains activated.
     *
     * \note The ID is unique within a Vuforia session. It is generated at runtime and is not persistent
     * across Vuforia sessions. The id here is currently for convenience only
     * An observation can also be uniquely identified through the type + text (payload) combination
     */
    int32_t id;
} VuBarcodeObservationInfo;

/*! \var VU_OBSERVER_BARCODE_TYPE VU_OBSERVER_BARCODE_TYPE
 * \brief Type identifier for barcode observers
 */
VU_CONST_INT(VU_OBSERVER_BARCODE_TYPE, 0x10);

/*! \var VU_OBSERVATION_BARCODE_TYPE VU_OBSERVATION_BARCODE_TYPE
 * \brief Type identifier for barcode observations
 */
VU_CONST_INT(VU_OBSERVATION_BARCODE_TYPE, 0x10);

/// \brief Create a barcode observer
/**
 * \note Only one barcode observer can be active at a time.
 */
VU_API VuResult VU_API_CALL vuEngineCreateBarcodeObserver(VuEngine* engine, VuObserver** observer, const VuBarcodeConfig* config,
                                                          VuBarcodeCreationError* error);

/// \brief Get all barcode observers
VU_API VuResult VU_API_CALL vuEngineGetBarcodeObservers(const VuEngine* engine, VuObserverList* observerList);

/// \brief Get the set of barcode types that will be observed by the observer
VU_API VuResult VU_API_CALL vuBarcodeObserverGetObservedTypes(const VuObserver* observer, VuBarcodeTypeSet* types);

/// \brief Create barcode type set
VU_API VuResult VU_API_CALL vuBarcodeTypeSetCreate(VuBarcodeTypeSet** list);

/// \brief Get number of elements in a barcode type set
VU_API VuResult VU_API_CALL vuBarcodeTypeSetGetSize(const VuBarcodeTypeSet* set, int32_t* setSize);

/// \brief Return VU_TRUE if an element is found in a barcode type set
VU_API VuResult VU_API_CALL vuBarcodeTypeSetHasElement(const VuBarcodeTypeSet* set, VuBarcodeType type, VuBool* result);

/// \brief Add an element to a barcode type set
/**
 * Nothing is done if the type is already in the set.
 */
VU_API VuResult VU_API_CALL vuBarcodeTypeSetAddElement(VuBarcodeTypeSet* set, VuBarcodeType type);

/// \brief Destroy a barcode type set
VU_API VuResult VU_API_CALL vuBarcodeTypeSetDestroy(VuBarcodeTypeSet* set);

/// \brief Get all barcode observations
VU_API VuResult VU_API_CALL vuStateGetBarcodeObservations(const VuState* state, VuObservationList* observationList);

/// \brief Get the barcode info associated with a Barcode observation
VU_API VuResult VU_API_CALL vuBarcodeObservationGetInfo(const VuObservation* observation, VuBarcodeObservationInfo* info);

/// \brief Get the barcode instance info associated with a Barcode observation
VU_API VuResult VU_API_CALL vuBarcodeObservationGetInstanceInfo(const VuObservation* observation,
                                                                VuBarcodeObservationInstanceInfo* instanceInfo);
/** \} */

#ifdef __cplusplus
}
#endif

#endif // _VU_BARCODEOBSERVER_H_
