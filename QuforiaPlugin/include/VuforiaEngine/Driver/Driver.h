/*===============================================================================
Copyright (c) 2024 PTC Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other
countries.
===============================================================================*/

#ifndef _VU_DRIVER_H_
#define _VU_DRIVER_H_

/**
 * \file Driver.h
 * \brief Header file for the Vuforia Driver
 */

#include <stdint.h>

#if defined(_MSC_VER)
#define VUFORIA_DRIVER_API_EXPORT         __declspec(dllexport)
#define VUFORIA_DRIVER_CALLING_CONVENTION __cdecl
#elif defined(__GNUC__) || defined(__clang__)
#if defined(__APPLE__)
#define VUFORIA_DRIVER_API_EXPORT __attribute__((visibility("default")))
#define VUFORIA_DRIVER_CALLING_CONVENTION
#else
#define VUFORIA_DRIVER_API_EXPORT
#define VUFORIA_DRIVER_CALLING_CONVENTION
#endif
#else
#error "Unsupported compiler"
#endif


#if defined(__ANDROID__)
#include <jni.h>
#endif

/**
 * \defgroup VuforiaDriverAPIGroup Vuforia Driver API
 * \{
 * \defgroup VuforiaDriverCameraGroup External Camera
 * \defgroup VuforiaDriverPDTGroup External Positional Device Tracker
 * \}
 */

namespace VuforiaDriver
{

/// \brief External provider API-version number that this header defines
/// \ingroup VuforiaDriverAPIGroup
const uint32_t VUFORIA_DRIVER_API_VERSION = 7;

// Platform-specific initialization data

#pragma pack(push, 1)
#if defined(__ANDROID__)

/// \brief Android-specific initialization data
/**
 * \ingroup VuforiaDriverAPIGroup
 * \note Data required to access Java functions through JNI
 */
struct PlatformData
{
    ///< Pointer to the current JVM
    JavaVM* javaVM{ nullptr };

    ///< Java object reference to current Activity.
    jobject activity{ nullptr };

    ///< JNI version number
    jint jniVersion{ -1 };
};
static_assert(sizeof(PlatformData) == (sizeof(void*) == 4 ? 12 : 20), "Unexpected size for the PlatformData struct");

#else

/// \brief Empty struct as a placeholder for platforms that don't have platform-specific initialization data
/// \ingroup VuforiaDriverAPIGroup
struct PlatformData
{
};

#endif
#pragma pack(pop)

/// \brief List of supported pixel formats for camera frames
/// \ingroup VuforiaDriverCameraGroup
enum class PixelFormat : int32_t
{
    UNKNOWN,  ///< Unknown format
    YUYV,     ///< YUV 4:2:2. Single 16-bit interleaved plane. Same as YUY2
    NV12,     ///< YUV 4:2:0. 8bit Y plane + 8-bit interleaved UV plane (subsampled 2x2)
    NV21,     ///< YUV 4:2:0. 8bit Y plane + 8-bit interleaved VU plane (subsampled 2x2)
    RGB888,   ///< RGB. 24-bits in 3 channels, 8 bits per channel
    RGBA8888, ///< RGB. 32-bits in 4 channels, 8 bits per channel
    YUV420P,  ///< YUV 4:2:0. 8-bit Y plane + 8-bit U plane (half resolution) + 8-bit V plane (half resolution)
    YV12,     ///< YUV 4:2:0. 8-bit Y plane + 8-bit V plane (half resolution) + 8-bit U plane (half resolution)
};

/// \brief Camera focus modes
/// \ingroup VuforiaDriverCameraGroup
enum class FocusMode : int32_t
{
    UNKNOWN,         ///< Unknown focus mode
    AUTO,            ///< Single trigger auto focus
    CONTINUOUS_AUTO, ///< Continuous auto focus
    MACRO,           ///< Macro mode
    INFINITY_FOCUS,  ///< Focus to infinity
    FIXED            ///< Fixed focus that can't be adjusted
};

/// \brief Camera exposure modes
/// \ingroup VuforiaDriverCameraGroup
enum class ExposureMode : int32_t
{
    UNKNOWN,         ///< Unknown exposure mode
    AUTO,            ///< Single trigger auto exposure
    CONTINUOUS_AUTO, ///< Continuous auto exposure
    MANUAL,          ///< Manual exposure mode
    SHUTTER_PRIORITY ///< Shutter priority mode
};

/// \brief Driver capabilities
/// \ingroup VuforiaDriverAPIGroup
enum Capability : uint32_t
{
    CAMERA_IMAGE = 0, ///< Driver supports camera Image(s)
    CAMERA_POSE = 1   ///< Driver supports camera pose(s)
};

/// \brief Enum values to describe the state of a camera pose
/// \ingroup VuforiaDriverPDTGroup
enum class PoseReason : int32_t
{
    INITIALIZING,          ///< The pose is still intializing
    VALID,                 ///< Valid pose
    EXCESSIVE_MOTION,      ///< The user is moving the device to fast
    INSUFFICIENT_FEATURES, ///< There are not enough features to provide a valid pose
    INSUFFICIENT_LIGHT,    ///< There is not enough light to provide a valid pose
    RELOCALIZING           ///< The system is relocalizing
};

/// \brief Pose is expected to be in the camera coordinate system
/// \ingroup VuforiaDriverPDTGroup
enum class PoseCoordSystem : int32_t
{
    CAMERA ///< Camera coordinate system.
};

/// \brief Enum values to indicate validity (quality) of a pose
/// \ingroup VuforiaDriverPDTGroup
enum class PoseValidity : int32_t
{
    VALID,     ///< The camera pose is valid
    UNRELIABLE ///< The camera pose is valid, but the quality is not guaranteed
};

/// \brief Enum values to describe anchor update reason
/// \ingroup VuforiaDriverPDTGroup
enum class AnchorStatus : int32_t
{
    ADDED,   ///< Anchor(s) added
    UPDATED, ///< Anchor(s) updated
    REMOVED, ///< Anchor(s) removed
    PAUSED   ///< Anchor(s) paused
};

/// \brief The names of Pose Fusion parameters that may be queried
/// \ingroup VuforiaDriverPDTGroup
enum PoseFusionParameter : int32_t
{
    POSE_FUSION_PARAMETER_RESERVED = 0 ///< Reserved for future use
};

#pragma pack(push, 1)

/// \brief Data structure describing the size, frame rate and format of a camera frame
/// \ingroup VuforiaDriverCameraGroup
struct CameraMode
{
    /// \brief Frame width
    uint32_t width{ 0 };

