#!/bin/bash
set -e

echo "=== kate-agents: Build, Test & Install ==="

# Fix build dir ownership if corrupted by previous sudo runs
if [ -d build ] && [ "$(stat -c '%U' build)" != "$USER" ]; then
    echo "Fixing build/ ownership (requires sudo)..."
    sudo chown -R "$USER:$USER" build/
fi

# Configure build directory if missing
if [ ! -f build/cmake_install.cmake ]; then
    echo "[1/3] Configuring CMake..."
    cmake -B build -S .
else
    echo "[1/3] Build directory already configured."
fi

# Build plugin + all tests
echo "[2/3] Building plugin and tests..."
cmake --build build -j"$(nproc)" 2>&1 | tail -20

# Run tests
echo ""
echo "[3/3] Running tests..."
cd build
if ctest --output-on-failure 2>&1 | tail -40; then
    echo "Tests passed."
else
    echo "Some tests failed — see output above."
fi
cd ..

# Install to system directory
echo ""
echo "=== Installing plugin ==="
sudo cmake --install build

echo ""
echo "=== Done! ==="
echo "Plugin: /usr/lib/x86_64-linux-gnu/qt6/plugins/kf6/ktexteditor/kateagentplugin.so"
echo ""
echo "To use: restart Kate, enable plugin in Settings -> Plugins"
echo "Clear cache if needed: rm -rf ~/.cache/Kate* ~/.local/share/Kate* ~/.config/kate/*"
