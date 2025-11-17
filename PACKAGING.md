# Packaging Guide

This document describes how to create and maintain packages for different platforms.

## Quick Reference

| Platform | Location | Command |
|----------|----------|---------|
| **Arch Linux (AUR)** | Root: `PKGBUILD`, `.SRCINFO` | `makepkg -si` |
| **Homebrew** | Separate repo: `homebrew-alias-manager` | `brew install kazetachinuu/alias-manager/alias-manager` |
| **Debian/Ubuntu** | `packaging/deb/` | `./packaging/deb/build.sh` |

## Version Management

Version is defined in `Makefile`:
```make
VERSION = 2.1.0
```

This version must be synchronized across:
- `Makefile` (line 1)
- `PKGBUILD` (pkgver field)
- `am.1` man page (TH header)
- Homebrew formula

## Release Process

### 1. Update Version

```bash
# Update VERSION in Makefile
sed -i 's/VERSION = .*/VERSION = X.Y.Z/' Makefile

# Update PKGBUILD
sed -i 's/pkgver=.*/pkgver=X.Y.Z/' PKGBUILD

# Update man page
sed -i 's/"[0-9.]*" "Alias Manager"/"X.Y.Z" "Alias Manager"/' am.1

# Regenerate .SRCINFO
makepkg --printsrcinfo > .SRCINFO
```

### 2. Create GitHub Release

```bash
# Commit version changes
git add Makefile PKGBUILD .SRCINFO am.1
git commit -m "Bump version to X.Y.Z"
git push

# Tag and push
git tag -a vX.Y.Z -m "Release version X.Y.Z"
git push origin vX.Y.Z

# Create release on GitHub
gh release create vX.Y.Z --title "Release X.Y.Z" --notes "..."
```

### 3. Get SHA256 Checksums

```bash
wget https://github.com/kazetachinuu/alias_manager/archive/vX.Y.Z.tar.gz
sha256sum vX.Y.Z.tar.gz
```

### 4. Update Packages

#### Arch Linux (AUR)

```bash
# Update sha256sums in PKGBUILD
makepkg --printsrcinfo > .SRCINFO
namcap PKGBUILD

# Push to AUR
cd ../alias-manager-aur
cp ../alias_manager/{PKGBUILD,.SRCINFO} .
git add PKGBUILD .SRCINFO
git commit -m "Update to X.Y.Z"
git push
```

#### Homebrew

```bash
cd ../homebrew-alias-manager
# Update Formula/alias-manager.rb
#   - url
#   - sha256
git add Formula/alias-manager.rb
git commit -m "Update alias-manager to X.Y.Z"
git push
```

#### Debian/Ubuntu

```bash
# Build .deb package (on Debian/Ubuntu system)
./packaging/deb/build.sh

# Attach to GitHub release
gh release upload vX.Y.Z packaging/deb/alias-manager_X.Y.Z_amd64.deb
```

## Platform-Specific Details

### Arch Linux

Files: `PKGBUILD`, `.SRCINFO` (root directory)

- PKGBUILD defines build process
- .SRCINFO is auto-generated metadata
- Maintained on AUR: https://aur.archlinux.org/packages/alias-manager
- Validation: `namcap PKGBUILD`

### Homebrew

Repository: https://github.com/kazetachinuu/homebrew-alias-manager

- Formula: `Formula/alias-manager.rb`
- Ruby-based DSL
- Validation: `brew audit --strict alias-manager`
- Testing: `brew test alias-manager`

### Debian/Ubuntu

Directory: `packaging/deb/`

- `control`: Package metadata
- `build.sh`: Build script
- Outputs to: `packaging/deb/alias-manager_X.Y.Z_amd64.deb`
- Validation: `lintian *.deb`

## Testing

Before releasing, test installation on each platform:

```bash
# Arch Linux
makepkg -si

# Homebrew (local test)
brew install --build-from-source ./Formula/alias-manager.rb

# Debian/Ubuntu
sudo dpkg -i alias-manager_X.Y.Z_amd64.deb
```

Verify:
```bash
am version  # Should show X.Y.Z
man am      # Should display man page
am init     # Should output shell config
```

## Troubleshooting

**Version mismatch errors:**
- Ensure all files use same version
- Regenerate .SRCINFO after PKGBUILD changes

**SHA256 checksum fails:**
- Clear old tarballs: `rm *.tar.gz`
- Download fresh: `wget https://...`
- Verify checksum: `sha256sum vX.Y.Z.tar.gz`

**Build fails:**
- Check compiler flags in Makefile
- Verify VERSION flag is passed: `make clean && make`
- Test with: `./am version`

## See Also

- AUR Guidelines: https://wiki.archlinux.org/title/AUR_submission_guidelines
- Homebrew Formula Cookbook: https://docs.brew.sh/Formula-Cookbook
- Debian Policy: https://www.debian.org/doc/debian-policy/
