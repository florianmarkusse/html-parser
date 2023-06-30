#!/bin/bash
set -e

YELLOW='\033[33m'
RED='\033[31m'
BOLD='\033[1m'
NO_COLOR='\033[0m'

if [ $# -lt 2 ]; then
	echo -e "${RED}${BOLD}Please provide the folder path as the first argument and the starting name as the second argument.${NO_COLOR}"
	exit 1
fi

folder_path=$1
starting_name=$2

executable_paths=$(find "$folder_path" -name "${starting_name}*" -type f -executable)

if [ -z "${executable_paths[*]}" ]; then
	echo -e "${RED}${BOLD}Executable not found.${NO_COLOR}"
	echo -e "Looked for ${YELLOW}${starting_name}${NO_COLOR} in ${folder_path}"
	exit 1
fi

num_executables=$(echo "$executable_paths" | wc -l)
if [ "$num_executables" -gt 1 ]; then
	echo -e "${RED}${BOLD}Multiple executables found. Please specify a more specific starting name.${NO_COLOR}"
	echo -e "${executable_paths}"
	echo -e "Looked for ${YELLOW}${starting_name}${NO_COLOR} in ${folder_path}"
	exit 1
fi

echo -e "Executing ${YELLOW}${executable_paths}${NO_COLOR}..."
"$executable_paths"
