#!/bin/bash

# Builds the GreetdExtension GDExtension library, exports the Godot demo
# project, and installs both to /usr/local/bin/.
# Automatically downloads Godot export templates if they are missing.
#
# Usage: ./install.sh [debug|release]
#   debug    Build and install debug target only
#   release  Build and install release target only
#   (none)   Build and install both targets

set -e

# ANSI color codes
CYAN='\033[0;36m'
NC='\033[0m' # No Color

BUILD_TARGET="${1:-all}"

if [ "$BUILD_TARGET" != "all" ] && [ "$BUILD_TARGET" != "debug" ] && [ "$BUILD_TARGET" != "release" ]; then
    echo "Usage: $0 [debug|release]"
    echo "  debug    Build and install debug target only"
    echo "  release  Build and install release target only"
    echo "  (none)   Build and install both targets"
    exit 1
fi

echo -e "${CYAN}Building GreetdExtension...${NC}"

if [ "$BUILD_TARGET" = "all" ] || [ "$BUILD_TARGET" = "debug" ]; then
    echo -e "${CYAN}Building debug target...${NC}"
    scons use_llvm=true compiledb=yes target=template_debug
fi

if [ "$BUILD_TARGET" = "all" ] || [ "$BUILD_TARGET" = "release" ]; then
    echo -e "${CYAN}Building release target...${NC}"
    scons use_llvm=true compiledb=yes target=template_release
fi

echo -e "${CYAN}Checking for Godot export templates...${NC}"
GODOT_VERSION=$(godot --version | head -n1 | grep -oE '^[0-9]+\.[0-9]+(\.[0-9]+)?\.[a-z]+')
TEMPLATE_DIR="$HOME/.local/share/godot/export_templates/$GODOT_VERSION"

if [ ! -f "$TEMPLATE_DIR/linux_debug.x86_64" ] || [ ! -f "$TEMPLATE_DIR/linux_release.x86_64" ]; then
    if ! command -v unzip &> /dev/null; then
        echo "Error: 'unzip' is required to extract export templates but was not found."
        exit 1
    fi
    # Convert version format from "4.6.3.stable" to "4.6.3-stable" for the download URL
    DOWNLOAD_VERSION=$(echo "$GODOT_VERSION" | sed 's/\.\([a-z]*\)$/-\1/')
    TEMPLATE_URL="https://github.com/godotengine/godot/releases/download/$DOWNLOAD_VERSION/Godot_v${DOWNLOAD_VERSION}_export_templates.tpz"

    echo -e "${CYAN}Export templates not found, downloading from:${NC}"
    echo "  $TEMPLATE_URL"

    TEMP_DIR=$(mktemp -d)
    trap "rm -rf $TEMP_DIR" EXIT

    curl -L -o "$TEMP_DIR/templates.tpz" "$TEMPLATE_URL"
    mkdir -p "$TEMPLATE_DIR"
    unzip -o "$TEMP_DIR/templates.tpz" -d "$TEMP_DIR/extracted"
    cp -r "$TEMP_DIR/extracted/templates/"* "$TEMPLATE_DIR/"

    echo -e "${CYAN}Export templates installed to $TEMPLATE_DIR${NC}"
else
    echo -e "${CYAN}Export templates found at $TEMPLATE_DIR${NC}"
fi

echo -e "${CYAN}Exporting Godot demo project...${NC}"
cd demo
if [ "$BUILD_TARGET" = "debug" ]; then
    godot --headless --export-debug "test_greeter" testlogin.x86_64
else
    godot --headless --export-release "test_greeter" testlogin.x86_64
fi
cd ..

echo -e "${CYAN}Installing libraries to /usr/local/bin/...${NC}"
if [ "$BUILD_TARGET" = "all" ]; then
    sudo cp bin/linux/libgreetd.linux.template_* /usr/local/bin/
else
    sudo cp bin/linux/libgreetd.linux.template_${BUILD_TARGET}* /usr/local/bin/
fi

echo -e "${CYAN}Installing demo binary to /usr/local/bin/...${NC}"
sudo cp demo/testlogin.x86_64 /usr/local/bin/

echo -e "${CYAN}Installation complete!${NC}"
