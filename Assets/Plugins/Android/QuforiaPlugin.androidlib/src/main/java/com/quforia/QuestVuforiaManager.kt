package com.quforia

import android.app.Activity
import android.util.Log

class QuestVuforiaManager(private val activity: Activity) {

    companion object {
        private const val TAG = "QuestVuforiaManager"

        init {
            System.loadLibrary("quforia")
        }
    }

    private var initialized = false

    fun initialize(licenseKey: String): Boolean {
        if (initialized) {
            Log.w(TAG, "Already initialized")
            return true
        }

        initialized = nativeInitialize(licenseKey)
        if (initialized) {
            Log.i(TAG, "Vuforia initialized successfully")
        } else {
            Log.e(TAG, "Failed to initialize Vuforia")
        }
        return initialized
    }

    fun shutdown() {
        if (!initialized) return
        nativeShutdown()
        initialized = false
        Log.i(TAG, "Vuforia shut down")
    }

    fun processFrame(
        imageData: ByteArray,
        width: Int,
        height: Int,
        intrinsics: FloatArray,
        timestamp: Long
    ): Boolean {
        if (!initialized) {
            Log.w(TAG, "Not initialized")
            return false
        }
        return nativeProcessFrame(imageData, width, height, intrinsics, timestamp)
    }

    fun loadImageTargetDatabase(databasePath: String): Boolean {
        if (!initialized) {
            Log.w(TAG, "Not initialized")
            return false
        }
        val result = nativeLoadImageTargetDatabase(databasePath)
        if (result) {
            Log.i(TAG, "Image target database loaded: $databasePath")
        } else {
            Log.e(TAG, "Failed to load image target database: $databasePath")
        }
        return result
    }

    fun createImageTarget(targetName: String): Boolean {
        if (!initialized) {
            Log.w(TAG, "Not initialized")
            return false
        }
        val result = nativeCreateImageTarget(targetName)
        if (result) {
            Log.i(TAG, "Image target created: $targetName")
        } else {
            Log.e(TAG, "Failed to create image target: $targetName")
        }
        return result
    }

    fun destroyImageTarget(targetName: String) {
        if (!initialized) return
        nativeDestroyImageTarget(targetName)
        Log.i(TAG, "Image target destroyed: $targetName")
    }

    fun getImageTargetResults(): Array<TrackingResult> {
        if (!initialized) return emptyArray()
        return nativeGetImageTargetResults()
    }

    fun loadModelTargetDatabase(databasePath: String): Boolean {
        if (!initialized) {
            Log.w(TAG, "Not initialized")
            return false
        }
        val result = nativeLoadModelTargetDatabase(databasePath)
        if (result) {
            Log.i(TAG, "Model target database loaded: $databasePath")
        } else {
            Log.e(TAG, "Failed to load model target database: $databasePath")
        }
        return result
    }

    fun createModelTarget(targetName: String, guideViewName: String? = null): Boolean {
        if (!initialized) {
            Log.w(TAG, "Not initialized")
            return false
        }
        val result = nativeCreateModelTarget(targetName, guideViewName)
        if (result) {
            Log.i(TAG, "Model target created: $targetName")
        } else {
            Log.e(TAG, "Failed to create model target: $targetName")
        }
        return result
    }

    fun destroyModelTarget(targetName: String) {
        if (!initialized) return
        nativeDestroyModelTarget(targetName)
        Log.i(TAG, "Model target destroyed: $targetName")
    }

    fun getModelTargetResults(): Array<TrackingResult> {
        if (!initialized) return emptyArray()
        return nativeGetModelTargetResults()
    }

    private external fun nativeInitialize(licenseKey: String): Boolean
    private external fun nativeShutdown()
    private external fun nativeProcessFrame(
        imageData: ByteArray,
        width: Int,
        height: Int,
        intrinsics: FloatArray,
        timestamp: Long
    ): Boolean
    private external fun nativeLoadImageTargetDatabase(databasePath: String): Boolean
    private external fun nativeCreateImageTarget(targetName: String): Boolean
    private external fun nativeDestroyImageTarget(targetName: String)
    private external fun nativeGetImageTargetResults(): Array<TrackingResult>
    private external fun nativeLoadModelTargetDatabase(databasePath: String): Boolean
    private external fun nativeCreateModelTarget(targetName: String, guideViewName: String?): Boolean
    private external fun nativeDestroyModelTarget(targetName: String)
    private external fun nativeGetModelTargetResults(): Array<TrackingResult>
}