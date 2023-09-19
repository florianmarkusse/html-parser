#!/bin/bash
set -e

YELLOW='\033[33m'
RED='\033[31m'
BOLD='\033[1m'
NO_COLOR='\033[0m'

BUILD_MODES=("Debug" "Release" "Profiling")

BUILD_MODE="Release"
BUILD_SHARED_LIBS=false
RUN_TESTS=false
RUN_BENCHMARKS=false

# Function to display usage
function display_usage() {
	echo -e "${RED}${BOLD}Usage: $0 [${YELLOW}OPTIONS${RED}]${NO_COLOR}"
	echo -e "${BOLD}Options:${NO_COLOR}"
	echo -e "  -m, --build-mode <TYPE>    Set the build mode (${YELLOW}${BUILD_MODES[*]}${NO_COLOR}). Default is ${YELLOW}Release${NO_COLOR}."
	echo -e "  -s, --shared-libs          Build shared libraries."
	echo -e "  -t, --run-tests            Build tests after building."
	echo -e "  -b, --run-benchmarks       Build benchmarks after building."
	echo -e "  -h, --help                 Display this help message."
	exit 1
}

# Function to check if a build mode is valid
function is_valid_build_mode() {
	local mode="$1"
	for valid_mode in "${BUILD_MODES[@]}"; do
		if [[ "$mode" == "$valid_mode" ]]; then
			return 0
		fi
	done
	return 1
}

# Function to display configuration
function display_configuration() {
	echo -e "${BOLD}${YELLOW}Configuration...${NO_COLOR}"
	echo -e "${BOLD}${YELLOW}BUILD_MODE${NO_COLOR}: ${YELLOW}${BUILD_MODE}${NO_COLOR}"
	echo -e "${BOLD}${YELLOW}SHARED LIBS${NO_COLOR}: ${YELLOW}${BUILD_SHARED_LIBS}${NO_COLOR}"
	if [ "$RUN_TESTS" = true ]; then
		echo -e "${BOLD}${YELLOW}Run tests${NO_COLOR}: ${YELLOW}Yes${NO_COLOR}"
	else
		echo -e "${BOLD}${YELLOW}Run tests${NO_COLOR}: ${YELLOW}No${NO_COLOR}"
	fi
	if [ "$RUN_BENCHMARKS" = true ]; then
		echo -e "${BOLD}${YELLOW}Run benchmarks${NO_COLOR}: ${YELLOW}Yes${NO_COLOR}"
	else
		echo -e "${BOLD}${YELLOW}Run benchmarks${NO_COLOR}: ${YELLOW}No${NO_COLOR}"
	fi
	echo ""
}

# Parse command-line options
while [[ "$#" -gt 0 ]]; do
	case $1 in
	-m | --build-mode)
		if ! is_valid_build_mode "$2"; then
			echo -e "${RED}${BOLD}Invalid ${YELLOW}BUILD_MODE${RED}. Valid options: ${NO_COLOR}${YELLOW}${BUILD_MODES[*]}${NO_COLOR}."
			exit 1
		fi
		BUILD_MODE="$2"
		shift 2
		;;
	-t | --run-tests)
		RUN_TESTS=true
		shift
		;;
	-b | --run-benchmarks)
		RUN_BENCHMARKS=true
		shift
		;;
	-h | --help)
		display_usage
		;;
	-s | --shared-libs)
		BUILD_SHARED_LIBS="ON"
		shift
		;;
	*)
		display_usage
		;;
	esac
done

# Display the configuration
display_configuration

# Perform the build based on the options
cmake -S . -B build/ -D CMAKE_BUILD_TYPE="$BUILD_MODE" -D BUILD_SHARED_LIBS="$BUILD_SHARED_LIBS" -D BUILD_TESTS="$RUN_TESTS" -D BUILD_BENCHMARKS="$RUN_BENCHMARKS"
cmake --build build/
