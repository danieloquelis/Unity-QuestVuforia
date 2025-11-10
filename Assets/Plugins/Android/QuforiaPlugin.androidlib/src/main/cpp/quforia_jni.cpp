#include <jni.h>
#include <android/log.h>
#include "vuforia_driver.h"

#define LOG_TAG "QuestVuforiaJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

/**
 * JNI Bridge for Vuforia Driver Framework
 *
 * This bridge provides methods for feeding camera frames and device poses
 * to the Vuforia Driver Framework. The actual Vuforia initialization and
 * target tracking is handled by the Vuforia Unity SDK.
 *
 * Key differences from old architecture:
 * - No direct Vuforia Engine initialization (handled by Unity SDK)
 * - No Image/Model Target management (handled by Unity SDK)
 * - Focus on frame/pose feeding to the Driver
 */

extern "C" {

/**
 * Feed camera frame to the Vuforia Driver
 * Called from Kotlin layer when a new frame is available
 */
JNIEXPORT jboolean JNICALL
Java_com_quforia_QuestVuforiaManager_nativeFeedCameraFrame(
    JNIEnv* env, jobject thiz,
    jbyteArray imageData, jint width, jint height,
    jfloatArray intrinsicsArray, jlong timestamp) {

    (void)thiz;  // Unused JNI parameter
    LOGD("nativeFeedCameraFrame: %dx%d, timestamp=%lld", width, height, (long long)timestamp);

    if (!g_driverInstance) {
        LOGE("Driver not initialized");
        return JNI_FALSE;
    }

    if (!imageData || !intrinsicsArray) {
        LOGE("Null image data or intrinsics");
        return JNI_FALSE;
    }

    // Get image data
    jbyte* imageBytes = env->GetByteArrayElements(imageData, nullptr);
    if (!imageBytes) {
        LOGE("Failed to get image bytes");
        return JNI_FALSE;
    }

    // Get intrinsics
    jfloat* intrinsics = env->GetFloatArrayElements(intrinsicsArray, nullptr);
    if (!intrinsics) {
        LOGE("Failed to get intrinsics");
        env->ReleaseByteArrayElements(imageData, imageBytes, JNI_ABORT);
        return JNI_FALSE;
    }

    // Feed frame to driver
    g_driverInstance->feedCameraFrame(
        reinterpret_cast<const uint8_t*>(imageBytes),
        width, height, intrinsics, timestamp
    );

    // Release arrays
    env->ReleaseByteArrayElements(imageData, imageBytes, JNI_ABORT);
    env->ReleaseFloatArrayElements(intrinsicsArray, intrinsics, JNI_ABORT);

    return JNI_TRUE;
}

/**
 * Feed device pose to the Vuforia Driver
 * Called from Kotlin layer for every frame to provide 6DoF tracking
 * CRITICAL: Must be called BEFORE feedCameraFrame with same timestamp
 */
JNIEXPORT jboolean JNICALL
Java_com_quforia_QuestVuforiaManager_nativeFeedDevicePose(
    JNIEnv* env, jobject thiz,
    jfloatArray positionArray, jfloatArray rotationArray, jlong timestamp) {

    (void)thiz;  // Unused JNI parameter
    LOGD("nativeFeedDevicePose: timestamp=%lld", (long long)timestamp);

    if (!g_driverInstance) {
        LOGE("Driver not initialized");
        return JNI_FALSE;
    }

    if (!positionArray || !rotationArray) {
        LOGE("Null position or rotation");
        return JNI_FALSE;
    }

    // Get position (x, y, z)
    jfloat* position = env->GetFloatArrayElements(positionArray, nullptr);
    if (!position) {
        LOGE("Failed to get position");
        return JNI_FALSE;
    }

    // Get rotation quaternion (x, y, z, w)
    jfloat* rotation = env->GetFloatArrayElements(rotationArray, nullptr);
    if (!rotation) {
        LOGE("Failed to get rotation");
        env->ReleaseFloatArrayElements(positionArray, position, JNI_ABORT);
        return JNI_FALSE;
    }

    // Feed pose to driver
    g_driverInstance->feedDevicePose(position, rotation, timestamp);

    // Release arrays
    env->ReleaseFloatArrayElements(positionArray, position, JNI_ABORT);
    env->ReleaseFloatArrayElements(rotationArray, rotation, JNI_ABORT);

    return JNI_TRUE;
}

/**
 * Set camera intrinsics (called once at initialization)
 * Intrinsics are cached and used for all frames
 */
JNIEXPORT jboolean JNICALL
Java_com_quforia_QuestVuforiaManager_nativeSetCameraIntrinsics(
    JNIEnv* env, jobject thiz, jfloatArray intrinsicsArray) {

    (void)thiz;  // Unused JNI parameter
    LOGI("nativeSetCameraIntrinsics");

    if (!g_driverInstance) {
        LOGE("Driver not initialized");
        return JNI_FALSE;
    }

    if (!intrinsicsArray) {
        LOGE("Null intrinsics");
        return JNI_FALSE;
    }

    // Get intrinsics array
    jfloat* intrinsics = env->GetFloatArrayElements(intrinsicsArray, nullptr);
    if (!intrinsics) {
        LOGE("Failed to get intrinsics");
        return JNI_FALSE;
    }

    // Set intrinsics in driver
    g_driverInstance->setCameraIntrinsics(intrinsics);

    LOGI("Camera intrinsics set: %.0fx%.0f, fx=%.2f, fy=%.2f, cx=%.2f, cy=%.2f",
         intrinsics[0], intrinsics[1], intrinsics[2], intrinsics[3],
         intrinsics[4], intrinsics[5]);

    // Release array
    env->ReleaseFloatArrayElements(intrinsicsArray, intrinsics, JNI_ABORT);

    return JNI_TRUE;
}

/**
 * Check if driver is initialized
 * Useful for debugging and status checks
 */
JNIEXPORT jboolean JNICALL
Java_com_quforia_QuestVuforiaManager_nativeIsDriverInitialized(
    JNIEnv* env, jobject thiz) {
    (void)env;   // Unused JNI parameter
    (void)thiz;  // Unused JNI parameter
    return (g_driverInstance != nullptr) ? JNI_TRUE : JNI_FALSE;
}

} // extern "C"
