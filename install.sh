#!/bin/bash

# Function to display error messages
display_error() {
  echo "[Error]: $1"
  exit 1
}

# Function to display success messages
display_success() {
  echo "[Success]: $1"
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
if grep -q ".my_aliases.zsh" "/home/$SUDO_USER/.$user_shell"rc; then
  display_success ".my_aliases.zsh already sourced in .$user_shell"rc
else
  # Append a source command to the appropriate shell rc file for .my_aliases.zsh
  echo "" >> "/home/$SUDO_USER/.$user_shell"rc
  echo "# Alias Management Tool" >> "/home/$SUDO_USER/.$user_shell"rc
  echo "source /home/$SUDO_USER/.my_aliases.zsh" >> "/home/$SUDO_USER/.$user_shell"rc
fi


# Print installation success message
display_success "Installation complete!"
display_success "You can now use 'aliasmanager' to manage your aliases."
