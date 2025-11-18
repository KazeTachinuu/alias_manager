# Alias Manager

> A lightweight CLI tool to manage your shell aliases efficiently

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![AUR version](https://img.shields.io/aur/version/alias-manager)](https://aur.archlinux.org/packages/alias-manager)
[![GitHub release](https://img.shields.io/github/v/release/kazetachinuu/alias_manager)](https://github.com/kazetachinuu/alias_manager/releases)

## Features

- **Simple**: Add, remove, and list aliases with easy commands
- **Safe**: Confirmation prompts before removing aliases
- **Fast**: Written in C with zero dependencies
- **Universal**: Works with bash, zsh, fish, and any POSIX shell
- **Smart**: Auto-detects terminal capabilities for colored output
- **Memory-safe**: No dynamic allocation, stack-only implementation

## Quick Start

```bash
# Install (see Installation section for your platform)
brew install kazetachinuu/alias-manager/alias-manager  # macOS/Linux
# or
yay -S alias-manager                                    # Arch Linux

# Set up (add to ~/.bashrc or ~/.zshrc)
eval "$(am init)"

# Create your first alias
am add gc "git commit -m"

# Use it!
gc "my commit message"
```

## Installation

### Arch Linux

```bash
yay -S alias-manager
# or
paru -S alias-manager
```

### macOS / Linux (Homebrew)

```bash
brew install kazetachinuu/alias-manager/alias-manager
```

### Debian / Ubuntu

```bash
# Download and install the .deb package
wget https://github.com/kazetachinuu/alias_manager/releases/latest/download/alias-manager_2.1.2_amd64.deb
sudo dpkg -i alias-manager_2.1.2_amd64.deb
```

### Other Linux (from tarball)

```bash
# Download the binary
wget https://github.com/kazetachinuu/alias_manager/releases/latest/download/alias-manager-2.1.2-linux-amd64.tar.gz
tar -xzf alias-manager-2.1.2-linux-amd64.tar.gz

# Install
sudo install -m 755 am /usr/local/bin/am
sudo install -m 644 am.1 /usr/local/share/man/man1/am.1
```

### From Source

```bash
git clone https://github.com/kazetachinuu/alias_manager.git
cd alias_manager
make
sudo make install
```

## Setup

After installation, add one of these lines to your shell configuration file:

**Bash** (`~/.bashrc`):
```bash
eval "$(am init)"
```

**Zsh** (`~/.zshrc`):
```bash
eval "$(am init)"
```

**Fish** (`~/.config/fish/config.fish`):
```bash
am init fish | source
```

Then reload your shell:
```bash
source ~/.bashrc  # or ~/.zshrc
```

## Usage

### Add an Alias

```bash
# Basic usage
am add <name> <command>

# Examples
am add ll "ls -lah"
am add gc "git commit -m"
am add gp "git push origin"
am add update "sudo apt update && sudo apt upgrade -y"
```

### List Aliases

```bash
# List all aliases
am ls

# Search for specific aliases
am ls git    # Shows all aliases containing "git"
```

### Remove an Alias

```bash
# Remove with confirmation prompt
am rm gc

# Force remove without confirmation
am rm gc -f
```

### Get Help

```bash
# Show help
am help

# Show version
am version

# Read manual page
man am
```

## Command Reference

| Command | Description | Example |
|---------|-------------|---------|
| `am add <name> <command>` | Create or update an alias | `am add gc "git commit -m"` |
| `am rm <name> [-f]` | Remove an alias (optionally force) | `am rm gc` or `am rm gc -f` |
| `am ls [pattern]` | List all aliases or filter by pattern | `am ls` or `am ls git` |
| `am init [shell]` | Generate shell integration code | `am init` or `am init fish` |
| `am help` | Show help message | `am help` |
| `am version` | Show version information | `am version` |

## Configuration

### Alias Storage Location

By default, aliases are stored in:
- **XDG-compliant**: `~/.config/am/aliases.txt` (preferred)
- **Legacy fallback**: `~/.my_aliases.txt` (if it exists)

### Custom Location

You can override the storage location with an environment variable:

```bash
export AM_ALIAS_FILE="$HOME/my-custom-aliases.txt"
```

Add this to your shell configuration file before the `eval "$(am init)"` line.

## Examples

### Git Workflow Aliases

```bash
am add gs "git status"
am add ga "git add"
am add gc "git commit -m"
am add gp "git push"
am add gl "git log --oneline --graph --all"
am add gd "git diff"
```

### System Shortcuts

```bash
am add update "sudo apt update && sudo apt upgrade -y"
am add cleanup "sudo apt autoremove && sudo apt autoclean"
am add ports "sudo netstat -tulpn"
am add myip "curl -s ifconfig.me"
```

### Directory Navigation

```bash
am add projects "cd ~/projects"
am add downloads "cd ~/Downloads"
am add docs "cd ~/Documents"
```

## Building from Source

Requirements:
- C11-compatible compiler (gcc, clang)
- make
- POSIX-compliant system

```bash
git clone https://github.com/kazetachinuu/alias_manager.git
cd alias_manager
make
sudo make install
```

To uninstall:
```bash
sudo make uninstall
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

**Hugo Sibony** - [@KazeTachinuu](https://github.com/KazeTachinuu)
