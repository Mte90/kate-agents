#!/bin/bash
# Kate Agent Plugin Installer

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PLUGIN_DIR="${SCRIPT_DIR}/build"

# Find Kate plugins directory
KATE_PLUGIN_DIRS=(
    "$HOME/.local/lib/kate/plugins"
    "$HOME/.kde/lib/kate/plugins"
    "$HOME/.kde5/lib/kate/plugins"
    "/usr/lib/x86_64-linux-gnu/kate/plugins"
    "/usr/lib/kate/plugins"
    "/usr/local/lib/kate/plugins"
)

install_dir=""
for dir in "${KATE_PLUGIN_DIRS[@]}"; do
    if [ -d "$dir" ] || [ -w "$(dirname "$dir")" ]; then
        install_dir="$dir"
        break
    fi
done

if [ -z "$install_dir" ]; then
    install_dir="$HOME/.local/lib/kate/plugins"
    mkdir -p "$install_dir"
fi

echo "Installing Kate Agent Plugin to: $install_dir"

# Check if plugin exists
if [ ! -f "$PLUGIN_DIR/libkateagentplugin.so" ]; then
    echo "Building plugin first..."
    cd "$SCRIPT_DIR"
    rm -rf build
    cmake -B build . && cmake --build build
fi

# Copy plugin
cp "$PLUGIN_DIR/libkateagentplugin.so" "$install_dir/"
echo "✅ Installed libkateagentplugin.so to $install_dir"

echo ""
echo "To use the plugin:"
echo "1. Open Kate"
echo "2. Go to Settings → Configure Kate"
echo "3. Select 'Plugins' tab"
echo "4. Enable 'Kate Agent'"
echo ""
echo "Note: You may need to restart Kate for the plugin to appear."
