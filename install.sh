#!/bin/bash

#

set -e

# ANSI color code for red text
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${RED}Building GreetdExtension...${NC}"
echo -e "${RED}Building debug target...${NC}"
scons use_llvm=true compiledb=yes target=template_debug

echo -e "${RED}Building release target...${NC}"
scons use_llvm=true compiledb=yes target=template_release

echo -e "${RED}Exporting Godot demo project...${NC}"
cd demo
godot --headless --export-release "test_greeter" testlogin.x86_64
cd ..

echo -e "${RED}Installing libraries to /usr/local/bin/...${NC}"
sudo cp bin/linux/libgreetd.linux.template_* /usr/local/bin/

echo -e "${RED}Installing demo binary to /usr/local/bin/...${NC}"
sudo cp demo/testlogin.x86_64 /usr/local/bin/

echo -e "${RED}Installation complete!${NC}"
