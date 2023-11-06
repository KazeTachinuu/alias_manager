#!/bin/bash


# Color codes for output messages
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

# Function to display error messages
display_error() {
  echo -e "${RED}[Error]: $1${NC}"
  exit 1
}

# Function to display success messages
display_success() {
  echo -e "${GREEN}[Success]: $1${NC}"
}


# Build the tool
gcc aliasmanager.c -o aliasmanager || display_error "Failed to build the tool."

mkdir -p ~/.local/bin || display_error "Failed to created ~/.local/bin directory"

# Copy the binary to a directory in the PATH
cp aliasmanager ~/.local/bin/ || display_error "Failed to copy the binary to ~/.local/bin"


# Determine the user's shell
user_shell=$(basename "$SHELL")"rc"
# Check if the source command already exists in the appropriate shell rc file
if grep -q ".my_aliases.txt" "$HOME/.$user_shell"; then
  display_success ".my_aliases.txt already sourced in .$user_shell"
else
  # Append a source command to the appropriate shell rc file for .my_aliases.txt
  touch $HOME/.my_aliases.txt || display_error "Failed to create .my_aliases.txt"

  echo "" >> "$HOME/.$user_shell"
          echo "# Alias Management Tool" >> "$HOME/.$user_shell"
  echo "source \$HOME/.my_aliases.txt" >> "$HOME/.$user_shell"
  display_success "Added source command to $HOME/.$user_shell"
  echo "PATH=\$PATH:\$HOME/.local/bin" >> "$HOME/.$user_shell"
fi


echo "alias am='aliasmanager'" >> $HOME/.my_aliases.txt || display_error "Failed to add aliasmanager alias to ~/.my_aliases.txt"
# Print installation success message
display_success "Installation complete!\n"

echo "alias am=aliasmanager added to ~/.my_aliases.txt"