    /// \brief Frame height
    uint32_t height{ 0 };

    /// \brief Frame rate measured in frames per second
    uint32_t fps{ 0 };

    /// \brief Frame format
    PixelFormat format{ PixelFormat::YUYV };
};
static_assert(sizeof(CameraMode) == 16, "Unexpected size for the CameraMode struct");

/// \brief Properties required to support the intrinsics for a camera
/// \ingroup VuforiaDriverCameraGroup
/**
 * \note These values should be obtained from camera calibration.
 */
struct CameraIntrinsics
{
    /// Focal length x-component. 0.f if not available
    float focalLengthX{ 0.f };

    /// Focal length y-component. 0.f if not available
    float focalLengthY{ 0.f };

    /// Principal point x-component. 0.f if not available
    float principalPointX{ 0.f };

    /// Principal point y-component. 0.f if not available
    float principalPointY{ 0.f };

    /// An 8-element array of distortion coefficients
    /**
     * Array should be filled in the following order (r: radial, t:tangential):
     * [r0, r1, t0, t1, r2, r3, r4, r5]
     * Values that are not available should be set to 0.f.
     */
    float distortionCoefficients[8]{ 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
};
static_assert(sizeof(CameraIntrinsics) == 48, "Unexpected size for the CameraIntrinsics struct");

/// \brief Data structure describing a camera frame
/// \ingroup VuforiaDriverCameraGroup
struct CameraFrame
{
    /// \brief Frame timestamp at end of exposure in nanoseconds
    /**
     * \note The time base varies between the platforms.
     * On Android CLOCK_MONOTONIC is used, the current timestamp can be obtained with clock_gettime().
     */
    uint64_t timestamp{ 0 };

    /// \brief Exposure duration in nanoseconds
    uint64_t exposureTime{ 0 };

    /// \brief Pointer to first byte of the pixel buffer
    uint8_t* buffer{ nullptr };

    /// \brief Size of the pixel buffer in bytes
    uint32_t bufferSize{ 0 };

    /// \brief Frame index, ascending number
    uint32_t index{ 0 };

