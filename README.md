# Alias Manager (am)

> A lightweight CLI tool to manage your shell aliases efficiently



## 🚀 Features

- ✨ Create and update aliases with a simple command
- 🗑️ Remove aliases safely (with confirmation)
- 📋 List all your aliases
- 🔍 Search aliases by pattern
- 💪 Works with any POSIX-compliant shell (bash, zsh, fish)
- 🎨 Smart color output (auto-detects TTY)
- 📖 Comprehensive man page
- 🔒 Memory-safe implementation (no malloc)
- ⚡ Zero dependencies (only libc)

## 📦 Installation

### From Source

```bash
# Clone and build
git clone https://github.com/kazetachinuu/alias_manager.git
cd alias_manager
make && sudo make install

# Setup shell integration (auto-detects your shell)
eval "$(am init)"

# Or add to your shell config for persistence:
echo 'eval "$(am init)"' >> ~/.bashrc   # For bash
echo 'eval "$(am init)"' >> ~/.zshrc   # For zsh
am init fish >> ~/.config/fish/config.fish  # For fish
```

### Arch Linux (AUR)

```bash
# Using an AUR helper like yay
yay -S alias-manager

# Or manually
git clone https://aur.archlinux.org/alias-manager.git
cd alias-manager
makepkg -si
```

### Other Package Managers

Coming soon...

## 🎯 Quick Start

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

## 📖 Command Reference

| Command | Description | Example |
|---------|-------------|---------|
| `init [shell]` | Output shell configuration | `eval "$(am init)"` |
| `add <name> <command...>` | Create or update an alias | `am add gc git commit -m` |
| `rm <name> [-f]` | Remove an alias | `am rm gc` |
| `ls [pattern]` | List all aliases or filter by pattern | `am ls git` |
| `help` / `--help` | Show help message | `am help` |
| `version` / `--version` | Show version | `am version` |

For detailed information, see the man page: `man am`


## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


## 📝 TODO

- [ ] Add package manager support (apt, brew, etc.)
- [ ] Add alias backup/restore functionality
- [ ] Add shell completion scripts
- [ ] Support for alias categories/groups

