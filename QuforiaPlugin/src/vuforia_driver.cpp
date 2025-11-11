#include "vuforia_driver.h"
#include "external_camera.h"
#include "external_tracker.h"
#include <android/log.h>
#include <cstring>

#define LOG_TAG "QUFORIA"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// Global driver instance
QuestVuforiaDriver* g_driverInstance = nullptr;

// =============================================================================
// Entry Point Functions (C linkage - required by Vuforia Driver Framework)
// =============================================================================

extern "C" {

JNIEXPORT VuforiaDriver::Driver* JNICALL vuforiaDriver_init(
    VuforiaDriver::PlatformData* platformData,
    void* userData)
{
    LOGI("vuforiaDriver_init called");

    if (g_driverInstance != nullptr) {
        LOGE("Driver already initialized");
        return g_driverInstance;
    }

    try {
        g_driverInstance = new QuestVuforiaDriver(platformData, userData);
        LOGI("QuestVuforiaDriver created successfully");
        return g_driverInstance;
    } catch (const std::exception& e) {
        LOGE("Failed to create driver: %s", e.what());
        return nullptr;
    }
}

JNIEXPORT void JNICALL vuforiaDriver_deinit(VuforiaDriver::Driver* driver) {
    LOGI("vuforiaDriver_deinit called");

    if (driver == nullptr) {
        LOGE("Driver is null");
        return;
    }

    if (driver == g_driverInstance) {
        delete g_driverInstance;
        g_driverInstance = nullptr;
        LOGI("QuestVuforiaDriver destroyed");
    } else {
        LOGE("Driver mismatch");
    }
}

JNIEXPORT uint32_t JNICALL vuforiaDriver_getAPIVersion() {
    // Return Vuforia Driver API version (should be 7 for SDK 11.4.4)
    return VuforiaDriver::VUFORIA_DRIVER_API_VERSION;
}

JNIEXPORT uint32_t JNICALL vuforiaDriver_getLibraryVersion(char* versionString, const uint32_t maxLen) {
    const char* version = "QuestVuforiaDriver 1.0.0";
    uint32_t len = strlen(version);
    if (len >= maxLen) {
        len = maxLen - 1;
    }
    strncpy(versionString, version, len);
    versionString[len] = '\0';
    return len;
}

} // extern "C"

// =============================================================================
// QuestVuforiaDriver Implementation
// =============================================================================

QuestVuforiaDriver::QuestVuforiaDriver(VuforiaDriver::PlatformData* platformData,
                                       void* userData)
    : camera_(nullptr)
    , tracker_(nullptr)
    , intrinsicsSet_(false)
{
    (void)platformData;  // Unused parameter (provided by Vuforia for Android JNI access if needed)
    (void)userData;      // Unused parameter
    LOGI("QuestVuforiaDriver constructor");

    // Initialize cached intrinsics with default values
    memset(&cachedIntrinsics_, 0, sizeof(cachedIntrinsics_));
}

QuestVuforiaDriver::~QuestVuforiaDriver() {
    LOGI("QuestVuforiaDriver destructor");

    if (camera_) {
        delete camera_;
        camera_ = nullptr;
    }

    if (tracker_) {
        delete tracker_;
        tracker_ = nullptr;
    }

    // Clear frame queue
    std::lock_guard<std::mutex> frameLock(frameMutex_);
    while (!frameQueue_.empty()) {
        frameQueue_.pop();
    }

    // Clear pose queue
    std::lock_guard<std::mutex> poseLock(poseMutex_);
    while (!poseQueue_.empty()) {
        poseQueue_.pop();
    }
}

uint32_t QuestVuforiaDriver::getCapabilities() {
    // Report that this driver provides:
    // - Camera images (CAMERA_IMAGE)
    // - Device pose (CAMERA_POSE)
    uint32_t capabilities = (1 << VuforiaDriver::CAMERA_IMAGE) |
                           (1 << VuforiaDriver::CAMERA_POSE);

    LOGI("getCapabilities() returning: 0x%X", capabilities);
    return capabilities;
}