    /// \brief Frame width
    uint32_t width{ 0 };

    /// \brief Frame height
    uint32_t height{ 0 };

    /// \brief Stride value indicating how many bytes are used per row
    /**
     * \note If the frame is tightly packed this should equal to width * bytes per pixel.
     * For NV12 and NV21 images, this should be the stride of the Y plane in bytes and
     * the stride of the UV plane must be identical.
     * For YUV420p and YV12 images, this should be the stride of the Y plane in bytes.
     * The stride of the U and V planes must be exactly half of that.
     */
    uint32_t stride{ 0 };

    /// \brief Frame format
    PixelFormat format{ PixelFormat::YUYV };

    /// \brief Camera intrinsics used to capture the frame
    CameraIntrinsics intrinsics;
};
static_assert(sizeof(CameraFrame) == (44 + (sizeof(void*) == 4 ? 0 : 4) + sizeof(CameraIntrinsics)),
              "Unexpected size for the CameraFrame struct");

/// \brief Data structure describing a pose
/// \ingroup VuforiaDriverPDTGroup
struct Pose
{
    /// \brief Timestamp when pose was captured
    uint64_t timestamp{ 0 };

    /// \brief Array with translation vector values (x, y, z) associated with the pose
    float translationData[3];

    /// \brief Array with elements of 3*3 rotation matrix associated with the pose
    float rotationData[9];

    /// \brief Reason for the pose update
    PoseReason reason;

    /// \brief Coordinate system for the pose
    PoseCoordSystem coordinateSystem;

    /// \brief Quality / validity of the pose
    PoseValidity validity;
};
static_assert(sizeof(Pose) == 68, "Unexpected size for the Pose struct");

/// \brief Data structure for storing translation and rotation matrix associated with an anchor
/// \ingroup VuforiaDriverPDTGroup
struct AnchorPose
{
    /// \brief Array with translation vector values (x, y, z) associated with the anchor
    float translationData[3];

    /// \brief Array with elements of 3*3 rotation matrix associated with the anchor
    float rotationData[9];
};
static_assert(sizeof(AnchorPose) == 48, "Unexpected size for the AnchorPose struct");

/// \brief Data structure for storing information about an anchor
/// \ingroup VuforiaDriverPDTGroup
struct Anchor
{
    /// \brief UUID associated with the anchor
    /**
     * \note A successful call to createAnchor() generates an UUID.
     */
    const char* uuid;

    /// \brief Pose associated with the anchor
    AnchorPose pose;
};
static_assert(sizeof(Anchor) == (4 + (sizeof(void*) == 4 ? 0 : 4) + sizeof(AnchorPose)), "Unexpected size for the Anchor struct");

#pragma pack(pop)


/// \brief Interface to deliver camera frames to %Vuforia
/// \ingroup VuforiaDriverCameraGroup
class CameraCallback
{
public:
    /// \brief Callback function invoked when a new camera frame is available
    virtual void VUFORIA_DRIVER_CALLING_CONVENTION onNewCameraFrame(CameraFrame* frame) = 0;
};

/// \brief Interface to deliver poses to %Vuforia
/// \ingroup VuforiaDriverPDTGroup
class PoseCallback
{
public:
    /// \brief Callback function invoked when a new pose is available
    virtual void VUFORIA_DRIVER_CALLING_CONVENTION onNewPose(Pose* pose) = 0;
};

/// \brief Interface to deliver anchor updates to %Vuforia
/// \ingroup VuforiaDriverPDTGroup
class AnchorCallback
{
public:
    /// \brief Callback function invoked when new anchor information is available
    virtual void VUFORIA_DRIVER_CALLING_CONVENTION onAnchorUpdate(Anchor* anchors, int numAnchors, AnchorStatus status) = 0;
};

/// \brief Interface used by %Vuforia to interact with the external camera implementation
/**
 * \ingroup VuforiaDriverCameraGroup
 * \note The sequence of events between %Vuforia and the external camera implementation is as follows:
 *  1. %Vuforia calls VuforiaDriver::createExternalCamera() when the driver capability includes CAMERA_IMAGE.
 *  2. The implementation creates an ExternalCamera instance and returns it to %Vuforia.
 *  3. %Vuforia calls ExternalCamera::open() on the returned instance.
 *  4. %Vuforia discovers supported camera modes by iterating them by getting the number of modes
 *     with ExternalCamera::getNumSupportedCameraModes() and then iterates over the list
 *     with ExternalCamera::getSupportedCameraMode().
 *  5. %Vuforia calls ExternalCamera::processFramesOnThread()
 *  6. %Vuforia calls ExternalCamera::start(), which starts the flow of frames into
 *     the provided CameraCallback.
 *  7. On shutdown %Vuforia calls
 *     -> ExternalCamera::stop()
 *     -> ExternalCamera::close()
 *     -> and finally VuforiaDriver::destroyExternalCamera().
 */
class ExternalCamera
{
public:
    /// \brief Open the camera
    /**
     * \note After opening the camera, the supported video modes should be available to be queried
     * with getNumSupportedCameraModes() and getSupportedCameraMode().
     *
     * \returns True if the camera was opened, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION open() = 0;

    /// \brief Close the camera
    /**
     * \returns True if the camera was closed, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION close() = 0;

    /// \brief Start the camera
    /**
     * \param cameraMode The requested camera mode that the camera should deliver the frames in
     * \param cb Callback that the camera frames should be delivered to
     *
     * \returns True if the camera was started, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION start(CameraMode cameraMode, CameraCallback* cb) = 0;

    /// \brief Stop the camera
    /**
     * \returns True if the camera was stopped, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION stop() = 0;

    /// \brief Get the number of supported camera modes
    /**
     *  \note This function returns the total number of supported camera modes.
     *  %Vuforia uses this number to query the camera modes with
     *  getSupportedCameraMode(), which iterates from 0 to (totalNumber - 1).
     *
     * \returns Number of camera modes that this camera supports
     */
    virtual uint32_t VUFORIA_DRIVER_CALLING_CONVENTION getNumSupportedCameraModes() = 0;

