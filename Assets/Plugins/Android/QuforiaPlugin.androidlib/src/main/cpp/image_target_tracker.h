#pragma once

#include <VuforiaEngine/VuforiaEngine.h>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

struct ImageTargetResult {
    char name[256];
    float poseMatrix[16];
    int status;
};

class ImageTargetTracker {
public:
    ImageTargetTracker(VuEngine* engine);
    ~ImageTargetTracker();

    bool loadDatabase(const char* databasePath);
    bool createTargetObserver(const char* targetName);
    void destroyTargetObserver(const char* targetName);
    void destroyAllObservers();

    int getTrackedTargets(ImageTargetResult* results, int maxResults);

private:
    VuEngine* engine_;
    VuObserverList* observers_;
    std::string databasePath_;
    std::mutex mutex_;
};