VuforiaDriver::ExternalCamera* QuestVuforiaDriver::createExternalCamera() {
    LOGI("createExternalCamera()");

    if (camera_ != nullptr) {
        LOGE("Camera already exists");
        return camera_;
    }

    try {
        camera_ = new QuestExternalCamera(this);
        LOGI("QuestExternalCamera created");
        return camera_;
    } catch (const std::exception& e) {
        LOGE("Failed to create external camera: %s", e.what());
        return nullptr;
    }
}

void QuestVuforiaDriver::destroyExternalCamera(VuforiaDriver::ExternalCamera* instance) {
    LOGI("destroyExternalCamera()");

    if (instance == camera_) {
        delete camera_;
        camera_ = nullptr;
        LOGI("QuestExternalCamera destroyed");
    } else {
        LOGE("Camera instance mismatch");
    }
}

VuforiaDriver::ExternalPositionalDeviceTracker*
QuestVuforiaDriver::createExternalPositionalDeviceTracker() {
    LOGI("createExternalPositionalDeviceTracker()");

    if (tracker_ != nullptr) {
        LOGE("Tracker already exists");
        return tracker_;
    }

    try {
        tracker_ = new QuestExternalTracker(this);
        LOGI("QuestExternalTracker created");
        return tracker_;
    } catch (const std::exception& e) {
        LOGE("Failed to create external tracker: %s", e.what());
        return nullptr;
    }
}

void QuestVuforiaDriver::destroyExternalPositionalDeviceTracker(
    VuforiaDriver::ExternalPositionalDeviceTracker* instance) {
    LOGI("destroyExternalPositionalDeviceTracker()");

    if (instance == tracker_) {
        delete tracker_;
        tracker_ = nullptr;
        LOGI("QuestExternalTracker destroyed");
    } else {
        LOGE("Tracker instance mismatch");
    }
}

// =============================================================================
// Frame and Pose Feeding (called from JNI layer)
// =============================================================================

void QuestVuforiaDriver::feedCameraFrame(const uint8_t* imageData, int width, int height,
                                        const float* intrinsics, int64_t timestamp) {
    std::lock_guard<std::mutex> lock(frameMutex_);

    // Create new frame data
    auto frameData = std::make_shared<CameraFrameData>();
    frameData->width = width;
    frameData->height = height;
    frameData->timestamp = timestamp;

    // Copy image data
    int dataSize = width * height * 3;  // RGB888
    frameData->imageData = new uint8_t[dataSize];
    memcpy(frameData->imageData, imageData, dataSize);

    // Set intrinsics (use cached if available, otherwise from parameter)
    // Note: Intrinsics array format from Unity: [width, height, fx, fy, cx, cy, d0-d7]
    // Width/height are in indices 0-1 (ignored here, used from frame dimensions)
    // Focal lengths and principal point in indices 2-5
    // Distortion coefficients in indices 6-13
    {
        std::lock_guard<std::mutex> intrinsicsLock(intrinsicsMutex_);
        if (intrinsicsSet_) {
            frameData->intrinsics = cachedIntrinsics_;
        } else if (intrinsics != nullptr) {
            frameData->intrinsics.focalLengthX = intrinsics[2];
            frameData->intrinsics.focalLengthY = intrinsics[3];
            frameData->intrinsics.principalPointX = intrinsics[4];
            frameData->intrinsics.principalPointY = intrinsics[5];
            // Distortion coefficients (8 values starting at index 6)
            for (int i = 0; i < 8; i++) {
                frameData->intrinsics.distortionCoefficients[i] = intrinsics[i + 6];
            }
        }
    }

    // Add to queue
    frameQueue_.push(frameData);

    // Keep only last N frames
    while (frameQueue_.size() > MAX_FRAME_QUEUE_SIZE) {
        frameQueue_.pop();
    }

    LOGD("Frame fed: %dx%d, timestamp=%lld, queue_size=%zu",
         width, height, (long long)timestamp, frameQueue_.size());
}

