#ifndef QUEST_VUFORIA_DRIVER_H
#define QUEST_VUFORIA_DRIVER_H

#include <VuforiaEngine/Driver/Driver.h>
#include <mutex>
#include <queue>
#include <memory>
#include <atomic>

// Forward declarations
class QuestExternalCamera;
class QuestExternalTracker;

// Frame data structure for passing from Java/Kotlin layer
struct CameraFrameData {
    uint8_t* imageData;
    int width;
    int height;
    int64_t timestamp;  // Nanoseconds
    VuforiaDriver::CameraIntrinsics intrinsics;

    CameraFrameData() : imageData(nullptr), width(0), height(0), timestamp(0) {
        memset(&intrinsics, 0, sizeof(intrinsics));
    }

    ~CameraFrameData() {
        if (imageData) {
            delete[] imageData;
            imageData = nullptr;
        }
    }
};

// Pose data structure for 6DoF tracking
struct PoseData {
    int64_t timestamp;  // Nanoseconds
    float position[3];   // World space position (x, y, z)
    float rotation[4];   // Quaternion (x, y, z, w)

    PoseData() : timestamp(0) {
        position[0] = position[1] = position[2] = 0.0f;
        rotation[0] = rotation[1] = rotation[2] = 0.0f;
        rotation[3] = 1.0f;  // Identity quaternion
    }
};

// Main driver class implementing Vuforia Driver Framework
class QuestVuforiaDriver : public VuforiaDriver::Driver {
public:
    QuestVuforiaDriver(VuforiaDriver::PlatformData* platformData, void* userData);
    virtual ~QuestVuforiaDriver();

    // VuforiaDriver::Driver interface
    virtual uint32_t getCapabilities() override;
    virtual VuforiaDriver::ExternalCamera* createExternalCamera() override;
    virtual void destroyExternalCamera(VuforiaDriver::ExternalCamera* instance) override;
    virtual VuforiaDriver::ExternalPositionalDeviceTracker* createExternalPositionalDeviceTracker() override;
    virtual void destroyExternalPositionalDeviceTracker(VuforiaDriver::ExternalPositionalDeviceTracker* instance) override;

    // Frame and pose feeding methods (called from JNI)
    void feedCameraFrame(const uint8_t* imageData, int width, int height,
                        const float* intrinsics, int64_t timestamp);
    void feedDevicePose(const float* position, const float* rotation, int64_t timestamp);
    void setCameraIntrinsics(const float* intrinsics);

    // Frame buffer management
    std::shared_ptr<CameraFrameData> acquireLatestFrame();
    std::shared_ptr<PoseData> acquirePoseForTimestamp(int64_t timestamp);

private:
    QuestExternalCamera* camera_;
    QuestExternalTracker* tracker_;

    // Frame buffer (circular buffer, keep last 3 frames)
    std::mutex frameMutex_;
    std::queue<std::shared_ptr<CameraFrameData>> frameQueue_;
    static const size_t MAX_FRAME_QUEUE_SIZE = 3;

    // Pose buffer (keep last 90 poses ~ 3 seconds @ 30fps)
    std::mutex poseMutex_;
    std::queue<std::shared_ptr<PoseData>> poseQueue_;
    static const size_t MAX_POSE_QUEUE_SIZE = 90;

    // Cached intrinsics
    std::mutex intrinsicsMutex_;
    VuforiaDriver::CameraIntrinsics cachedIntrinsics_;
    bool intrinsicsSet_;
};

// Global driver instance (managed by Vuforia)
extern QuestVuforiaDriver* g_driverInstance;

#endif // QUEST_VUFORIA_DRIVER_H
