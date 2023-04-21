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

# Check if script is run with root privileges
if [ "$EUID" -ne 0 ]; then
  display_error "This script requires root privileges. Please run with sudo."
fi

# Build the tool
cargo build --release || display_error "Failed to build the tool."

# Copy the binary to a directory in the PATH
cp target/release/aliasmanager /usr/local/bin/ || display_error "Failed to copy the binary to /usr/local/bin."

# Determine the user's shell
user_shell=$(basename "$SHELL")
echo "Detected user shell: $user_shell"
# Check if the source command already exists in the appropriate shell rc file
if grep -q ".my_aliases.txt" "/home/$SUDO_USER/.$user_shell"rc; then
  display_success ".my_aliases.txt already sourced in .$user_shell"rc
else
  # Append a source command to the appropriate shell rc file for .my_aliases.txt
  touch /home/$SUDO_USER/.my_aliases.txt || display_error "Failed to create .my_aliases.txt"
  echo "" >> "/home/$SUDO_USER/.$user_shell"rc
  echo "# Alias Management Tool" >> "/home/$SUDO_USER/.$user_shell"rc
  echo "source /home/$SUDO_USER/.my_aliases.txt" >> "/home/$SUDO_USER/.$user_shell"rc
  display_success "Added source command to /home/$SUDO_USER/.$user_shell"rc
fi

# Print installation success message
display_success "Installation complete!"
display_success "You can now use 'aliasmanager' to manage your aliases."