void QuestVuforiaDriver::feedDevicePose(const float* position, const float* rotation,
                                       int64_t timestamp) {
    std::lock_guard<std::mutex> lock(poseMutex_);

    // Create new pose data
    auto poseData = std::make_shared<PoseData>();
    poseData->timestamp = timestamp;

    // Copy position (x, y, z)
    if (position != nullptr) {
        memcpy(poseData->position, position, 3 * sizeof(float));
    }

    // Copy rotation quaternion (x, y, z, w)
    if (rotation != nullptr) {
        memcpy(poseData->rotation, rotation, 4 * sizeof(float));
    }

    // Add to queue
    poseQueue_.push(poseData);

    // Keep only last N poses
    while (poseQueue_.size() > MAX_POSE_QUEUE_SIZE) {
        poseQueue_.pop();
    }

    LOGD("Pose fed: pos(%.3f,%.3f,%.3f), timestamp=%lld, queue_size=%zu",
         poseData->position[0], poseData->position[1], poseData->position[2],
         (long long)timestamp, poseQueue_.size());
}

void QuestVuforiaDriver::setCameraIntrinsics(const float* intrinsics) {
    if (intrinsics == nullptr) {
        LOGE("setCameraIntrinsics: intrinsics is null");
        return;
    }

    std::lock_guard<std::mutex> lock(intrinsicsMutex_);

    // Intrinsics array format from Unity: [width, height, fx, fy, cx, cy, d0-d7]
    // Width/height are at indices 0-1 (only for reference, not stored in CameraIntrinsics struct)
    // Focal lengths and principal point at indices 2-5
    // Distortion coefficients at indices 6-13
    cachedIntrinsics_.focalLengthX = intrinsics[2];
    cachedIntrinsics_.focalLengthY = intrinsics[3];
    cachedIntrinsics_.principalPointX = intrinsics[4];
    cachedIntrinsics_.principalPointY = intrinsics[5];

    // Distortion coefficients (8 values starting at index 6)
    for (int i = 0; i < 8; i++) {
        cachedIntrinsics_.distortionCoefficients[i] = intrinsics[i + 6];
    }

    intrinsicsSet_ = true;

    LOGI("Camera intrinsics set: %.0fx%.0f, fx=%.2f, fy=%.2f, cx=%.2f, cy=%.2f",
         intrinsics[0], intrinsics[1],  // width, height for logging only
         cachedIntrinsics_.focalLengthX, cachedIntrinsics_.focalLengthY,
         cachedIntrinsics_.principalPointX, cachedIntrinsics_.principalPointY);
}

// =============================================================================
// Frame/Pose Retrieval (called by ExternalCamera and ExternalTracker)
// =============================================================================

std::shared_ptr<CameraFrameData> QuestVuforiaDriver::acquireLatestFrame() {
    std::lock_guard<std::mutex> lock(frameMutex_);

    if (frameQueue_.empty()) {
        return nullptr;
    }

    // Return the latest frame (back of queue)
    // Don't pop it - let it age out naturally
    return frameQueue_.back();
}

std::shared_ptr<PoseData> QuestVuforiaDriver::acquirePoseForTimestamp(int64_t timestamp) {
    std::lock_guard<std::mutex> lock(poseMutex_);

    if (poseQueue_.empty()) {
        LOGD("Pose queue is empty");
        return nullptr;
    }

    // Find the pose with the closest timestamp
    // In production, you should interpolate between poses
    std::shared_ptr<PoseData> closestPose = nullptr;
    int64_t minTimeDiff = INT64_MAX;

    // Create a temporary queue to iterate without modifying the original
    std::queue<std::shared_ptr<PoseData>> tempQueue = poseQueue_;

    while (!tempQueue.empty()) {
        auto pose = tempQueue.front();
        tempQueue.pop();

        int64_t timeDiff = std::abs(pose->timestamp - timestamp);
        if (timeDiff < minTimeDiff) {
            minTimeDiff = timeDiff;
            closestPose = pose;
        }
    }

    if (closestPose && minTimeDiff < 50000000) {  // Within 50ms
        LOGD("Found pose for timestamp %lld (diff=%lld ns)",
             (long long)timestamp, (long long)minTimeDiff);
        return closestPose;
    } else {
        LOGD("No matching pose found for timestamp %lld (closest diff=%lld ns)",
             (long long)timestamp, (long long)minTimeDiff);
        return nullptr;
    }
}
