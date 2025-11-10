#include "vuforia_engine_wrapper.h"
#include <android/log.h>
#include <cstring>

#define LOG_TAG "QuestVuforia"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace VuforiaDriver;

class ExternalCameraImpl : public ExternalCamera {
public:
    ExternalCameraImpl() = default;
    ~ExternalCameraImpl() override = default;

    bool open() override {
        LOGI("ExternalCamera::open()");
        return true;
    }

    bool start(CameraMode cameraMode, CameraCallback* cb) override {
        LOGI("ExternalCamera::start()");
        callback_ = cb;
        return true;
    }

    bool stop() override {
        LOGI("ExternalCamera::stop()");
        callback_ = nullptr;
        return true;
    }

    bool close() override {
        LOGI("ExternalCamera::close()");
        return true;
    }

    uint32_t getNumSupportedCameraModes() override {
        return 1;
    }

    bool getSupportedCameraMode(uint32_t index, CameraMode* out) override {
        if (index != 0 || !out) return false;
        out->width = 1280;
        out->height = 960;
        out->fps = 30;
        out->format = PixelFormat::RGB888;
        return true;
    }

    bool supportsExposureMode(ExposureMode) override { return false; }
    ExposureMode getExposureMode() override { return ExposureMode::UNKNOWN; }
    bool setExposureMode(ExposureMode) override { return false; }
    bool getExposureValueRange(uint64_t*, uint64_t*) override { return false; }
    uint64_t getExposureValue() override { return 0; }
    bool setExposureValue(uint64_t) override { return false; }
    bool supportsFocusMode(FocusMode) override { return false; }
    FocusMode getFocusMode() override { return FocusMode::UNKNOWN; }
    bool setFocusMode(FocusMode) override { return false; }
    bool getFocusValueRange(float*, float*) override { return false; }
    float getFocusValue() override { return 0.0f; }
    bool setFocusValue(float) override { return false; }

    void deliverFrame(const uint8_t* data, int width, int height, PixelFormat format,
                     const CameraIntrinsics& intrinsics, int64_t timestamp) {
        if (!callback_) return;

        CameraFrame frame;
        frame.timestamp = timestamp;
        frame.exposureTime = 0;
        frame.buffer = const_cast<uint8_t*>(data);
        frame.bufferSize = width * height * 3;
        frame.index = frameIndex_++;
        frame.width = width;
        frame.height = height;
        frame.stride = width * 3;
        frame.format = format;
        frame.intrinsics = intrinsics;

        callback_->onNewCameraFrame(&frame);
    }

private:
    CameraCallback* callback_ = nullptr;
    uint32_t frameIndex_ = 0;
};

VuforiaEngineWrapper& VuforiaEngineWrapper::getInstance() {
    static VuforiaEngineWrapper instance;
    return instance;
}

VuforiaEngineWrapper::~VuforiaEngineWrapper() {
    shutdown();
}

bool VuforiaEngineWrapper::initialize(const char* licenseKey, JavaVM* javaVM, jobject activity) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (initialized_) {
        LOGE("Vuforia already initialized");
        return false;
    }

    VuEngineConfigSet* configSet = nullptr;
    if (vuEngineConfigSetCreate(&configSet) != VU_SUCCESS) {
        LOGE("Failed to create engine config set");
        return false;
    }

    VuLicenseConfig licenseConfig = { licenseKey };
    vuEngineConfigSetAddLicenseConfig(configSet, &licenseConfig);

    VuPlatformAndroidConfig platformConfig;
    platformConfig.javaVM = javaVM;
    platformConfig.activity = activity;
    vuEngineConfigSetAddPlatformAndroidConfig(configSet, &platformConfig);

    externalCamera_ = new ExternalCameraImpl();

    VuDriverConfig driverConfig;
    driverConfig.externalCamera = externalCamera_;
    driverConfig.platformData.javaVM = javaVM;
    driverConfig.platformData.activity = activity;
    driverConfig.platformData.jniVersion = JNI_VERSION_1_6;

    vuEngineConfigSetAddDriverConfig(configSet, &driverConfig);

    VuEngineCreationError error;
    if (vuEngineCreate(&engine_, configSet, &error) != VU_SUCCESS) {
        LOGE("Failed to create Vuforia Engine: %d", error);
        vuEngineConfigSetDestroy(configSet);
        delete externalCamera_;
        externalCamera_ = nullptr;
        return false;
    }

    vuEngineConfigSetDestroy(configSet);

    if (vuEngineStart(engine_) != VU_SUCCESS) {
        LOGE("Failed to start Vuforia Engine");
        vuEngineDestroy(engine_);
        engine_ = nullptr;
        delete externalCamera_;
        externalCamera_ = nullptr;
        return false;
    }

    initialized_ = true;
    LOGI("Vuforia Engine initialized successfully");
    return true;
}

void VuforiaEngineWrapper::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) return;

    if (engine_) {
        vuEngineStop(engine_);
        vuEngineDestroy(engine_);
        engine_ = nullptr;
    }

    if (externalCamera_) {
        delete externalCamera_;
        externalCamera_ = nullptr;
    }

    initialized_ = false;
    LOGI("Vuforia Engine shut down");
}

bool VuforiaEngineWrapper::processFrame(const uint8_t* imageData, int width, int height,
                                       int format, const VuCameraIntrinsics& intrinsics,
                                       int64_t timestamp) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_ || !externalCamera_) {
        return false;
    }

    CameraIntrinsics driverIntrinsics;
    driverIntrinsics.size[0] = intrinsics.size.data[0];
    driverIntrinsics.size[1] = intrinsics.size.data[1];
    driverIntrinsics.focalLength[0] = intrinsics.focalLength.data[0];
    driverIntrinsics.focalLength[1] = intrinsics.focalLength.data[1];
    driverIntrinsics.principalPoint[0] = intrinsics.principalPoint.data[0];
    driverIntrinsics.principalPoint[1] = intrinsics.principalPoint.data[1];

    for (int i = 0; i < 8; i++) {
        driverIntrinsics.distortionCoefficients[i] = intrinsics.distortionParameters.data[i];
    }

    PixelFormat pixelFormat = PixelFormat::RGB888;

    static_cast<ExternalCameraImpl*>(externalCamera_)->deliverFrame(
        imageData, width, height, pixelFormat, driverIntrinsics, timestamp
    );

    return true;
}

VuState* VuforiaEngineWrapper::acquireLatestState() {
    if (!initialized_ || !engine_) return nullptr;

    VuState* state = nullptr;
    if (vuEngineAcquireLatestState(engine_, &state) == VU_SUCCESS) {
        return state;
    }
    return nullptr;
}

void VuforiaEngineWrapper::releaseState(VuState* state) {
    if (state) {
        vuStateRelease(state);
    }
}
