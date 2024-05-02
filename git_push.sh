#!/bin/bash

PURPLE='\033[0;35m'
ORANGE='\033[0;33m'
GREEN='\033[0;32m'
RESET='\033[0m'

# Check if the argument is provided
if [ -z "$1" ]; then
    echo "Error: No project name provided."
    echo "Usage: $0 <project-name>"
    exit 1
fi

NAME=$1

echo -e "${ORANGE} \n/=======================================================================\\ ${RESET}"
git status
echo -e "${ORANGE}\\=======================================================================/\n ${RESET}n"
git add .
git commit -m "auto commit: $NAME"
git push
echo -e "${GREEN} \n/=======================================================================\\ ${RESET}"
git status
echo -e "${GREEN}\\=======================================================================/\n ${RESET}"
git log -n1 --pretty=short | cat