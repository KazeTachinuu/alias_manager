# Package Management

This directory contains packaging files and scripts for different package managers.

## Directory Structure

```
packaging/
├── debian/          # Debian/Ubuntu .deb packages
│   ├── control      # Package metadata
│   └── build-deb.sh # Build script
├── homebrew/        # Homebrew formula (maintained in separate repo)
│   └── README.md    # Instructions
└── arch/            # Arch Linux (AUR, maintained separately)
    └── README.md    # Instructions
```

## Building Packages

### Debian/Ubuntu (.deb)

```bash
# On a Debian/Ubuntu system:
./packaging/debian/build-deb.sh

# Output: packaging/debian/alias-manager_2.1.0_amd64.deb
```

### Arch Linux (AUR)

The AUR package is maintained at: https://aur.archlinux.org/packages/alias-manager

Files: `PKGBUILD` and `.SRCINFO` in the root directory.

To update:
```bash
# Update PKGBUILD version and sha256sum
makepkg --printsrcinfo > .SRCINFO
# Push to AUR repository
```

### Homebrew

The Homebrew formula is maintained in a separate tap repository:
https://github.com/kazetachinuu/homebrew-alias-manager

To update:
```bash
# Update Formula/alias-manager.rb with new version and sha256
# Push to homebrew tap repository
```

## Release Checklist

When releasing a new version:

1. **Update version** in `Makefile`
2. **Update version** in `PKGBUILD`
3. **Update version** in `am.1` man page
4. **Commit changes** and create git tag
5. **Create GitHub release**
6. **Update Homebrew formula** with new version and sha256
7. **Build .deb package** and attach to GitHub release
8. **Update AUR package** with new sha256sum
9. **Test installations** from all package managers

## Version Management

All version numbers are defined in:
- `Makefile` (line 1): `VERSION = X.Y.Z`
- `PKGBUILD` (line 3): `pkgver=X.Y.Z`
- `am.1` (line 1): `.TH AM 1 "Month Year" "X.Y.Z"`

Keep these synchronized!