    /// \brief Get a camera mode from a certain index
    /**
     * \param index The index of the mode to get, in the range [0 .. getNumSupportedCameraModes()-1]
     * \param cameraMode On return, this output parameter will be populated with the camera mode corresponding to the requested index.
     *
     * \returns True on success, or false if the index was out of bounds
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION getSupportedCameraMode(uint32_t index, CameraMode* cameraMode) = 0;

    /// \brief Check whether a particular exposure mode is supported
    /**
     * \param exposureMode The exposure mode to check
     * \returns True if the exposure mode is supported, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION supportsExposureMode(ExposureMode exposureMode) = 0;

    /// \brief Get current exposure mode
    /**
     * \returns The current exposure mode
     */
    virtual ExposureMode VUFORIA_DRIVER_CALLING_CONVENTION getExposureMode() = 0;

    /// \brief Set the current exposure mode
    /**
     * \param exposureMode New exposure mode
     *
     * \returns True if setting the exposure mode succeeded, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION setExposureMode(ExposureMode exposureMode) = 0;

    /// \brief Check whether setting the exposure manually is supported
    /**
     * \returns True if usage of setExposureValue() is supported, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION supportsExposureValue() = 0;

    /// \brief Get the minimum supported value for manual exposure
    /**
     * \returns The minimum value that can be provided for setExposureValue()
     */
    virtual uint64_t VUFORIA_DRIVER_CALLING_CONVENTION getExposureValueMin() = 0;

    /// \brief Get the maximum supported value for manual exposure
    /**
     * \returns The maximum value that can be provided for setExposureValue()
     */
    virtual uint64_t VUFORIA_DRIVER_CALLING_CONVENTION getExposureValueMax() = 0;

    /// \brief Get the current manual exposure value
    /**
     * \returns The current camera exposure value
     */
    virtual uint64_t VUFORIA_DRIVER_CALLING_CONVENTION getExposureValue() = 0;

    /// \brief Set the current manual exposure value
    /**
     * \param exposureTime New value for manual exposure
     *
     * \note To use this function, supportsExposureValue() must return true
     * and the provided value must be in the range specified by
     * getExposureValueMin() and getExposureValueMax() inclusive.
     *
     * \returns True if value was set successfully, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION setExposureValue(uint64_t exposureTime) = 0;

    /// \brief Check whether a particular focus mode is supported
    /**
     * \param focusMode The focus mode to check
     * \returns True if the mode is supported, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION supportsFocusMode(FocusMode focusMode) = 0;

    /// \brief Get the current focus mode
    /**
     * \returns The current focus mode
     */
    virtual FocusMode VUFORIA_DRIVER_CALLING_CONVENTION getFocusMode() = 0;

