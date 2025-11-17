# Homebrew Packaging

The Homebrew formula is maintained in a separate tap repository.

## Tap Repository

- **Repository**: https://github.com/kazetachinuu/homebrew-alias-manager
- **Formula**: `Formula/alias-manager.rb`

## Updating the Formula

1. Create GitHub release for the new version

2. Get SHA256 of the release tarball:
   ```bash
   wget https://github.com/kazetachinuu/alias_manager/archive/vX.Y.Z.tar.gz
   sha256sum vX.Y.Z.tar.gz
   ```

3. Update `Formula/alias-manager.rb`:
   ```ruby
   url "https://github.com/kazetachinuu/alias_manager/archive/vX.Y.Z.tar.gz"
   sha256 "new_sha256_here"
   ```

4. Test the formula locally:
   ```bash
   brew install --build-from-source ./Formula/alias-manager.rb
   brew test alias-manager
   brew audit --strict alias-manager
   ```

5. Push to tap repository:
   ```bash
   cd /path/to/homebrew-alias-manager
   git add Formula/alias-manager.rb
   git commit -m "Update alias-manager to X.Y.Z"
   git push
   ```

## Installation

Users can install with:
```bash
brew install kazetachinuu/alias-manager/alias-manager
```

Or:
```bash
brew tap kazetachinuu/alias-manager
brew install alias-manager
```
