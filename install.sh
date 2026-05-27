#!/bin/bash
# Install script for kate-agents plugin
# This script requires sudo privileges

set -e

echo "=== Installing kate-agents plugin ==="

# Install to system directory
sudo cmake --install build

echo ""
echo "=== Installation complete! ==="
echo ""
echo "Plugin installed to: /usr/lib/x86_64-linux-gnu/qt6/plugins/kf6/ktexteditor/"
echo ""
echo "To use:"
echo "  1. Restart Kate completely"
echo "  2. Enable plugin in Kate Settings -> Configure Kate -> Plugins"
echo ""
echo "Optional: Clear Kate cache if plugin doesn't appear:"
echo "  rm -rf ~/.cache/Kate* ~/.local/share/Kate* ~/.config/kate/*"