    /// \brief Set the current focus mode
    /**
     * \param mode The new focus mode
     *
     * \returns True if value was set successfully, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION setFocusMode(FocusMode mode) = 0;

    /// \brief Get whether setting manual focus distance is supported
    /**
     * \returns True if usage of setFocusValue() is supported, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION supportsFocusValue() = 0;

    /// \brief Get the minimum supported value for manual focus distance
    /**
     * \returns The minimum value in millimeters that can be provided for setFocusValue()
     */
    virtual float VUFORIA_DRIVER_CALLING_CONVENTION getFocusValueMin() = 0;

    /// Get the maximum supported value for manual focus distance.
    /**
     * \returns The maximum value in millimeters that can be provided for setFocusValue()
     */
    virtual float VUFORIA_DRIVER_CALLING_CONVENTION getFocusValueMax() = 0;

    /// \brief Get the current manual focus distance
    /**
     * \returns Current manual focus value in millimeters
     */
    virtual float VUFORIA_DRIVER_CALLING_CONVENTION getFocusValue() = 0;

    /// \brief Set the current manual focus value
    /**
     * \param focusValue The new value for manual focus distance in millimeters
     *
     * \note To use this supportsFocusValue() must return true
     * and the provided value must be in the range specified by
     * getFocusValueMin() and getFocusValueMax() inclusive.
     *
     * \returns True value was set successfully, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION setFocusValue(float focusValue) = 0;

    /// Function called by %Vuforia to determine whether it should create a separate thread to process frame data
    /**
     * \note By default this method returns false and %Vuforia will process the frame in the callback.
     * Once the callback returns a new %Vuforia state will be ready for the App to interrogate for
     * results. If this method returns true %Vuforia will create a separate thread to process the
     * frame, during the callback %Vuforia will just copy the provided data and queue it for processing
     * on the thread.
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION processFramesOnThread() { return false; }
};

/// \brief Interface used by %Vuforia to interact with the external positional device tracker implementation
/**
 * \ingroup VuforiaDriverPDTGroup
 * \note The sequence of events between %Vuforia and the external positional device tracker implementation is as follows:
 *  1. %Vuforia calls VuforiaDriver::createExternalPositionalDeviceTracker() when the driver capability includes CAMERA_POSE.
 *  2. The implementation creates an ExternalPositionalDeviceTracker instance and returns it to %Vuforia.
 *  3. %Vuforia calls ExternalPositionalDeviceTracker::open() on the returned instance.
 *  4. %Vuforia calls ExternalPositionalDeviceTracker::start(), which starts the flow of poses into
 *     the provided PoseCallback.
 *  5. On shutdown %Vuforia calls
 *     -> ExternalPositionalDeviceTracker::stop()
 *     -> ExternalPositionalDeviceTracker::close()
 *     -> and finally VuforiaDriver::destroyExternalPositionalDeviceTracker().
 */
class ExternalPositionalDeviceTracker
{
public:
    /// \brief Open the external positional device tracker
    /**
     * \returns True if the external positional device tracker is opened successfully, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION open() = 0;

    /// \brief Close the external positional device tracker
    /**
     * \returns True if the external positional device tracker is closed successfully, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION close() = 0;

    /// \brief Start the external positional device tracker
    /**
     * \param cb Callback that the pose should be delivered to
     * \param anchorCb Anchor callback to be notified when anchors are updated
     *
     * \returns True if the external positional device tracker was started, false otherwise.
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION start(PoseCallback* cb, AnchorCallback* anchorCb = nullptr) = 0;

    /// \brief Stop the external positional device tracker
    /**
     * \returns True if the external positional device tracker was stopped, false otherwise.
     * Once the stop returns true, driver implementation should stop pushing additional poses via PoseCallback.
     * PoseCallback should be considered as invalid once stop returns true.
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION stop() = 0;

    /// \brief Notify external positional device tracker to reset tracking the world/pose
    /**
     * \returns True if the external positional device tracker is successful in resetting tracking, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION resetTracking() = 0;

    /// \brief Query the driver to check if it supports anchors
    /**
     * \note Anchor support is an optional feature, therefore the default implementation returns false
     * \returns True if the anchors are supported, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION isAnchorSupported() { return false; }

    /// \brief Create an anchor with the given pose
    /**
     * \param anchorPose Pose to create an anchor with
     * \note Anchor support is an optional feature, therefore the default implementation returns nullptr
     * \returns UUID (char*) if anchor was successfully created
     */
    virtual const char* VUFORIA_DRIVER_CALLING_CONVENTION createAnchor(AnchorPose* anchorPose)
    {
        (void)anchorPose;
        return nullptr;
    }

