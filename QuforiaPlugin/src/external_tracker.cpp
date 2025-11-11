#include "external_tracker.h"
#include "vuforia_driver.h"
#include <android/log.h>
#include <chrono>
#include <thread>
#include <cstring>
#include <cmath>

#define LOG_TAG "QUFORIA"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

QuestExternalTracker::QuestExternalTracker(QuestVuforiaDriver* driver)
    : driver_(driver)
    , callback_(nullptr)
    , isRunning_(false)
    , isOpen_(false)
    , lastPoseTimestamp_(0)
{
    LOGI("QuestExternalTracker constructor");
}

QuestExternalTracker::~QuestExternalTracker() {
    LOGI("QuestExternalTracker destructor");
    stop();
    close();
}

// =============================================================================
// Lifecycle Methods
// =============================================================================

bool QuestExternalTracker::open() {
    LOGI("open()");

    if (isOpen_) {
        LOGW("Tracker already open");
        return true;
    }

    isOpen_ = true;
    LOGI("Tracker opened successfully");
    return true;
}

bool QuestExternalTracker::close() {
    LOGI("close()");

    if (!isOpen_) {
        LOGD("Tracker already closed");
        return true;
    }

    // Stop pose delivery if running
    if (isRunning_) {
        stop();
    }

    isOpen_ = false;
    LOGI("Tracker closed");
    return true;
}

bool QuestExternalTracker::start(VuforiaDriver::PoseCallback* cb, VuforiaDriver::AnchorCallback* anchorCb) {
    LOGI("start()");

    if (!isOpen_) {
        LOGE("Tracker not open");
        return false;
    }

    if (isRunning_) {
        LOGW("Tracker already running");
        return true;
    }

    if (cb == nullptr) {
        LOGE("Callback is null");
        return false;
    }

    callback_ = cb;
    // anchorCb is optional and not used (anchor support not implemented)
    (void)anchorCb;

    isRunning_ = true;
    lastPoseTimestamp_ = 0;

    // Start pose delivery thread
    poseThread_ = std::thread(&QuestExternalTracker::poseDeliveryThread, this);

    LOGI("Tracker started successfully");
    return true;
}

bool QuestExternalTracker::stop() {
    LOGI("stop()");

    if (!isRunning_) {
        LOGD("Tracker not running");
        return true;
    }

    // Signal thread to stop
    isRunning_ = false;

    // Wait for thread to finish
    if (poseThread_.joinable()) {
        poseThread_.join();
    }

    callback_ = nullptr;
    LOGI("Tracker stopped");
    return true;
}

bool QuestExternalTracker::resetTracking() {
    LOGI("resetTracking()");

    // Reset tracking state
    lastPoseTimestamp_ = 0;

    // In a full implementation, this would reset the Quest's tracking system
    // For now, we just reset our internal state
    LOGW("resetTracking() not fully implemented - only resetting internal state");
    return true;
}

// =============================================================================
// Pose Delivery Thread
// =============================================================================

void QuestExternalTracker::poseDeliveryThread() {
    LOGI("Pose delivery thread started");

    int poseCount = 0;
    const auto pollInterval = std::chrono::milliseconds(10);  // Poll every 10ms

    while (isRunning_) {
        auto pollStartTime = std::chrono::steady_clock::now();

        // Acquire latest frame to get its timestamp
        auto frameData = driver_->acquireLatestFrame();

        if (frameData && callback_) {
            int64_t frameTimestamp = frameData->timestamp;

            // Only deliver pose if timestamp is new (avoid duplicates)
            if (frameTimestamp != lastPoseTimestamp_) {
                // Acquire pose for this frame's timestamp
                auto poseData = driver_->acquirePoseForTimestamp(frameTimestamp);

                if (poseData) {
                    // Transform pose from OpenXR to Vuforia CV convention
                    float transformedPosition[3];
                    float transformedRotation[9];  // 3x3 rotation matrix

                    transformOpenXRToCV(poseData->position, poseData->rotation,
                                       transformedPosition, transformedRotation);

                    // Prepare Vuforia pose structure
                    VuforiaDriver::Pose vuforiaPose;
                    vuforiaPose.timestamp = frameTimestamp;
                    memcpy(vuforiaPose.translationData, transformedPosition, 3 * sizeof(float));
                    memcpy(vuforiaPose.rotationData, transformedRotation, 9 * sizeof(float));
                    vuforiaPose.reason = VuforiaDriver::PoseReason::VALID;
                    vuforiaPose.coordinateSystem = VuforiaDriver::PoseCoordSystem::CAMERA;
                    vuforiaPose.validity = VuforiaDriver::PoseValidity::VALID;

                    // **CRITICAL:** Deliver pose BEFORE frame
                    // This is a requirement of the Vuforia Driver Framework
                    callback_->onNewPose(&vuforiaPose);

                    lastPoseTimestamp_ = frameTimestamp;
                    poseCount++;

                    if (poseCount % 30 == 0) {
                        LOGD("Delivered %d poses (latest timestamp: %lld)",
                             poseCount, (long long)frameTimestamp);
                    }
                } else {
                    LOGD("No pose available for timestamp %lld", (long long)frameTimestamp);
                }
            }
        }

        // Sleep to avoid busy waiting
        auto pollEndTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            pollEndTime - pollStartTime);

        if (elapsed < pollInterval) {
            std::this_thread::sleep_for(pollInterval - elapsed);
        }
    }

    LOGI("Pose delivery thread stopped (delivered %d poses)", poseCount);
}

