# Package Management

This directory contains packaging configurations and build scripts for different platforms.

## Directory Structure

```
packaging/
├── deb/             # Debian/Ubuntu packages
│   ├── control      # Package metadata
│   └── build.sh     # Build script
├── arch/            # Arch Linux documentation
│   └── README.md    # AUR packaging guide
├── homebrew/        # Homebrew documentation
│   └── README.md    # Tap repository guide
├── build-all.sh     # Master build script
└── README.md        # This file
```

**Note:** Actual packaging files for Arch Linux (PKGBUILD, .SRCINFO) are in the project root, as per AUR conventions.

## Quick Build

```bash
# Build all packages (where possible)
./packaging/build-all.sh

# Build specific package
./packaging/deb/build.sh          # Debian/Ubuntu (requires dpkg-deb)
```

## Platform-Specific Instructions

### Arch Linux (AUR)

See: `packaging/arch/README.md` or root PKGBUILD/. SRCINFO

**Location:** https://aur.archlinux.org/packages/alias-manager

### Homebrew

See: `packaging/homebrew/README.md`

**Repository:** https://github.com/kazetachinuu/homebrew-alias-manager

### Debian/Ubuntu

See: `packaging/deb/`

Build with: `./packaging/deb/build.sh`

## Release Workflow

See `PACKAGING.md` in the project root for the complete release workflow and version management guide.