    /// \brief Remove the Anchor for the given UUID
    /**
     * \param uuid UUID of the anchor to be removed. This UUID was obtained via successful call to createAnchor
     * \note Anchor support is an optional feature, therefore the default implementation returns false
     * \returns True if Anchor was successfully removed, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION removeAnchor(const char* uuid)
    {
        (void)uuid;
        return false;
    }

    /// \brief Query the Driver whether it provides a value for the specified PoseFusionParameter
    /**
     * \param poseFusionParam One of the values from the PoseFusionParameter enum.
     * \returns True if the Driver provides a value for the requested PoseFusionParameter, false otherwise
     */
    virtual bool VUFORIA_DRIVER_CALLING_CONVENTION providesFusionParameterValue(PoseFusionParameter poseFusionParam)
    {
        (void)poseFusionParam;
        return false;
    };

    /// \brief Get the value for the specified PoseFusionParameter
    /**
     * \param poseFusionParam One of the values from the PoseFusionParameter enum
     * \returns The value that %Vuforia Engine should use with this %Vuforia Driver
     */
    virtual int32_t VUFORIA_DRIVER_CALLING_CONVENTION getFusionParameterValue(PoseFusionParameter poseFusionParam)
    {
        (void)poseFusionParam;
        return -1;
    }
};

/// \brief Interface used by %Vuforia to interact with the Vuforia Driver implementation
/**
 *  \ingroup VuforiaDriverAPIGroup
 *  \note The sequence of events between %Vuforia and the external driver implementation is as follows:
 *  1. %Vuforia calls vuforiaDriver_init().
 *  2. %Vuforia uses the VuforiaDriver.
 *  3. On shutdown %Vuforia calls vuforiaDriver_deinit().
 */
class Driver
{
public:
    /// \brief Construct a new ExternalCamera instance
    /**
     * \note %Vuforia uses this instance to interact with a camera. The object is
     * expected to be valid until destroyExternalCamera() is called.
     * The memory for the object is owned by the library.
     * Only one instance of ExternalCamera is created, a second call to
     * createExternalCamera without a call to destroyExternalCamera is
     * an error and the implementation should return a nullptr.
     *
     * \returns New camera instance
     */
    virtual ExternalCamera* VUFORIA_DRIVER_CALLING_CONVENTION createExternalCamera() = 0;

    /// \brief Destruct an instance of an ExternalCamera object
    /**
     * %Vuforia will call this function to destroy the instance that was created with createExternalCamera().
     * \param instance Camera instance to destroy
     */
    virtual void VUFORIA_DRIVER_CALLING_CONVENTION destroyExternalCamera(ExternalCamera* instance) = 0;

    /// \brief Construct a new instance of an external positional device tracker
    /**
     * \note %Vuforia will call this function only if the driver capability includes CAMERA_POSE.
     * \note %Vuforia will use this instance to receive pose(s). The object is
     * expected to be valid until destroyExternalPositionalDeviceTracker() is called.
     * The memory for the object is owned by the library.
     * Only one instance of ExternalPositionalDeviceTracker is created, a second call to
     * createExternalPositionalDeviceTracker without a call to destroyExternalPositionalDeviceTracker is
     * an error and the implementation should return a nullptr.
     *
     * \returns New ExternalPositionalDeviceTracker instance
     */
    virtual ExternalPositionalDeviceTracker* VUFORIA_DRIVER_CALLING_CONVENTION createExternalPositionalDeviceTracker() { return nullptr; }

    /// \brief Destruct an instance of an ExternalPositionalDeviceTracker object.
    /**
     * \note %Vuforia will call this function to destroy the instance created with createExternalPositionalDeviceTracker()
     * \param instance ExternalPositionalDeviceTracker instance to destroy
     */
    virtual void VUFORIA_DRIVER_CALLING_CONVENTION destroyExternalPositionalDeviceTracker(ExternalPositionalDeviceTracker* instance)
    {
        (void)instance;
    }

