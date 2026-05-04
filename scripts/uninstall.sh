#!/bin/bash
# Kate Agent Plugin Uninstaller

set -e

KATE_PLUGIN_DIRS=(
    "$HOME/.local/lib/kate/plugins"
    "$HOME/.kde/lib/kate/plugins"
    "$HOME/.kde5/lib/kate/plugins"
    "/usr/lib/x86_64-linux-gnu/kate/plugins"
    "/usr/lib/kate/plugins"
    "/usr/local/lib/kate/plugins"
)

for dir in "${KATE_PLUGIN_DIRS[@]}"; do
    if [ -f "$dir/libkateagentplugin.so" ]; then
        rm "$dir/libkateagentplugin.so"
        echo "✅ Removed libkateagentplugin.so from $dir"
    fi
done

echo "✅ Uninstallation complete"
echo "Restart Kate to complete removal."
