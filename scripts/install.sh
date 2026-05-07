#!/bin/bash
# Kate Agent Plugin Installer

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
PLUGIN_DIR="${PROJECT_DIR}/build"

# Kate KTextEditor plugin directory (where Kate actually looks)
KATE_PLUGIN_DIR="/usr/lib/x86_64-linux-gnu/qt6/plugins/kf6/ktexteditor"

# Check if plugin exists, build if not
if [ ! -f "$PLUGIN_DIR/libkateagentplugin.so" ]; then
    echo "Building plugin first..."
    cd "$PROJECT_DIR"
    rm -rf build
    cmake -B build . && cmake --build build -j"$(nproc)"
fi

echo "Installing Kate Agent Plugin..."

# Check if we can write to system plugin dir
if [ -w "$KATE_PLUGIN_DIR" ] || [ "$EUID" -eq 0 ]; then
    # Direct copy or symlink to system dir
    ln -sf "$PLUGIN_DIR/libkateagentplugin.so" "$KATE_PLUGIN_DIR/libkateagentplugin.so"
    echo "✅ Installed symlink: $KATE_PLUGIN_DIR/libkateagentplugin.so -> $PLUGIN_DIR/libkateagentplugin.so"
else
    echo "⚠️  Need root to install to $KATE_PLUGIN_DIR"
    echo "   Running with sudo..."
    sudo ln -sf "$PLUGIN_DIR/libkateagentplugin.so" "$KATE_PLUGIN_DIR/libkateagentplugin.so"
    echo "✅ Installed symlink: $KATE_PLUGIN_DIR/libkateagentplugin.so -> $PLUGIN_DIR/libkateagentplugin.so"
fi

# Also install the JSON metadata file
if [ -f "$PROJECT_DIR/src/kateagentplugin.json" ]; then
    if [ -w "$KATE_PLUGIN_DIR" ] || [ "$EUID" -eq 0 ]; then
        cp "$PROJECT_DIR/src/kateagentplugin.json" "$KATE_PLUGIN_DIR/"
    else
        sudo cp "$PROJECT_DIR/src/kateagentplugin.json" "$KATE_PLUGIN_DIR/"
    fi
fi

echo ""
echo "To use the plugin:"
echo "1. Open Kate"
echo "2. Go to Settings → Configure Kate"
echo "3. Select 'Plugins' tab"
echo "4. Enable 'Kate Agent'"
echo ""
echo "Note: You may need to restart Kate for the plugin to appear."
