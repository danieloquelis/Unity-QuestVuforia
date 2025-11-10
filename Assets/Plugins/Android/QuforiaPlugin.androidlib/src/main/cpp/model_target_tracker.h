#pragma once

#include <VuforiaEngine/VuforiaEngine.h>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

struct ModelTargetResult {
    char name[256];
    float poseMatrix[16];
    int status;
};

class ModelTargetTracker {
public:
    ModelTargetTracker(VuEngine* engine);
    ~ModelTargetTracker();

    bool loadDatabase(const char* databasePath);
    bool createTargetObserver(const char* targetName, const char* guideViewName);
    void destroyTargetObserver(const char* targetName);
    void destroyAllObservers();

    int getTrackedTargets(ModelTargetResult* results, int maxResults);

private:
    VuEngine* engine_;
    VuObserverList* observers_;
    std::string databasePath_;
    std::mutex mutex_;
};
