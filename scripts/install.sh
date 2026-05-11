#!/bin/bash
# Kate Agent Plugin Installer

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
PLUGIN_DIR="${PROJECT_DIR}/build"

# Kate KTextEditor plugin directory (where Kate actually looks)
KATE_PLUGIN_DIR="/usr/lib/x86_64-linux-gnu/qt6/plugins/kf6/ktexteditor"

# Check if plugin exists, build if not
if [ ! -f "$PLUGIN_DIR/kateagentplugin.so" ]; then
    echo "Building plugin first..."
    cd "$PROJECT_DIR"
    rm -rf build
    cmake -B build . && cmake --build build -j"$(nproc)"
fi

echo "Installing Kate Agent Plugin..."

if [ -w "$KATE_PLUGIN_DIR" ] || [ "$EUID" -eq 0 ]; then
    ln -sf "$PLUGIN_DIR/kateagentplugin.so" "$KATE_PLUGIN_DIR/kateagentplugin.so"
    cp "$PROJECT_DIR/src/kateagentplugin.json" "$KATE_PLUGIN_DIR/"
    echo "✅ Installed to $KATE_PLUGIN_DIR/"
else
    echo "⚠️  Need root to install to $KATE_PLUGIN_DIR"
    sudo ln -sf "$PLUGIN_DIR/kateagentplugin.so" "$KATE_PLUGIN_DIR/kateagentplugin.so"
    sudo cp "$PROJECT_DIR/src/kateagentplugin.json" "$KATE_PLUGIN_DIR/"
    echo "✅ Installed to $KATE_PLUGIN_DIR/"
fi

echo ""
echo "To use the plugin:"
echo "1. Open Kate"
echo "2. Go to Settings → Configure Kate"
echo "3. Select 'Plugins' tab"
echo "4. Enable 'Kate Agent'"
echo ""
echo "Note: You may need to restart Kate for the plugin to appear."
