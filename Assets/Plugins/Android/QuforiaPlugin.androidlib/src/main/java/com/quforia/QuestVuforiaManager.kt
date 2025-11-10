package com.quforia

import android.app.Activity
import android.util.Log

/**
 * QuestVuforiaManager - Bridge between Unity and Vuforia Driver Framework
 *
 * This class acts as a simple bridge to feed camera frames and device poses
 * to the native Vuforia Driver. All Vuforia initialization and target tracking
 * is handled by the Vuforia Unity SDK using the Driver Framework.
 *
 * Architecture:
 * Unity C# (MetaCameraProvider) → QuestVuforiaManager.kt → Native Driver (C++)
 */
class QuestVuforiaManager(private val activity: Activity) {

    companion object {
        private const val TAG = "QuestVuforiaManager"

        init {
            try {
                System.loadLibrary("quforia")
                Log.i(TAG, "Native library libquforia.so loaded successfully")
            } catch (e: UnsatisfiedLinkError) {
                Log.e(TAG, "Failed to load native library: ${e.message}")
                throw e
            }
        }
    }

    /**
     * Set camera intrinsics (called once at initialization)
     * Intrinsics array format: [width, height, fx, fy, cx, cy, d0-d7]
     *
     * @param intrinsics Camera intrinsics array (14 floats)
     * @return true if successful
     */
    fun setCameraIntrinsics(intrinsics: FloatArray): Boolean {
        if (intrinsics.size < 6) {
            Log.e(TAG, "Intrinsics array too small: ${intrinsics.size}, expected at least 6")
            return false
        }

        val result = nativeSetCameraIntrinsics(intrinsics)
        if (result) {
            Log.i(TAG, "Camera intrinsics set: ${intrinsics[0].toInt()}x${intrinsics[1].toInt()}, " +
                      "fx=${intrinsics[2]}, fy=${intrinsics[3]}, cx=${intrinsics[4]}, cy=${intrinsics[5]}")
        } else {
            Log.e(TAG, "Failed to set camera intrinsics")
        }
        return result
    }

    /**
     * Feed device pose to the Vuforia Driver
     * CRITICAL: Must be called BEFORE feedCameraFrame with the same timestamp
     *
     * @param position Camera position in world space [x, y, z]
     * @param rotation Camera rotation as quaternion [x, y, z, w]
     * @param timestamp Frame timestamp in nanoseconds
     * @return true if successful
     */
    fun feedDevicePose(
        position: FloatArray,
        rotation: FloatArray,
        timestamp: Long
    ): Boolean {
        if (position.size != 3) {
            Log.e(TAG, "Position array must have 3 elements")
            return false
        }
        if (rotation.size != 4) {
            Log.e(TAG, "Rotation array must have 4 elements (quaternion)")
            return false
        }

        return nativeFeedDevicePose(position, rotation, timestamp)
    }

    /**
     * Feed camera frame to the Vuforia Driver
     * Must be called AFTER feedDevicePose with the same timestamp
     *
     * @param imageData RGB image data (width * height * 3 bytes)
     * @param width Image width in pixels
     * @param height Image height in pixels
     * @param intrinsics Camera intrinsics (optional, uses cached if null)
     * @param timestamp Frame timestamp in nanoseconds
     * @return true if successful
     */
    fun feedCameraFrame(
        imageData: ByteArray,
        width: Int,
        height: Int,
        intrinsics: FloatArray?,
        timestamp: Long
    ): Boolean {
        val expectedSize = width * height * 3  // RGB888
        if (imageData.size != expectedSize) {
            Log.e(TAG, "Image data size mismatch: ${imageData.size}, expected $expectedSize")
            return false
        }

        // Use empty array if intrinsics not provided (will use cached)
        val intrinsicsArray = intrinsics ?: FloatArray(0)

        return nativeFeedCameraFrame(imageData, width, height, intrinsicsArray, timestamp)
    }

    /**
     * Check if the native driver is initialized
     * @return true if driver is ready
     */
    fun isDriverInitialized(): Boolean {
        return nativeIsDriverInitialized()
    }

    // =========================================================================
    // Native Method Declarations
    // =========================================================================

    /**
     * Set camera intrinsics in the native driver
     * Format: [width, height, fx, fy, cx, cy, d0, d1, d2, d3, d4, d5, d6, d7]
     */
    private external fun nativeSetCameraIntrinsics(intrinsics: FloatArray): Boolean

    /**
     * Feed device pose to the native driver
     * Must be called before feedCameraFrame with same timestamp
     */
    private external fun nativeFeedDevicePose(
        position: FloatArray,
        rotation: FloatArray,
        timestamp: Long
    ): Boolean

    /**
     * Feed camera frame to the native driver
     */
    private external fun nativeFeedCameraFrame(
        imageData: ByteArray,
        width: Int,
        height: Int,
        intrinsics: FloatArray,
        timestamp: Long
    ): Boolean

    /**
     * Check if driver is initialized
     */
    private external fun nativeIsDriverInitialized(): Boolean
}
