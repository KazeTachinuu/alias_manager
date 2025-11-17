#!/bin/bash
# Master build script for all package types

set -e

echo "=== Alias Manager Package Builder ==="
echo ""

VERSION=$(grep "^VERSION = " Makefile | cut -d' ' -f3)
echo "Building version: $VERSION"
echo ""

# Check if on correct system for .deb
if command -v dpkg-deb &> /dev/null; then
    echo "→ Building .deb package..."
    ./packaging/debian/build-deb.sh
    echo ""
else
    echo "⊘ Skipping .deb (dpkg-deb not available - run on Debian/Ubuntu)"
    echo ""
fi

# Arch Linux
echo "→ Checking PKGBUILD..."
if [ -f "PKGBUILD" ]; then
    if command -v namcap &> /dev/null; then
        echo "  Running namcap validation..."
        namcap PKGBUILD || echo "  Warning: namcap found issues"
    else
        echo "  ⊘ Skipping validation (namcap not available)"
    fi
else
    echo "  ✗ PKGBUILD not found!"
fi
echo ""

echo "=== Build Summary ==="
echo ""
echo "Version: $VERSION"
echo ""
echo "Built packages:"
ls -lh packaging/deb/*.deb 2>/dev/null || echo "  No .deb packages (run on Debian/Ubuntu)"
ls -lh *.pkg.tar.zst 2>/dev/null || echo "  No .pkg.tar.zst packages"
echo ""
echo "External repositories:"
echo "  Homebrew: ../homebrew-alias-manager/Formula/alias-manager.rb"
echo "  AUR: ssh://aur@aur.archlinux.org/alias-manager.git"
echo ""
echo "Next steps:"
echo "  1. Test packages locally"
echo "  2. Create GitHub release (v$VERSION)"
echo "  3. Attach .deb to release"
echo "  4. Update Homebrew formula"
echo "  5. Update AUR (PKGBUILD + .SRCINFO)"
echo ""
echo "See PACKAGING.md for detailed instructions."
