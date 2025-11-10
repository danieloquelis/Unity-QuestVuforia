cmake_minimum_required(VERSION 3.22.1)
project(quforia)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(VUFORIA_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../Vuforia_SDK_Android_11.4.4/build")

include_directories(
    ${VUFORIA_SDK_DIR}/include
)

add_library(VuforiaEngine SHARED IMPORTED)
set_target_properties(VuforiaEngine PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libVuforiaEngine.so
)

add_library(quforia SHARED
    quforia_jni.cpp
    vuforia_engine_wrapper.cpp
    image_target_tracker.cpp
    model_target_tracker.cpp
)

target_link_libraries(quforia
    android
    log
    VuforiaEngine
)