    /// \brief Query the driver implementation for supported capabilities
    /**
     * \note Vuforia defaults to the Camera Image only capability
     */
    virtual uint32_t VUFORIA_DRIVER_CALLING_CONVENTION getCapabilities() { return (uint32_t)Capability::CAMERA_IMAGE; }

    /// Get the camera orientation
    /**
     * \note The value may need to be adjusted based on the current device orientation provided in the parameter.
     *
     * \note The camera orientation describes how much the camera sensor is rotated with respect to the natural orientation of a device.
     * This value is needed for correctly displaying the camera frames coming from the Vuforia Driver.
     *
     * \param deviceOrientationInDegrees The device orientation of the app that will display the driver data. The device orientation
     * describes the orientation of the device with respect to its natural position. For instance, many mobile phones will naturally be
     * in portrait orientation. So, if such a mobile phone is held in a landscape orientation, this will be a non-zero angle value.
     * \returns the camera orientation value (in degrees) required for correct display. By default, the driver returns 0 degrees.
     */
    virtual uint32_t VUFORIA_DRIVER_CALLING_CONVENTION getCameraOrientation(uint32_t deviceOrientationInDegrees)
    {
        (void)deviceOrientationInDegrees;
        return 0;
    }

    /// Get the value for the specified hint
    /**
     * \note Vuforia Engine can modify its behavior based on hints about the device that it is running on.
     * Drivers may provide values for certain hints to optimize performance on particular hardware.
     *
     * \param name The name of the hint as provided by the Vuforia team
     * \returns the null terminated string value for the requested hint or nullptr if the hint name is not recognized
     */
    virtual const char* VUFORIA_DRIVER_CALLING_CONVENTION getDeviceHint(const char* name)
    {
        (void)name;
        return nullptr;
    }
};

} // namespace VuforiaDriver

/** \addtogroup VuforiaDriverAPIGroup
 * \{
 */

extern "C"
{
/// \brief Query the Vuforia Driver API version number
/**
 * \note This function returns the version of the Vuforia Driver API that this plugin conforms to.
 * \returns Should return EXTERNAL_PROVIDER_API_VERSION defined in this file.
 */
VUFORIA_DRIVER_API_EXPORT uint32_t VUFORIA_DRIVER_CALLING_CONVENTION vuforiaDriver_getAPIVersion();

/// \brief Return the library version number
/**
 * \note This function should write a versionString-parameter with a user defined library version string
 * with maximum length of maxLen.
 *
 * \note Empty strings are not supported
 *
 * \param versionString This should be populated with the library version string
 * \param maxLen Maximum length in bytes that can fit to versionString
 * \returns The number of bytes written into versionString
 */
VUFORIA_DRIVER_API_EXPORT uint32_t VUFORIA_DRIVER_CALLING_CONVENTION vuforiaDriver_getLibraryVersion(char* versionString,
                                                                                                     const uint32_t maxLen);

/// \brief Construct a new instance of a VuforiaDriver
/**
 * \note %Vuforia will use this instance to interact with the library. The object is
 * expected to be valid until vuforiaDriver_deinit() is called.
 * The memory for the object is owned by the library.
 * Only one instance of VuforiaDriver is created, a second call to
 * vuforiaDriver_init without a call to vuforiaDriver_deinit is
 * an error and the implementation should return a nullptr.
 *
 * \param platformData Platform-specific initialization data, that has been defined in the beginning of this header file
 * \param userData User data that the developer provides as the "userData" parameter in the VuDriverConfig Engine configuration data
 * structure \returns New Vuforia Driver instance
 */
VUFORIA_DRIVER_API_EXPORT VuforiaDriver::Driver* VUFORIA_DRIVER_CALLING_CONVENTION
vuforiaDriver_init(VuforiaDriver::PlatformData* platformData, void* userData);

/// \brief Destruct an instance of a VuforiaDriver object
/**
 * \note %Vuforia will call this to destroy the instance that was created with vuforiaDriver_init().
 * \param instance Vuforia Driver instance to destroy
 */
VUFORIA_DRIVER_API_EXPORT void VUFORIA_DRIVER_CALLING_CONVENTION vuforiaDriver_deinit(VuforiaDriver::Driver* instance);
}

/** \} */

#endif // _VU_DRIVER_H_
