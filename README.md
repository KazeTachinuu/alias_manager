# Alias Manager (am)

> A lightweight CLI tool to manage your shell aliases efficiently



## Features

- Create and update aliases with a simple command
- Remove aliases safely (with confirmation)
- List all your aliases
- Search aliases by pattern
- Works with any POSIX-compliant shell (bash, zsh, fish)
- Smart color output (auto-detects TTY)
- Comprehensive man page
- Memory-safe implementation (no malloc)
- Zero dependencies (only libc)

## Installation

### Arch Linux

```bash
# Using an AUR helper (recommended)
yay -S alias-manager

# Or with paru
paru -S alias-manager

# Manual installation from AUR
git clone https://aur.archlinux.org/alias-manager.git
cd alias-manager
makepkg -si
```

### macOS / Linux (Homebrew)

```bash
# Install from tap
brew install kazetachinuu/alias-manager/alias-manager

# Or add tap first, then install
brew tap kazetachinuu/alias-manager
brew install alias-manager
```

### Debian / Ubuntu

```bash
# Download the latest .deb package
wget https://github.com/kazetachinuu/alias_manager/releases/download/v2.1.0/alias-manager_2.1.0_amd64.deb

# Install with dpkg
sudo dpkg -i alias-manager_2.1.0_amd64.deb

# Or install with apt to handle dependencies
sudo apt install ./alias-manager_2.1.0_amd64.deb
```

### From Source

```bash
# Clone and build
git clone https://github.com/kazetachinuu/alias_manager.git
cd alias_manager
make

# Install system-wide
sudo make install

# Or install to your home directory
make PREFIX=$HOME/.local install
```

### Post-Installation Setup

After installing via any method, set up shell integration:

```bash
# Auto-detect your shell and setup
eval "$(am init)"

# Make it permanent by adding to your shell config:
echo 'eval "$(am init)"' >> ~/.bashrc   # For bash
echo 'eval "$(am init)"' >> ~/.zshrc   # For zsh
am init fish >> ~/.config/fish/config.fish  # For fish
```

## Quick Start

```bash
# Setup (run once)
eval "$(am init)"

# Create aliases
am add gl git log --oneline --graph
am add gc git commit -m
am add gs git status

# List all aliases
am ls

# Search for git-related aliases
am ls git

# Remove an alias (with confirmation)
am rm gl

# Remove an alias (force, no confirmation)
am rm gl -f

# Get help
am help
```

## Command Reference

| Command | Description | Example |
|---------|-------------|---------|
| `init [shell]` | Output shell configuration | `eval "$(am init)"` |
| `add <name> <command...>` | Create or update an alias | `am add gc git commit -m` |
| `rm <name> [-f]` | Remove an alias | `am rm gc` |
| `ls [pattern]` | List all aliases or filter by pattern | `am ls git` |
| `help` / `--help` | Show help message | `am help` |
| `version` / `--version` | Show version | `am version` |

For detailed information, see the man page: `man am`


## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


## Roadmap

- [ ] Add alias backup/restore functionality
- [ ] Add shell completion scripts (bash, zsh, fish)
- [ ] Support for alias categories/groups
- [ ] RPM packages for Fedora/RHEL
- [ ] Snap/Flatpak packages

