#include <jni.h>
#include <android/log.h>
#include "vuforia_engine_wrapper.h"
#include "image_target_tracker.h"
#include "model_target_tracker.h"

#define LOG_TAG "QuestVuforia"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static ImageTargetTracker* g_imageTargetTracker = nullptr;
static ModelTargetTracker* g_modelTargetTracker = nullptr;

extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_quforia_QuestVuforiaManager_nativeInitialize(
    JNIEnv* env, jobject thiz, jstring licenseKey) {

    const char* key = env->GetStringUTFChars(licenseKey, nullptr);
    if (!key) {
        LOGE("Failed to get license key");
        return JNI_FALSE;
    }

    JavaVM* javaVM = nullptr;
    env->GetJavaVM(&javaVM);

    jobject activity = env->NewGlobalRef(thiz);

    auto& wrapper = VuforiaEngineWrapper::getInstance();
    bool success = wrapper.initialize(key, javaVM, activity);

    env->ReleaseStringUTFChars(licenseKey, key);

    if (success) {
        VuEngine* engine = wrapper.getEngine();
        g_imageTargetTracker = new ImageTargetTracker(engine);
        g_modelTargetTracker = new ModelTargetTracker(engine);
        LOGI("Native initialization successful");
    }

    return success ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_quforia_QuestVuforiaManager_nativeShutdown(JNIEnv* env, jobject thiz) {
    if (g_imageTargetTracker) {
        delete g_imageTargetTracker;
        g_imageTargetTracker = nullptr;
    }

    if (g_modelTargetTracker) {
        delete g_modelTargetTracker;
        g_modelTargetTracker = nullptr;
    }

    VuforiaEngineWrapper::getInstance().shutdown();
    LOGI("Native shutdown complete");
}

JNIEXPORT jboolean JNICALL
Java_com_quforia_QuestVuforiaManager_nativeProcessFrame(
    JNIEnv* env, jobject thiz, jbyteArray imageData, jint width, jint height,
    jfloatArray intrinsicsArray, jlong timestamp) {

    if (!imageData || !intrinsicsArray) {
        return JNI_FALSE;
    }

    jbyte* imageBytes = env->GetByteArrayElements(imageData, nullptr);
    jfloat* intrinsics = env->GetFloatArrayElements(intrinsicsArray, nullptr);

    if (!imageBytes || !intrinsics) {
        if (imageBytes) env->ReleaseByteArrayElements(imageData, imageBytes, JNI_ABORT);
        if (intrinsics) env->ReleaseFloatArrayElements(intrinsicsArray, intrinsics, JNI_ABORT);
        return JNI_FALSE;
    }

    VuCameraIntrinsics vuIntrinsics;
    vuIntrinsics.size.data[0] = intrinsics[0];
    vuIntrinsics.size.data[1] = intrinsics[1];
    vuIntrinsics.focalLength.data[0] = intrinsics[2];
    vuIntrinsics.focalLength.data[1] = intrinsics[3];
    vuIntrinsics.principalPoint.data[0] = intrinsics[4];
    vuIntrinsics.principalPoint.data[1] = intrinsics[5];
    vuIntrinsics.distortionMode = VU_CAMERA_DISTORTION_MODE_LINEAR;

    for (int i = 0; i < 8; i++) {
        vuIntrinsics.distortionParameters.data[i] = (i < 6) ? intrinsics[6 + i] : 0.0f;
    }

    bool result = VuforiaEngineWrapper::getInstance().processFrame(
        reinterpret_cast<const uint8_t*>(imageBytes),
        width, height, 0, vuIntrinsics, timestamp
    );

    env->ReleaseByteArrayElements(imageData, imageBytes, JNI_ABORT);
    env->ReleaseFloatArrayElements(intrinsicsArray, intrinsics, JNI_ABORT);

    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_quforia_QuestVuforiaManager_nativeLoadImageTargetDatabase(
    JNIEnv* env, jobject thiz, jstring databasePath) {

    if (!g_imageTargetTracker) {
        LOGE("Image target tracker not initialized");
        return JNI_FALSE;
    }

    const char* path = env->GetStringUTFChars(databasePath, nullptr);
    bool result = g_imageTargetTracker->loadDatabase(path);
    env->ReleaseStringUTFChars(databasePath, path);

    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_quforia_QuestVuforiaManager_nativeCreateImageTarget(
    JNIEnv* env, jobject thiz, jstring targetName) {

    if (!g_imageTargetTracker) {
        LOGE("Image target tracker not initialized");
        return JNI_FALSE;
    }

    const char* name = env->GetStringUTFChars(targetName, nullptr);
    bool result = g_imageTargetTracker->createTargetObserver(name);
    env->ReleaseStringUTFChars(targetName, name);

    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_quforia_QuestVuforiaManager_nativeDestroyImageTarget(
    JNIEnv* env, jobject thiz, jstring targetName) {

    if (!g_imageTargetTracker) return;

    const char* name = env->GetStringUTFChars(targetName, nullptr);
    g_imageTargetTracker->destroyTargetObserver(name);
    env->ReleaseStringUTFChars(targetName, name);
}

JNIEXPORT jobjectArray JNICALL
Java_com_quforia_QuestVuforiaManager_nativeGetImageTargetResults(
    JNIEnv* env, jobject thiz) {

    if (!g_imageTargetTracker) {
        return env->NewObjectArray(0, env->FindClass("com/quforia/TrackingResult"), nullptr);
    }

    ImageTargetResult results[10];
    int count = g_imageTargetTracker->getTrackedTargets(results, 10);

    jclass resultClass = env->FindClass("com/quforia/TrackingResult");
    jmethodID constructor = env->GetMethodID(resultClass, "<init>", "(Ljava/lang/String;[FI)V");

    jobjectArray resultArray = env->NewObjectArray(count, resultClass, nullptr);

    for (int i = 0; i < count; i++) {
        jstring name = env->NewStringUTF(results[i].name);
        jfloatArray matrix = env->NewFloatArray(16);
        env->SetFloatArrayRegion(matrix, 0, 16, results[i].poseMatrix);

        jobject result = env->NewObject(resultClass, constructor, name, matrix, results[i].status);
        env->SetObjectArrayElement(resultArray, i, result);

        env->DeleteLocalRef(name);
        env->DeleteLocalRef(matrix);
        env->DeleteLocalRef(result);
    }

    return resultArray;
}

JNIEXPORT jboolean JNICALL
Java_com_quforia_QuestVuforiaManager_nativeLoadModelTargetDatabase(
    JNIEnv* env, jobject thiz, jstring databasePath) {

    if (!g_modelTargetTracker) {
        LOGE("Model target tracker not initialized");
        return JNI_FALSE;
    }

    const char* path = env->GetStringUTFChars(databasePath, nullptr);
    bool result = g_modelTargetTracker->loadDatabase(path);
    env->ReleaseStringUTFChars(databasePath, path);

    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_quforia_QuestVuforiaManager_nativeCreateModelTarget(
    JNIEnv* env, jobject thiz, jstring targetName, jstring guideViewName) {

    if (!g_modelTargetTracker) {
        LOGE("Model target tracker not initialized");
        return JNI_FALSE;
    }

    const char* name = env->GetStringUTFChars(targetName, nullptr);
    const char* guideName = guideViewName ? env->GetStringUTFChars(guideViewName, nullptr) : nullptr;

    bool result = g_modelTargetTracker->createTargetObserver(name, guideName);

    env->ReleaseStringUTFChars(targetName, name);
    if (guideName) env->ReleaseStringUTFChars(guideViewName, guideName);

    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_quforia_QuestVuforiaManager_nativeDestroyModelTarget(
    JNIEnv* env, jobject thiz, jstring targetName) {

    if (!g_modelTargetTracker) return;

    const char* name = env->GetStringUTFChars(targetName, nullptr);
    g_modelTargetTracker->destroyTargetObserver(name);
    env->ReleaseStringUTFChars(targetName, name);
}

JNIEXPORT jobjectArray JNICALL
Java_com_quforia_QuestVuforiaManager_nativeGetModelTargetResults(
    JNIEnv* env, jobject thiz) {

    if (!g_modelTargetTracker) {
        return env->NewObjectArray(0, env->FindClass("com/quforia/TrackingResult"), nullptr);
    }

    ModelTargetResult results[10];
    int count = g_modelTargetTracker->getTrackedTargets(results, 10);

    jclass resultClass = env->FindClass("com/quforia/TrackingResult");
    jmethodID constructor = env->GetMethodID(resultClass, "<init>", "(Ljava/lang/String;[FI)V");

    jobjectArray resultArray = env->NewObjectArray(count, resultClass, nullptr);

    for (int i = 0; i < count; i++) {
        jstring name = env->NewStringUTF(results[i].name);
        jfloatArray matrix = env->NewFloatArray(16);
        env->SetFloatArrayRegion(matrix, 0, 16, results[i].poseMatrix);

        jobject result = env->NewObject(resultClass, constructor, name, matrix, results[i].status);
        env->SetObjectArrayElement(resultArray, i, result);

        env->DeleteLocalRef(name);
        env->DeleteLocalRef(matrix);
        env->DeleteLocalRef(result);
    }

    return resultArray;
}

}
