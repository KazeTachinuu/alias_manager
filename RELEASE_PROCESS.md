# Release Process

## Current Workflow

### 1. Update Version

Update version in these files:
- `Makefile` (VERSION = X.Y.Z)
- `PKGBUILD` (pkgver=X.Y.Z)
- `debian/control` (Version: X.Y.Z)

### 2. Commit and Tag

```bash
git add -A
git commit -m "vX.Y.Z: Brief description"
git push origin master
git tag vX.Y.Z
git push origin vX.Y.Z
```

GitHub Actions will automatically:
- Build .deb package
- Build Linux tarball
- Build macOS tarball
- Create GitHub release with all assets

### 3. Update PKGBUILD SHA256

Wait ~2 minutes for GitHub release, then:

```bash
# Get SHA256 of the release tarball
wget https://github.com/kazetachinuu/alias_manager/archive/vX.Y.Z.tar.gz
sha256sum vX.Y.Z.tar.gz

# Update sha256sums in PKGBUILD
# Update .SRCINFO (if on Arch: makepkg --printsrcinfo > .SRCINFO)
```

### 4. Publish to AUR (Manual)

```bash
# Clone AUR repo (if not already cloned)
git clone ssh://aur@aur.archlinux.org/alias-manager.git aur-alias-manager
cd aur-alias-manager

# Copy updated files
cp ../alias_manager/PKGBUILD .
cp ../alias_manager/.SRCINFO .

# Test build (optional)
makepkg --clean

# Publish
git add PKGBUILD .SRCINFO
git commit -m "Update to vX.Y.Z"
git push
```

### 5. Update Homebrew Formula (Manual)

```bash
# Clone tap repo (if not already cloned)
git clone git@github.com:KazeTachinuu/homebrew-alias-manager.git
cd homebrew-alias-manager

# Update Formula/alias-manager.rb:
# - Update version: vX.Y.Z
# - Update sha256: (same SHA256 from step 3)

# Test (if on macOS)
brew install --build-from-source ./Formula/alias-manager.rb
brew test alias-manager
brew audit --strict alias-manager

# Publish
git add Formula/alias-manager.rb
git commit -m "Update alias-manager to X.Y.Z"
git push
```

## Automation Options

### Option 1: Release Script (Semi-automated)

Create `scripts/release.sh` to automate SHA256 fetching and file updates.

### Option 2: GitHub Actions (Fully automated)

Add workflow to automatically:
- Update AUR via bot account
- Update Homebrew formula via PR

### Option 3: Stay Manual

Keep it simple for small projects. Manual process takes ~5 minutes.

## Recommended: Semi-automated

For a small CLI tool, semi-automation is the sweet spot:
- Version bumps: Manual (you control what changes)
- SHA256 & file updates: Automated script
- Publishing: Manual review before push
