#!/bin/bash
# Build script for kate-agents plugin
# Run this as normal user - no sudo required for build

set -e

echo "=== Building kate-agents plugin ==="

# Fix ownership if needed (only needed once after root operations)
if [ ! -w build ]; then
    echo "Build directory owned by root. Running chown..."
    sudo chown -R $USER:$USER build/
fi

# Create build directory if it doesn't exist
if [ ! -d build ]; then
    mkdir build
fi

# Configure with CMake
echo "Configuring with CMake..."
cmake -B build -S .

# Build the plugin
echo "Building plugin..."
cmake --build build -j$(nproc)

echo ""
echo "=== Build complete! ==="
echo ""
echo "To install (requires sudo):"
echo "  sudo cmake --install build"
echo ""
echo "To uninstall:"
echo "  sudo cmake --build build --target uninstall"
