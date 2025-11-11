cmake_minimum_required(VERSION 3.22.1)
project(quforia)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Building libquforia.so for Vuforia Driver Framework")
message(STATUS "Target ABI: ${ANDROID_ABI}")

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
add_library(quforia SHARED
    src/quforia_jni.cpp
    src/vuforia_driver.cpp
    src/external_camera.cpp
    src/external_tracker.cpp
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

# Ensure all symbols are exported (required for Vuforia Driver Framework)
set_target_properties(quforia PROPERTIES
    CXX_VISIBILITY_PRESET default
    LINK_FLAGS "-Wl,--export-dynamic"
)

message(STATUS "Configured quforia native library")
