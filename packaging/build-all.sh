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
    if command -v makepkg &> /dev/null; then
        echo "  Running namcap validation..."
        namcap PKGBUILD || echo "  Warning: namcap found issues"
    else
        echo "  ⊘ Skipping validation (makepkg not available)"
    fi
else
    echo "  ✗ PKGBUILD not found!"
fi
echo ""

# Homebrew
echo "→ Homebrew formula location:"
echo "  ../homebrew-alias-manager/Formula/alias-manager.rb"
echo "  Update manually after GitHub release"
echo ""

echo "=== Build Summary ==="
echo ""
echo "Built packages:"
ls -lh packaging/debian/*.deb 2>/dev/null || echo "  No .deb packages"
ls -lh *.pkg.tar.zst 2>/dev/null || echo "  No .pkg.tar.zst packages"
echo ""
echo "Next steps:"
echo "1. Test all packages"
echo "2. Attach .deb to GitHub release"
echo "3. Update Homebrew formula"
echo "4. Push PKGBUILD/.SRCINFO to AUR"
