#!/usr/bin/env bash
set -e

# Colors for output
GREEN="\033[0;32m"
BLUE="\033[0;34m"
RESET="\033[0m"

echo -e "${BLUE}Building alias manager...${RESET}"
make clean
make

echo -e "\n${GREEN}Build successful!${RESET}"
echo -e "\nTo install alias manager:"
echo -e "\n1. Install the program (requires sudo):"
echo "sudo make install"

echo -e "\n2. Add this line to your shell's rc file (~/.bashrc, ~/.zshrc, etc.):"
echo '[[ -f ~/.my_aliases.txt ]] && source ~/.my_aliases.txt      # Load aliases if exists'

echo -e "\n3. Reload your shell configuration:"
echo "source ~/.bashrc  # (or your shell's rc file)"


