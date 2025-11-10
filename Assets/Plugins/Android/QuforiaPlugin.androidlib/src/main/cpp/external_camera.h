#ifndef QUEST_EXTERNAL_CAMERA_H
#define QUEST_EXTERNAL_CAMERA_H

#include <VuforiaEngine/Driver/Driver.h>
#include <thread>
#include <atomic>
#include <mutex>

// Forward declaration
class QuestVuforiaDriver;

/**
 * ExternalCamera implementation for Meta Quest passthrough camera.
 * Handles camera lifecycle and frame delivery to Vuforia Engine.
 */
class QuestExternalCamera : public VuforiaDriver::ExternalCamera {
public:
    explicit QuestExternalCamera(QuestVuforiaDriver* driver);
    virtual ~QuestExternalCamera();

    // Lifecycle methods
    virtual bool open() override;
    virtual bool close() override;
    virtual bool start(VuforiaDriver::CameraMode mode,
                      VuforiaDriver::CameraCallback* callback) override;
    virtual bool stop() override;

    // Camera mode query
    virtual uint32_t getNumSupportedCameraModes() override;
    virtual bool getSupportedCameraMode(uint32_t index,
                                       VuforiaDriver::CameraMode* cameraMode) override;

    // Exposure control (required)
    virtual bool supportsExposureMode(VuforiaDriver::ExposureMode mode) override;
    virtual VuforiaDriver::ExposureMode getExposureMode() override;
    virtual bool setExposureMode(VuforiaDriver::ExposureMode mode) override;
    virtual bool supportsExposureValue() override;
    virtual uint64_t getExposureValueMin() override;
    virtual uint64_t getExposureValueMax() override;
    virtual uint64_t getExposureValue() override;
    virtual bool setExposureValue(uint64_t exposureTime) override;

    // Focus control (required)
    virtual bool supportsFocusMode(VuforiaDriver::FocusMode mode) override;
    virtual VuforiaDriver::FocusMode getFocusMode() override;
    virtual bool setFocusMode(VuforiaDriver::FocusMode mode) override;
    virtual bool supportsFocusValue() override;
    virtual float getFocusValueMin() override;
    virtual float getFocusValueMax() override;
    virtual float getFocusValue() override;
    virtual bool setFocusValue(float focusValue) override;

private:
    // Frame delivery thread
    void frameDeliveryThread();

    QuestVuforiaDriver* driver_;
    VuforiaDriver::CameraCallback* callback_;
    VuforiaDriver::CameraMode currentMode_;

    std::thread frameThread_;
    std::atomic<bool> isRunning_;
    std::atomic<bool> isOpen_;

    // Exposure and focus settings
    VuforiaDriver::ExposureMode exposureMode_;
    VuforiaDriver::FocusMode focusMode_;

    // Frame buffer for conversion
    uint8_t* frameBuffer_;
    std::mutex bufferMutex_;
};

#endif // QUEST_EXTERNAL_CAMERA_H
