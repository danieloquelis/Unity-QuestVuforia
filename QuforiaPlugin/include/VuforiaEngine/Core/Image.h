/*===============================================================================
Copyright (c) 2024 PTC Inc. and/or Its Subsidiary Companies. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_IMAGE_H_
#define _VU_IMAGE_H_

#include <VuforiaEngine/Core/Basic.h>
#include <VuforiaEngine/Core/System.h>

#include <stdint.h> // Required for standard types (int32_t, etc.)

#ifdef __cplusplus
extern "C"
{
#endif

/** \addtogroup ImageGroup Image Handling
 * \{
 */

// IMAGE PIXEL FORMAT SUPPORT

/// \brief Pixel format types supported by Vuforia
/**
 * \note Pixel format types containing FORMAT_DEPTH in the name are specific to depth frames,
 * while others, which do not contain FORMAT_DEPTH, define video camera image pixel formats.
 */
VU_ENUM(VuImagePixelFormat)
{
    VU_IMAGE_PIXEL_FORMAT_UNKNOWN = 0x1,                      ///< Unknown pixel format.
    VU_IMAGE_PIXEL_FORMAT_RGB565 = 0x2,                       ///< A color pixel stored in 2 bytes using 5 bits for red,
                                                              ///< 6 bits for green and 5 bits for blue.
    VU_IMAGE_PIXEL_FORMAT_RGB888 = 0x3,                       ///< A color pixel stored in 3 bytes using 8 bits each
                                                              ///< for red, green and blue.
    VU_IMAGE_PIXEL_FORMAT_GRAYSCALE = 0x4,                    ///< A grayscale pixel stored in one byte.
    VU_IMAGE_PIXEL_FORMAT_RGBA8888 = 0x5,                     ///< A color pixel stored in 4 bytes using 8 bits each
                                                              ///< for red, green and blue and 8 bits for alpha channel.
    VU_IMAGE_PIXEL_FORMAT_NV21 = 0x6,                         ///< YUV 4:2:0 with a plane of 8 bit Y (luma) samples followed by
                                                              ///< an interleaved plane of 8 bit, 2x2 subsampled, V/U (chroma) samples.
    VU_IMAGE_PIXEL_FORMAT_NV12 = 0x7,                         ///< YUV 4:2:0 with a plane of 8 bit Y (luma) samples followed by
                                                              ///< an interleaved plane of 8 bit, 2x2 subsampled, U/V (chroma) samples.
    VU_IMAGE_PIXEL_FORMAT_YV12 = 0x8,                         ///< YUV 4:2:0 with a plane of 8 bit Y (luma) samples followed by
                                                              ///< a plane of 8 bit, 2x2 subsampled, V (chroma) samples followed by
                                                              ///< a plane of 8 bit, 2x2 subsampled, U (chroma) samples.
    VU_IMAGE_PIXEL_FORMAT_YUV420P = 0x9,                      ///< YUV 4:2:0 with a plane of 8 bit Y (luma) samples followed by
                                                              ///< a plane of 8 bit, 2x2 subsampled, U (chroma) samples followed by
                                                              ///< a plane of 8 bit, 2x2 subsampled, V (chroma) samples.
                                                              ///< Note that this format is also known as I420
    VU_IMAGE_PIXEL_FORMAT_YUYV = 0xA,                         ///< YUV 4:2:2 with a single plane of interleaved 8 bit samples in YUYV order
                                                              ///< where each pixel has a Y value and U, V values 2x1 subsampled.
                                                              ///< Note that this format is also known as YUY2
    VU_IMAGE_PIXEL_FORMAT_DEPTH_METER_FLOAT32 = 0x101,        ///< A depth value in meters, stored in a 32-bit floating point value.
                                                              ///< \note This pixel format is part of a feature in beta and
                                                              ///< may change from release to release without notice.
    VU_IMAGE_PIXEL_FORMAT_DEPTH_CONFIDENCE_LMH_UINT8 = 0x201, ///< A depth confidence pixel stored in one byte that can have
                                                              ///< one of the following three discrete values:
                                                              ///<  - 0: low confidence
                                                              ///<  - 1: medium confidence
                                                              ///<  - 2: high confidence
                                                              ///<
                                                              ///< \note This pixel format is part of a feature in beta and
                                                              ///< may change from release to release without notice.
};

// IMAGE PIXEL FORMAT LIST SUPPORT

/// \brief List of image pixel formats
typedef struct VuImagePixelFormatList_ VuImagePixelFormatList;

/// \brief Create an image pixel format list
VU_API VuResult VU_API_CALL vuImagePixelFormatListCreate(VuImagePixelFormatList** list);

/// \brief Get number of elements in an image pixel format list
VU_API VuResult VU_API_CALL vuImagePixelFormatListGetSize(const VuImagePixelFormatList* list, int32_t* listSize);

/// \brief Get an element in an image pixel format list
VU_API VuResult VU_API_CALL vuImagePixelFormatListGetElement(const VuImagePixelFormatList* list, int32_t element,
                                                             VuImagePixelFormat* format);

/// \brief Destroy an image pixel format list
VU_API VuResult VU_API_CALL vuImagePixelFormatListDestroy(VuImagePixelFormatList* list);

// IMAGE SUPPORT

/// \brief Vuforia Image
typedef struct VuImage_ VuImage;

/// \brief Vuforia Image List
typedef struct VuImageList_ VuImageList;

/// \brief Data structure describing image data
typedef struct VuImageInfo
{
    /// \brief Width of the image in pixels
    int32_t width;

    /// \brief Height of the image in pixels
    int32_t height;

    /// \brief Stride of the image in bytes
    int32_t stride;

    /// \brief Buffer width of the image in pixels
    int32_t bufferWidth;

    /// \brief Buffer height of the image in pixels
    int32_t bufferHeight;

    /// \brief Buffer size of the image in bytes
    int32_t bufferSize;

    /// \brief Image pixel format
    VuImagePixelFormat format;

    /// \brief Pixel buffer
    /**
     * \note The lifetime of the buffer is bound to the lifetime of the VuImage that was
     * used to retrieve this data.
     */
    const void* buffer;
} VuImageInfo;

/// \brief Get Image data
VU_API VuResult VU_API_CALL vuImageGetImageInfo(const VuImage* image, VuImageInfo* imageInfo);

/// \brief Acquire a new reference to the given image
VU_API VuResult VU_API_CALL vuImageAcquireReference(const VuImage* image, VuImage** imageOut);

/// \brief Release the given image
VU_API VuResult VU_API_CALL vuImageRelease(VuImage* image);

// IMAGE LIST SUPPORT

/// \brief Create an image list
VU_API VuResult VU_API_CALL vuImageListCreate(VuImageList** list);

/// \brief Get number of elements in an image list
VU_API VuResult VU_API_CALL vuImageListGetSize(const VuImageList* list, int32_t* numElements);

/// \brief Get an element in an image list
VU_API VuResult VU_API_CALL vuImageListGetElement(const VuImageList* list, int32_t element, VuImage** image);

/// \brief Append an element to an image list
VU_API VuResult VU_API_CALL vuImageListAppendElement(VuImageList* list, const VuImage* image);

/// \brief Destroy an image list
VU_API VuResult VU_API_CALL vuImageListDestroy(VuImageList* list);

/** \} */


#ifdef __cplusplus
}
#endif

#endif // _VU_IMAGE_H_
