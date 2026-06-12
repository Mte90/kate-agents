#!/bin/bash
set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"

cd "${PROJECT_DIR}"

if [ ! -f "${BUILD_DIR}/kateagentplugin.so" ]; then
    echo "Building plugin first..."
    rm -rf "${BUILD_DIR}"
    cmake -B "${BUILD_DIR}" .
    cmake --build "${BUILD_DIR}" -j16
fi

echo "Installing Kate Agent Plugin..."
cd "${BUILD_DIR}"

if [ "$EUID" -eq 0 ]; then
    cmake --install . --prefix /usr
else
    sudo cmake --install . --prefix /usr
fi

echo ""
echo "Done! Installed to /usr/lib/x86_64-linux-gnu/qt6/plugins/kf6/ktexteditor/"
echo "Restart Kate and check Settings / Configure Kate / Agent"
