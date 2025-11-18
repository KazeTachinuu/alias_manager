# Alias Manager

A simple CLI tool to manage your shell aliases.

## Install

```bash
# Arch Linux
yay -S alias-manager

# From source
git clone https://github.com/kazetachinuu/alias_manager.git
cd alias_manager
make && sudo make install
```

## Setup

```bash
# Add to ~/.bashrc or ~/.zshrc
eval "$(am init)"
```

## Usage

```bash
am add gc "git commit -m"     # Add alias
am ls                         # List all
am ls git                     # Search
am rm gc                      # Remove
```

Run `am help` or `man am` for details.

## License

MIT