// =============================================================================
// Coordinate System Transformation
// =============================================================================

void QuestExternalTracker::transformOpenXRToCV(const float* positionIn, const float* rotationIn,
                                               float* positionOut, float* rotationOut) {
    /**
     * Coordinate system transformation:
     *
     * Unity/OpenXR convention:
     *   X: Right
     *   Y: Up
     *   Z: Back (toward user)
     *   Handedness: Left-handed
     *
     * Vuforia CV convention:
     *   X: Right
     *   Y: Down
     *   Z: Away from camera (into scene)
     *   Handedness: Right-handed
     *
     * Transformation:
     *   X' = X (unchanged)
     *   Y' = -Y (flip Y axis: up → down)
     *   Z' = -Z (flip Z axis: back → forward)
     */

    // Transform position
    positionOut[0] = positionIn[0];   // X unchanged
    positionOut[1] = -positionIn[1];  // Y flipped
    positionOut[2] = -positionIn[2];  // Z flipped

    // Transform rotation (quaternion to rotation matrix)
    // Input quaternion: (x, y, z, w)
    float qx = rotationIn[0];
    float qy = rotationIn[1];
    float qz = rotationIn[2];
    float qw = rotationIn[3];

    // Apply 180-degree rotation around X-axis to flip Y and Z
    // This is equivalent to: R_cv = R_x(180°) * R_openxr
    // R_x(180°) = [1, 0, 0; 0, -1, 0; 0, 0, -1]

    // Transform the quaternion
    // q' = q_x(180°) * q_openxr
    // q_x(180°) = (1, 0, 0, 0) - represents 180° rotation around X
    float transformedQuat[4];
    transformedQuat[0] = qw;   // x' = w (from quaternion multiplication)
    transformedQuat[1] = -qz;  // y' = -z
    transformedQuat[2] = -qy;  // z' = -y
    transformedQuat[3] = qx;   // w' = x

    // Convert transformed quaternion to 3x3 rotation matrix (row-major)
    quaternionToMatrix(transformedQuat, rotationOut);
}

void QuestExternalTracker::quaternionToMatrix(const float* quat, float* matrix) {
    /**
     * Convert quaternion (x, y, z, w) to 3x3 rotation matrix (row-major)
     * Reference: https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/
     */

    float x = quat[0];
    float y = quat[1];
    float z = quat[2];
    float w = quat[3];

    float xx = x * x;
    float xy = x * y;
    float xz = x * z;
    float xw = x * w;
    float yy = y * y;
    float yz = y * z;
    float yw = y * w;
    float zz = z * z;
    float zw = z * w;

    // Row-major 3x3 matrix
    matrix[0] = 1.0f - 2.0f * (yy + zz);  // m00
    matrix[1] = 2.0f * (xy - zw);         // m01
    matrix[2] = 2.0f * (xz + yw);         // m02

    matrix[3] = 2.0f * (xy + zw);         // m10
    matrix[4] = 1.0f - 2.0f * (xx + zz);  // m11
    matrix[5] = 2.0f * (yz - xw);         // m12

    matrix[6] = 2.0f * (xz - yw);         // m20
    matrix[7] = 2.0f * (yz + xw);         // m21
    matrix[8] = 1.0f - 2.0f * (xx + yy);  // m22
}
