#include "vuforia_engine_wrapper.h"
#include <android/log.h>
#include <cstring>

#define LOG_TAG "QuestVuforia"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

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

    VuErrorCode error;
    if (vuEngineCreate(&engine_, configSet, &error) != VU_SUCCESS) {
        LOGE("Failed to create Vuforia Engine: %d", error);
        vuEngineConfigSetDestroy(configSet);
        return false;
    }

    vuEngineConfigSetDestroy(configSet);

    if (vuEngineStart(engine_) != VU_SUCCESS) {
        LOGE("Failed to start Vuforia Engine");
        vuEngineDestroy(engine_);
        engine_ = nullptr;
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

    initialized_ = false;
    LOGI("Vuforia Engine shut down");
}

bool VuforiaEngineWrapper::processFrame(const uint8_t* imageData, int width, int height,
                                       int format, const VuCameraIntrinsics& intrinsics,
                                       int64_t timestamp) {
    return initialized_;
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
