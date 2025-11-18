# Alias Manager

A lightweight CLI tool to manage your shell aliases efficiently

## Features

- Create and update aliases with a simple command
- Remove aliases safely (with confirmation)
- List and search aliases
- Works with any POSIX-compliant shell (bash, zsh, fish)
- Smart color output (auto-detects TTY)
- Memory-safe implementation (no malloc)
- Zero dependencies (only libc)

## Installation

### Arch Linux (AUR)

```bash
yay -S alias-manager
```

### Homebrew (macOS/Linux)

```bash
brew install kazetachinuu/alias-manager/alias-manager
```

### Debian/Ubuntu

Download the `.deb` package from the [releases page](https://github.com/kazetachinuu/alias_manager/releases):

```bash
sudo dpkg -i alias-manager_*_amd64.deb
```

### From Source

```bash
git clone https://github.com/kazetachinuu/alias_manager.git
cd alias_manager
make
sudo make install
```

## Setup

After installation, add this to your shell configuration:

```bash
# ~/.bashrc or ~/.zshrc
eval "$(am init)"
```

For fish shell:

```bash
# ~/.config/fish/config.fish
am init fish | source
```

## Usage

```bash
# Create a new alias
am add gc "git commit -m"

# List all aliases
am ls

# Search for git-related aliases
am ls git

# Remove an alias (with confirmation)
am rm gc

# Remove an alias (force, no confirmation)
am rm gc -f

# Show help
am help

# Show version
am version
```

## Command Reference

| Command | Description | Example |
|---------|-------------|---------|
| `add <name> <command...>` | Create or update an alias | `am add gc "git commit -m"` |
| `rm <name> [-f]` | Remove an alias | `am rm gc` |
| `ls [pattern]` | List all aliases or filter by pattern | `am ls git` |
| `init [shell]` | Generate shell integration code | `am init` |
| `help` | Show help message | `am help` |
| `version` | Show version | `am version` |

## Configuration

By default, aliases are stored in:
- `~/.config/am/aliases.txt` (XDG-compliant)
- Or `~/.my_aliases.txt` (legacy fallback)

You can override this with the `AM_ALIAS_FILE` environment variable:

```bash
export AM_ALIAS_FILE="$HOME/my-custom-aliases.txt"
```

## License

MIT License - see [LICENSE](LICENSE) file for details
