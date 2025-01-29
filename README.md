# Alias Manager (am)

> A lightweight CLI tool to manage your shell aliases efficiently



## 🚀 Features

- ✨ Create and update aliases with a simple command
- 🗑️ Remove aliases safely (with confirmation)
- 📋 List all your aliases
- 🔍 Search aliases by pattern
- 💪 Works with any POSIX-compliant shell (bash, zsh, fish)

## 📦 Installation

### From Source

```bash
# Clone the repository
git clone https://github.com/kazetachinuu/alias_manager.git

# Navigate to the directory
cd alias_manager

# Build and install
make
sudo make install

# Add to your shell configuration
echo '[[ -f ~/.my_aliases.txt ]] && source ~/.my_aliases.txt' >> ~/.bashrc  # For bash
# OR
echo '[[ -f ~/.my_aliases.txt ]] && source ~/.my_aliases.txt' >> ~/.zshrc   # For zsh
```

### Package Managers

Coming soon...

## 🎯 Quick Start

```bash
# Create a new alias
am add gl git log --oneline --graph

# List all aliases
am ls

# Search for git-related aliases
am ls git

# Remove an alias (with confirmation)
am rm gl

# Remove an alias (force)
am rm gl -f

# Show help
am help
```

## 📖 Command Reference

| Command | Description | Example |
|---------|-------------|---------|
| `add <name> <command...>` | Create or update an alias | `am add gc git commit -m` |
| `rm <name> [-f]` | Remove an alias | `am rm gc` |
| `ls [pattern]` | List all aliases or filter by pattern | `am ls git` |
| `help` | Show help message | `am help` |
| `version` | Show version | `am version` |


## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


## 📝 TODO

- [ ] Add package manager support (apt, brew, etc.)
- [ ] Add alias backup/restore functionality
- [ ] Add shell completion scripts
- [ ] Support for alias categories/groups

