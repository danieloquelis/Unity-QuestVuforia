#include <android/log.h>
#include <cstring>
#include "vuforia_driver.h"

#define LOG_TAG "QUFORIA"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

/**
 * Unity P/Invoke Bridge
 *
 * These functions are called directly from Unity C# using [DllImport].
 * Unity handles all array marshaling - no JNI needed!
 */

extern "C" {

/**
 * Set camera intrinsics (called once at initialization)
 */
bool nativeSetCameraIntrinsics(float* intrinsics, int length) {

    LOGI("nativeSetCameraIntrinsics: %d elements", length);

    if (!g_driverInstance) {
        LOGE("Driver not initialized");
        return false;
    }

    if (!intrinsics || length < 6) {
        LOGE("Invalid intrinsics array");
        return false;
    }

    g_driverInstance->setCameraIntrinsics(intrinsics);

    LOGI("Camera intrinsics set: %.0fx%.0f, fx=%.2f, fy=%.2f, cx=%.2f, cy=%.2f",
         intrinsics[0], intrinsics[1], intrinsics[2], intrinsics[3],
         intrinsics[4], intrinsics[5]);

    return true;
}

/**
 * Feed device pose to the Vuforia Driver
 * CRITICAL: Must be called BEFORE feedCameraFrame with same timestamp
 */
bool nativeFeedDevicePose(float* position, float* rotation, long long timestamp) {

    if (!g_driverInstance) {
        LOGE("Driver not initialized");
        return false;
    }

    if (!position || !rotation) {
        LOGE("Null position or rotation");
        return false;
    }

    g_driverInstance->feedDevicePose(position, rotation, timestamp);
    return true;
}

/**
 * Feed camera frame to the Vuforia Driver
 */
bool nativeFeedCameraFrame(unsigned char* imageData, int width, int height,
                           float* intrinsics, int intrinsicsLength, long long timestamp) {

    if (!g_driverInstance) {
        LOGE("Driver not initialized");
        return false;
    }

    if (!imageData) {
        LOGE("Null image data");
        return false;
    }

    g_driverInstance->feedCameraFrame(imageData, width, height, intrinsics, timestamp);
    return true;
}

/**
 * Check if driver is initialized
 */
bool nativeIsDriverInitialized() {
    return (g_driverInstance != nullptr);
}

} // extern "C"
