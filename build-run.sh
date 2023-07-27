#!/bin/bash
set -e

YELLOW='\033[33m'
RED='\033[31m'
BOLD='\033[1m'
NO_COLOR='\033[0m'

# Default values
BUILD_TYPE="release"
PROJECT_ONLY=false
# Initialize the positional arguments
EXECUTABLE=""

# Function to display usage
function display_usage() {
	echo -e "${RED}${BOLD}Usage: $0 <${YELLOW}EXECUTABLE${RED}> [${YELLOW}OPTIONS${RED}]${NO_COLOR}"
	echo -e "${BOLD}Options:${NO_COLOR}"
	echo -e "  -t, --build-type <TYPE>    Set the build type (${YELLOW}debug${NO_COLOR} or ${YELLOW}release${NO_COLOR}). Default is ${YELLOW}release${NO_COLOR}."
	echo -e "  -p, --project-only         Build only the main project without tests."
	echo -e "  -h, --help                 Display this help message."
	exit 1
}

# Parse command-line options
while [[ "$#" -gt 0 ]]; do
	case $1 in
	-t | --build-type)
		if [[ "$2" != "debug" && "$2" != "release" ]]; then
			echo -e "${RED}${BOLD}Invalid ${YELLOW}BUILD_TYPE${RED}. Valid options: ${NO_COLOR}${YELLOW}debug${BOLD}${RED}, ${NO_COLOR}${YELLOW}release${BOLD}${RED}.${NO_COLOR}"
			exit 1
		fi
		BUILD_TYPE="$2"
		shift 2
		;;
	-p | --project-only)
		PROJECT_ONLY=true
		shift
		;;
	-h | --help)
		display_usage
		;;
	*)
		EXECUTABLE="$1"
		shift
		;;
	esac
done

# Check if EXECUTABLE is provided
if [[ -z "$EXECUTABLE" ]]; then
	echo -e "${RED}${BOLD}Error: ${YELLOW}EXECUTABLE${RED} not specified.${NO_COLOR}"
	display_usage
fi

# Check if the provided BUILD_TYPE is valid (this check is already done in the loop, but we repeat it here for clarity)
if [[ "$BUILD_TYPE" != "debug" && "$BUILD_TYPE" != "release" ]]; then
	echo -e "${RED}${BOLD}Invalid ${YELLOW}BUILD_TYPE${RED}. Valid options: ${NO_COLOR}${YELLOW}debug${BOLD}${RED}, ${NO_COLOR}${YELLOW}release${BOLD}${RED}.${NO_COLOR}"
	exit 1
fi

# Display the configuration
echo -e "${BOLD}${YELLOW}Configuration...${NO_COLOR}"
echo -e "${BOLD}${YELLOW}EXECUTABLE${NO_COLOR}: ${YELLOW}$EXECUTABLE${NO_COLOR}"
echo -e "${BOLD}${YELLOW}BUILD_TYPE${NO_COLOR}: ${YELLOW}$BUILD_TYPE${NO_COLOR}"
if [ "$PROJECT_ONLY" = true ]; then
	echo -e "Build only the main project ${YELLOW}without tests${NO_COLOR}."
else
	echo -e "Build the main project and ${YELLOW}run tests${NO_COLOR}."
fi
echo ""

# Perform the build based on the options
cmake -S . -B build/ -D CMAKE_BUILD_TYPE="$BUILD_TYPE"
cmake --build build/

if [ "$PROJECT_ONLY" = false ]; then
	./run-executable.sh build "$EXECUTABLE"-tests-"$BUILD_TYPE"
fi
