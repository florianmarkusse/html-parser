#!/bin/bash
set -e

RED='\033[31m'
YELLOW='\033[33m'
BOLD='\033[1m'
NO_COLOR='\033[0m'

echo -e "${YELLOW}${BOLD}Installing clang. ${NO_COLOR}"
sudo apt install clang

echo -e "${YELLOW}${BOLD}Installing cmake. ${NO_COLOR}"
sudo apt install cmake
