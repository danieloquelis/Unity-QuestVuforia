#include "external_camera.h"
#include "vuforia_driver.h"
#include <android/log.h>
#include <chrono>
#include <thread>
#include <cstring>

#define LOG_TAG "QuestExternalCamera"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

QuestExternalCamera::QuestExternalCamera(QuestVuforiaDriver* driver)
    : driver_(driver)
    , callback_(nullptr)
    , isRunning_(false)
    , isOpen_(false)
    , exposureMode_(VuforiaDriver::ExposureMode::CONTINUOUS_AUTO)
    , focusMode_(VuforiaDriver::FocusMode::CONTINUOUS_AUTO)
    , frameBuffer_(nullptr)
{
    LOGI("QuestExternalCamera constructor");

    // Initialize default camera mode (1280x960 @ 30fps RGB888)
    currentMode_.width = 1280;
    currentMode_.height = 960;
    currentMode_.fps = 30;
    currentMode_.format = VuforiaDriver::PixelFormat::RGB888;
}

QuestExternalCamera::~QuestExternalCamera() {
    LOGI("QuestExternalCamera destructor");

    stop();
    close();

    if (frameBuffer_) {
        delete[] frameBuffer_;
        frameBuffer_ = nullptr;
    }
}

// =============================================================================
// Lifecycle Methods
// =============================================================================

bool QuestExternalCamera::open() {
    LOGI("open()");

    if (isOpen_) {
        LOGW("Camera already open");
        return true;
    }

    // Allocate frame buffer
    int bufferSize = currentMode_.width * currentMode_.height * 3;  // RGB888
    frameBuffer_ = new uint8_t[bufferSize];

    if (!frameBuffer_) {
        LOGE("Failed to allocate frame buffer");
        return false;
    }

    isOpen_ = true;
    LOGI("Camera opened successfully");
    return true;
}

bool QuestExternalCamera::close() {
    LOGI("close()");

    if (!isOpen_) {
        LOGD("Camera already closed");
        return true;
    }

    // Stop frame delivery if running
    if (isRunning_) {
        stop();
    }

    // Free frame buffer
    if (frameBuffer_) {
        std::lock_guard<std::mutex> lock(bufferMutex_);
        delete[] frameBuffer_;
        frameBuffer_ = nullptr;
    }

    isOpen_ = false;
    LOGI("Camera closed");
    return true;
}

bool QuestExternalCamera::start(VuforiaDriver::CameraMode mode,
                                VuforiaDriver::CameraCallback* callback) {
    LOGI("start() with mode: %ux%u@%ufps, format=%d",
         mode.width, mode.height, mode.fps, mode.format);

    if (!isOpen_) {
        LOGE("Camera not open");
        return false;
    }

    if (isRunning_) {
        LOGW("Camera already running");
        return true;
    }

    if (callback == nullptr) {
        LOGE("Callback is null");
        return false;
    }

    // Validate mode (we only support one mode for now)
    if (mode.width != currentMode_.width ||
        mode.height != currentMode_.height ||
        mode.format != currentMode_.format) {
        LOGE("Unsupported camera mode: %ux%u, format=%d",
             mode.width, mode.height, mode.format);
        return false;
    }

    currentMode_ = mode;
    callback_ = callback;
    isRunning_ = true;

    // Start frame delivery thread
    frameThread_ = std::thread(&QuestExternalCamera::frameDeliveryThread, this);

    LOGI("Camera started successfully");
    return true;
}

bool QuestExternalCamera::stop() {
    LOGI("stop()");

    if (!isRunning_) {
        LOGD("Camera not running");
        return true;
    }

    // Signal thread to stop
    isRunning_ = false;

    // Wait for thread to finish
    if (frameThread_.joinable()) {
        frameThread_.join();
    }

    callback_ = nullptr;
    LOGI("Camera stopped");
    return true;
}

// =============================================================================
// Camera Mode Query
// =============================================================================

uint32_t QuestExternalCamera::getNumSupportedCameraModes() {
    // We support one camera mode: 1280x960@30fps RGB888
    return 1;
}

bool QuestExternalCamera::getSupportedCameraMode(uint32_t index,
                                                 VuforiaDriver::CameraMode* cameraMode) {
    if (index != 0 || cameraMode == nullptr) {
        return false;
    }

    // Return the single supported mode
    cameraMode->width = 1280;
    cameraMode->height = 960;
    cameraMode->fps = 30;
    cameraMode->format = VuforiaDriver::PixelFormat::RGB888;

    LOGD("getSupportedCameraMode(%u): %ux%u@%ufps",
         index, cameraMode->width, cameraMode->height, cameraMode->fps);
    return true;
}

// =============================================================================
// Exposure Control
// =============================================================================

