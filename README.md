# Alias Manager

> A lightweight CLI tool to manage your shell aliases efficiently

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![AUR version](https://img.shields.io/aur/version/alias-manager)](https://aur.archlinux.org/packages/alias-manager)
[![GitHub release](https://img.shields.io/github/v/release/kazetachinuu/alias_manager)](https://github.com/kazetachinuu/alias_manager/releases)

## Features

- **Simple**: Add, remove, and list aliases with easy commands
- **Helpful**: Clear hints guide you to reload aliases after changes
- **Safe**: Confirmation prompts before removing aliases
- **Fast**: Written in C with zero dependencies
- **Universal**: Works with bash, zsh, fish, and any POSIX shell
- **Smart**: Auto-detects terminal capabilities for colored output
- **Memory-safe**: No dynamic allocation, stack-only implementation
- **XDG-compliant**: Follows modern standards for configuration storage

## Quick Start

```bash
# 1. Install
brew install kazetachinuu/alias-manager/alias-manager
# or: yay -S alias-manager

# 2. Setup
am init
source ~/.bashrc  # or ~/.zshrc

# 3. Use
am add gc "git commit -m"
eval "$(am reload)"
gc "my first commit"
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
wget https://github.com/kazetachinuu/alias_manager/releases/latest/download/alias-manager_2.3.1_amd64.deb
sudo dpkg -i alias-manager_2.3.1_amd64.deb
```

### Other Linux (from tarball)

```bash
# Download the binary
wget https://github.com/kazetachinuu/alias_manager/releases/latest/download/alias-manager-2.3.1-linux-amd64.tar.gz
tar -xzf alias-manager-2.3.1-linux-amd64.tar.gz

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

```bash
am init
source ~/.bashrc  # or ~/.zshrc
```

The `init` command detects your shell and adds integration to your config file (`~/.bashrc`, `~/.zshrc`, or `~/.config/fish/config.fish`).

### Manual Setup

Add to your shell config:
```bash
eval "$(am init --hook)"
```

## Usage

```bash
# Add an alias
am add gc "git commit -m"
# ✓ Added alias 'gc' → 'git commit -m'
#   (run "eval $(am reload)" to use it now)
eval "$(am reload)"

# List aliases
am ls
am ls git  # Filter by pattern

# Remove an alias
am rm gc
# ✓ Removed alias 'gc'
#   (run "unalias gc" to remove from current shell)
unalias gc

# Help
am help
am version
```

## Command Reference

| Command | Description | Example |
|---------|-------------|---------|
| `am init [shell]` | Interactive setup (modifies shell config) | `am init` |
| `am add <name> <command>` | Create or update an alias | `am add gc "git commit -m"` |
| `am rm <name> [-f]` | Remove an alias (optionally force) | `am rm gc` or `am rm gc -f` |
| `am ls [pattern]` | List all aliases or filter by pattern | `am ls` or `am ls git` |
| `am reload [shell]` | Output reload command for current shell | `eval "$(am reload)"` |
| `am path` | Show alias file path | `am path` |
| `am init --hook [shell]` | Output shell hook code (for manual setup) | `eval "$(am init --hook)"` |
| `am help` | Show help message | `am help` |
| `am version` | Show version information | `am version` |

### Optional: Quick Reload Alias

Create an `amr` alias for faster reloading:

```bash
am add amr 'eval "$(am reload)"'
eval "$(am reload)"
```

Now:
```bash
am add gp "git push"
amr  # Instead of eval "$(am reload)"
gp
```

## Configuration

### Alias Storage Location

Aliases are stored in the XDG-compliant location:
- `${XDG_CONFIG_HOME}/am/aliases.txt` (if `XDG_CONFIG_HOME` is set)
- `~/.config/am/aliases.txt` (default)

The directory is created automatically on first use.

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
