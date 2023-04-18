#!/bin/bash

# Function to display error messages
display_error() {
  echo "Error: $1"
  exit 1
}

# Function to display success messages
display_success() {
  echo "Success: $1"
}

# Build the tool
cargo build --release || display_error "Failed to build the tool."

# Copy the binary to a directory in the PATH
cp target/release/addalias /usr/local/bin/ || display_error "Failed to copy the binary to /usr/local/bin."

# Append a source command to .zshrc for .my_aliases.zsh
echo "" >> ~/.zshrc
echo "# Alias Management Tool" >> ~/.zshrc
echo "source ~/.my_aliases.zsh" >> ~/.zshrc

# Reload .zshrc in the current shell
source ~/.zshrc

# Print installation success message
display_success "Installation complete!"
display_success "You can now use 'addalias' to manage your aliases."
