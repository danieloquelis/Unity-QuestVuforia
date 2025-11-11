#!/bin/bash

# Quforia Plugin Build Script
# Builds libquforia.so and copies it to Unity's Plugins folder

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=====================================${NC}"
echo -e "${GREEN}Building Quforia Plugin${NC}"
echo -e "${GREEN}=====================================${NC}"

# Paths
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"
UNITY_PLUGINS_DIR="$SCRIPT_DIR/../Assets/Plugins/Android/libs/arm64-v8a"
NDK_PATH="/Applications/Unity/Hub/Editor/6000.0.61f1/PlaybackEngines/AndroidPlayer/NDK"

# Check NDK exists
if [ ! -d "$NDK_PATH" ]; then
    echo -e "${RED}ERROR: NDK not found at $NDK_PATH${NC}"
    exit 1
fi

echo -e "${YELLOW}Using NDK: $NDK_PATH${NC}"

# Clean previous build
if [ -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Cleaning previous build...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
echo -e "${YELLOW}Configuring CMake for arm64-v8a...${NC}"
cmake \
  -DCMAKE_TOOLCHAIN_FILE="$NDK_PATH/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-29 \
  -DCMAKE_BUILD_TYPE=Release \
  -DANDROID_STL=c++_static \
  ..

# Build
echo -e "${YELLOW}Building libquforia.so...${NC}"
cmake --build . --config Release -j8

# Check if library was built
if [ ! -f "$BUILD_DIR/libquforia.so" ]; then
    echo -e "${RED}ERROR: libquforia.so not found after build${NC}"
    exit 1
fi

# Get library size
LIB_SIZE=$(du -h "$BUILD_DIR/libquforia.so" | awk '{print $1}')
echo -e "${GREEN}✓ libquforia.so built successfully ($LIB_SIZE)${NC}"

# Create Unity plugins directory if it doesn't exist
mkdir -p "$UNITY_PLUGINS_DIR"

# Copy to Unity
echo -e "${YELLOW}Copying to Unity Plugins...${NC}"
cp "$BUILD_DIR/libquforia.so" "$UNITY_PLUGINS_DIR/libquforia.so"

echo -e "${GREEN}✓ Copied to: $UNITY_PLUGINS_DIR/libquforia.so${NC}"

# Verify
if [ -f "$UNITY_PLUGINS_DIR/libquforia.so" ]; then
    echo -e "${GREEN}=====================================${NC}"
    echo -e "${GREEN}Build Complete!${NC}"
    echo -e "${GREEN}=====================================${NC}"
    echo ""
    echo -e "${YELLOW}Next steps:${NC}"
    echo "1. Refresh Unity (Assets → Refresh)"
    echo "2. Build and deploy your APK"
    echo ""
else
    echo -e "${RED}ERROR: Failed to copy library${NC}"
    exit 1
fi
