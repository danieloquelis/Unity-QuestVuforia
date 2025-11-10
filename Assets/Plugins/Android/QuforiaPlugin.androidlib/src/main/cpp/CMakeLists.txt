cmake_minimum_required(VERSION 3.22.1)
project(quforia)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Building libquforia.so for Vuforia Driver Framework")

# Include directories - using local headers only
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Source files for Driver Framework
add_library(quforia SHARED
    quforia_jni.cpp
    vuforia_driver.cpp
    external_camera.cpp
    external_tracker.cpp
)

# Link libraries
target_link_libraries(quforia
    android
    log
)

# Compiler flags
target_compile_options(quforia PRIVATE
    -Wall
    -Wextra
    -Werror=return-type
)

# Ensure Driver Framework entry points are visible
set_target_properties(quforia PROPERTIES
    CXX_VISIBILITY_PRESET default
)

message(STATUS "Configured quforia native library for ${ANDROID_ABI}")
