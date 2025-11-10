#pragma once

#include <VuforiaEngine/VuforiaEngine.h>
#include <VuforiaEngine/Driver/Driver.h>
#include <memory>
#include <string>
#include <mutex>

class VuforiaEngineWrapper {
public:
    static VuforiaEngineWrapper& getInstance();

    bool initialize(const char* licenseKey, JavaVM* javaVM, jobject activity);
    void shutdown();

    bool processFrame(const uint8_t* imageData, int width, int height, int format,
                     const VuCameraIntrinsics& intrinsics, int64_t timestamp);

    VuEngine* getEngine() const { return engine_; }
    bool isInitialized() const { return initialized_; }

    VuState* acquireLatestState();
    void releaseState(VuState* state);

private:
    VuforiaEngineWrapper() = default;
    ~VuforiaEngineWrapper();

    VuEngine* engine_ = nullptr;
    VuforiaDriver::ExternalCamera* externalCamera_ = nullptr;
    bool initialized_ = false;
    std::mutex mutex_;
};