bool QuestExternalCamera::supportsExposureMode(VuforiaDriver::ExposureMode mode) {
    // Support continuous auto exposure only
    return (mode == VuforiaDriver::ExposureMode::CONTINUOUS_AUTO);
}

VuforiaDriver::ExposureMode QuestExternalCamera::getExposureMode() {
    return exposureMode_;
}

bool QuestExternalCamera::setExposureMode(VuforiaDriver::ExposureMode mode) {
    if (!supportsExposureMode(mode)) {
        LOGW("Unsupported exposure mode: %d", mode);
        return false;
    }

    exposureMode_ = mode;
    LOGD("Exposure mode set to: %d", mode);
    return true;
}

// =============================================================================
// Focus Control
// =============================================================================

bool QuestExternalCamera::supportsFocusMode(VuforiaDriver::FocusMode mode) {
    // Support continuous auto focus only
    return (mode == VuforiaDriver::FocusMode::CONTINUOUS_AUTO);
}

VuforiaDriver::FocusMode QuestExternalCamera::getFocusMode() {
    return focusMode_;
}

bool QuestExternalCamera::setFocusMode(VuforiaDriver::FocusMode mode) {
    if (!supportsFocusMode(mode)) {
        LOGW("Unsupported focus mode: %d", mode);
        return false;
    }

    focusMode_ = mode;
    LOGD("Focus mode set to: %d", mode);
    return true;
}

// =============================================================================
// Manual Exposure Value Control (Not Supported)
// =============================================================================

bool QuestExternalCamera::supportsExposureValue() {
    // Manual exposure control not supported for Quest passthrough camera
    return false;
}

uint64_t QuestExternalCamera::getExposureValueMin() {
    return 0;
}

uint64_t QuestExternalCamera::getExposureValueMax() {
    return 0;
}

uint64_t QuestExternalCamera::getExposureValue() {
    return 33333333;  // Return nominal 33.33ms @ 30fps
}

bool QuestExternalCamera::setExposureValue(uint64_t exposureTime) {
    (void)exposureTime;  // Unused
    LOGW("Manual exposure value control not supported");
    return false;
}

// =============================================================================
// Manual Focus Value Control (Not Supported)
// =============================================================================

bool QuestExternalCamera::supportsFocusValue() {
    // Manual focus control not supported for Quest passthrough camera
    return false;
}

float QuestExternalCamera::getFocusValueMin() {
    return 0.0f;
}

float QuestExternalCamera::getFocusValueMax() {
    return 0.0f;
}

float QuestExternalCamera::getFocusValue() {
    return 0.0f;
}

bool QuestExternalCamera::setFocusValue(float focusValue) {
    (void)focusValue;  // Unused
    LOGW("Manual focus value control not supported");
    return false;
}

// =============================================================================
// Frame Delivery Thread
// =============================================================================

void QuestExternalCamera::frameDeliveryThread() {
    LOGI("Frame delivery thread started");

    const int targetFPS = currentMode_.fps;
    const auto frameDuration = std::chrono::milliseconds(1000 / targetFPS);

    int frameCount = 0;

    while (isRunning_) {
        auto frameStartTime = std::chrono::steady_clock::now();

        // Acquire latest frame from driver
        auto frameData = driver_->acquireLatestFrame();

        if (frameData && callback_) {
            // Prepare Vuforia frame structure
            VuforiaDriver::CameraFrame vuforiaFrame;
            memset(&vuforiaFrame, 0, sizeof(vuforiaFrame));

            // Set frame data
            vuforiaFrame.buffer = frameData->imageData;
            vuforiaFrame.width = frameData->width;
            vuforiaFrame.height = frameData->height;
            vuforiaFrame.stride = frameData->width * 3;  // RGB888: 3 bytes per pixel
            vuforiaFrame.bufferSize = vuforiaFrame.stride * frameData->height;
            vuforiaFrame.format = VuforiaDriver::PixelFormat::RGB888;
            vuforiaFrame.timestamp = frameData->timestamp;
            vuforiaFrame.exposureTime = 33333333;  // 33.33ms @ 30fps (nanoseconds)
            vuforiaFrame.intrinsics = frameData->intrinsics;

            // Deliver frame to Vuforia (pass pointer, not value)
            callback_->onNewCameraFrame(&vuforiaFrame);

            frameCount++;
            if (frameCount % 30 == 0) {
                LOGD("Delivered %d frames (latest timestamp: %lld)",
                     frameCount, (long long)frameData->timestamp);
            }
        } else {
            // No frame available, wait a bit
            LOGD("No frame available from driver");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // Sleep to maintain target frame rate
        auto frameEndTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            frameEndTime - frameStartTime);

        if (elapsed < frameDuration) {
            std::this_thread::sleep_for(frameDuration - elapsed);
        }
    }

    LOGI("Frame delivery thread stopped (delivered %d frames)", frameCount);
}
