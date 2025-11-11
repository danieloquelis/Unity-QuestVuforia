#ifndef QUEST_EXTERNAL_TRACKER_H
#define QUEST_EXTERNAL_TRACKER_H

#include <VuforiaEngine/Driver/Driver.h>
#include <thread>
#include <atomic>
#include <mutex>

// Forward declaration
class QuestVuforiaDriver;

/**
 * ExternalPositionalDeviceTracker implementation for Meta Quest 6DoF tracking.
 * Handles pose delivery to Vuforia Engine with coordinate system transformation.
 */
class QuestExternalTracker : public VuforiaDriver::ExternalPositionalDeviceTracker {
public:
    explicit QuestExternalTracker(QuestVuforiaDriver* driver);
    virtual ~QuestExternalTracker();

    // Lifecycle methods
    virtual bool open() override;
    virtual bool close() override;
    virtual bool start(VuforiaDriver::PoseCallback* cb, VuforiaDriver::AnchorCallback* anchorCb = nullptr) override;
    virtual bool stop() override;
    virtual bool resetTracking() override;

private:
    // Pose delivery thread
    void poseDeliveryThread();

    // Coordinate transformation: OpenXR to Vuforia CV convention
    void transformOpenXRToCV(const float* positionIn, const float* rotationIn,
                            float* positionOut, float* rotationOut);

    // Quaternion to rotation matrix conversion
    void quaternionToMatrix(const float* quat, float* matrix);

    QuestVuforiaDriver* driver_;
    VuforiaDriver::PoseCallback* callback_;

    std::thread poseThread_;
    std::atomic<bool> isRunning_;
    std::atomic<bool> isOpen_;

    int64_t lastPoseTimestamp_;
};

#endif // QUEST_EXTERNAL_TRACKER_H
