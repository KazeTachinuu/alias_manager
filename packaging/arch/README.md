# Arch Linux (AUR) Packaging

The Arch Linux package is maintained on the AUR.

## Package Location

- **AUR Page**: https://aur.archlinux.org/packages/alias-manager
- **Git Repository**: ssh://aur@aur.archlinux.org/alias-manager.git

## Files

The packaging files are in the project root:
- `PKGBUILD` - Package build script
- `.SRCINFO` - Package metadata (auto-generated)

## Updating the Package

1. Update version in `PKGBUILD`:
   ```bash
   pkgver=X.Y.Z
   ```

2. Create GitHub release and get SHA256:
   ```bash
   wget https://github.com/kazetachinuu/alias_manager/archive/vX.Y.Z.tar.gz
   sha256sum vX.Y.Z.tar.gz
   ```

3. Update `sha256sums` in `PKGBUILD`

4. Regenerate `.SRCINFO`:
   ```bash
   makepkg --printsrcinfo > .SRCINFO
   ```

5. Test build:
   ```bash
   makepkg --clean
   ```

6. Push to AUR:
   ```bash
   cd /path/to/aur-alias-manager
   cp ../alias_manager/PKGBUILD .
   cp ../alias_manager/.SRCINFO .
   git add PKGBUILD .SRCINFO
   git commit -m "Update to version X.Y.Z"
   git push
   ```

## Validation

Always run before pushing:
```bash
namcap PKGBUILD
namcap *.pkg.tar.zst
```
