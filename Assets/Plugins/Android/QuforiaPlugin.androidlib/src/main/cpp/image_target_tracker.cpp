#include "image_target_tracker.h"
#include <android/log.h>
#include <cstring>

#define LOG_TAG "QuestVuforia"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

ImageTargetTracker::ImageTargetTracker(VuEngine* engine)
    : engine_(engine), observers_(nullptr) {
    if (vuObserverListCreate(&observers_) != VU_SUCCESS) {
        LOGE("Failed to create observer list");
    }
}

ImageTargetTracker::~ImageTargetTracker() {
    destroyAllObservers();
    if (observers_) {
        vuObserverListDestroy(observers_);
    }
}

bool ImageTargetTracker::loadDatabase(const char* databasePath) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!engine_ || !databasePath) {
        LOGE("Invalid engine or database path");
        return false;
    }

    VuDatabaseTargetInfoList* targetInfoList = nullptr;
    if (vuDatabaseTargetInfoListCreate(&targetInfoList) != VU_SUCCESS) {
        LOGE("Failed to create target info list");
        return false;
    }

    VuDatabaseTargetInfoError error;
    VuResult result = vuEngineGetDatabaseTargetInfo(engine_, databasePath, targetInfoList, &error);

    if (result != VU_SUCCESS) {
        LOGE("Failed to load database: %s, error: %d", databasePath, error);
        vuDatabaseTargetInfoListDestroy(targetInfoList);
        return false;
    }

    int32_t numTargets = 0;
    vuDatabaseTargetInfoListGetSize(targetInfoList, &numTargets);
    LOGI("Database loaded with %d targets", numTargets);

    vuDatabaseTargetInfoListDestroy(targetInfoList);

    databasePath_ = databasePath;
    return true;
}

bool ImageTargetTracker::createTargetObserver(const char* targetName) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!engine_ || !targetName) {
        LOGE("Invalid engine or target name");
        return false;
    }

    if (databasePath_.empty()) {
        LOGE("Database not loaded. Call loadDatabase first.");
        return false;
    }

    VuImageTargetConfig config = vuImageTargetConfigDefault();
    config.databasePath = databasePath_.c_str();
    config.targetName = targetName;
    config.activate = VU_TRUE;

    VuObserver* observer = nullptr;
    VuImageTargetCreationError error;
    VuResult result = vuEngineCreateImageTargetObserver(engine_, &observer, &config, &error);

    if (result != VU_SUCCESS) {
        LOGE("Failed to create image target observer for %s: %d", targetName, error);
        return false;
    }

    LOGI("Created image target observer: %s", targetName);
    return true;
}

void ImageTargetTracker::destroyTargetObserver(const char* targetName) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!observers_ || !targetName) return;

    int32_t numObservers = 0;
    vuObserverListGetSize(observers_, &numObservers);

    for (int32_t i = 0; i < numObservers; i++) {
        VuObserver* observer = nullptr;
        vuObserverListGetElement(observers_, i, &observer);

        const char* obsTargetName = nullptr;
        vuImageTargetObserverGetTargetName(observer, &obsTargetName);

        if (obsTargetName && strcmp(obsTargetName, targetName) == 0) {
            vuObserverDestroy(observer);
            LOGI("Destroyed image target observer: %s", targetName);
            return;
        }
    }
}

void ImageTargetTracker::destroyAllObservers() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!observers_) return;

    int32_t numObservers = 0;
    vuObserverListGetSize(observers_, &numObservers);

    for (int32_t i = 0; i < numObservers; i++) {
        VuObserver* observer = nullptr;
        vuObserverListGetElement(observers_, i, &observer);
        if (observer) {
            vuObserverDestroy(observer);
        }
    }

    LOGI("Destroyed all image target observers");
}

int ImageTargetTracker::getTrackedTargets(ImageTargetResult* results, int maxResults) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!engine_ || !results || maxResults <= 0) {
        return 0;
    }

    VuState* state = nullptr;
    if (vuEngineAcquireLatestState(engine_, &state) != VU_SUCCESS) {
        return 0;
    }

    VuObservationList* observations = nullptr;
    if (vuObservationListCreate(&observations) != VU_SUCCESS) {
        vuStateRelease(state);
        return 0;
    }

    if (vuStateGetObservations(state, observations) != VU_SUCCESS) {
        vuObservationListDestroy(observations);
        vuStateRelease(state);
        return 0;
    }

    int32_t numObservations = 0;
    vuObservationListGetSize(observations, &numObservations);

    int resultCount = 0;
    for (int32_t i = 0; i < numObservations && resultCount < maxResults; i++) {
        VuObservation* observation = nullptr;
        vuObservationListGetElement(observations, i, &observation);

        VuObservationType obsType;
        vuObservationGetType(observation, &obsType);

        if (obsType != VU_OBSERVATION_IMAGE_TARGET_TYPE) {
            continue;
        }

        VuPoseInfo poseInfo;
        if (vuObservationGetPoseInfo(observation, &poseInfo) != VU_SUCCESS) {
            continue;
        }

        if (poseInfo.poseStatus != VU_OBSERVATION_POSE_STATUS_TRACKED &&
            poseInfo.poseStatus != VU_OBSERVATION_POSE_STATUS_EXTENDED_TRACKED) {
            continue;
        }

        VuImageTargetObservationTargetInfo targetInfo;
        if (vuImageTargetObservationGetTargetInfo(observation, &targetInfo) == VU_SUCCESS && targetInfo.name) {
            strncpy(results[resultCount].name, targetInfo.name, sizeof(results[resultCount].name) - 1);
            results[resultCount].name[sizeof(results[resultCount].name) - 1] = '\0';
        }

        const float* matrix = poseInfo.pose.data;
        for (int j = 0; j < 16; j++) {
            results[resultCount].poseMatrix[j] = matrix[j];
        }

        VuImageTargetObservationStatusInfo statusInfo;
        if (vuImageTargetObservationGetStatusInfo(observation, &statusInfo) == VU_SUCCESS) {
            results[resultCount].status = statusInfo;
        } else {
            results[resultCount].status = 0;
        }

        resultCount++;
    }

    vuObservationListDestroy(observations);
    vuStateRelease(state);
    return resultCount;
}
