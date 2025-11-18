#!/bin/bash
# Release automation script for alias-manager
# This automates the tedious parts while keeping you in control

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== Alias Manager Release Script ===${NC}\n"

# Check if we're on master branch
BRANCH=$(git rev-parse --abbrev-ref HEAD)
if [ "$BRANCH" != "master" ]; then
    echo -e "${RED}Error: Must be on master branch${NC}"
    exit 1
fi

# Check for uncommitted changes
if ! git diff-index --quiet HEAD --; then
    echo -e "${RED}Error: Uncommitted changes detected${NC}"
    exit 1
fi

# Get current version
CURRENT_VERSION=$(grep "^VERSION = " Makefile | cut -d' ' -f3)
echo -e "Current version: ${YELLOW}$CURRENT_VERSION${NC}"

# Prompt for new version
read -p "Enter new version (e.g., 2.1.3): " NEW_VERSION
if [ -z "$NEW_VERSION" ]; then
    echo -e "${RED}Error: Version cannot be empty${NC}"
    exit 1
fi

# Confirm
echo -e "\n${YELLOW}This will:${NC}"
echo "  1. Update version in Makefile, PKGBUILD, debian/control, README.md"
echo "  2. Commit changes"
echo "  3. Create and push git tag v$NEW_VERSION"
echo "  4. Wait for GitHub Actions to build release"
echo "  5. Update PKGBUILD with correct SHA256"
echo "  6. Provide instructions for AUR and Homebrew"
echo ""
read -p "Continue? [y/N] " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Aborted."
    exit 1
fi

echo -e "\n${BLUE}Step 1: Updating version files...${NC}"

# Update Makefile
sed -i.bak "s/^VERSION = .*/VERSION = $NEW_VERSION/" Makefile && rm Makefile.bak
echo -e "  ${GREEN}✓${NC} Makefile"

# Update PKGBUILD
sed -i.bak "s/^pkgver=.*/pkgver=$NEW_VERSION/" PKGBUILD && rm PKGBUILD.bak
echo -e "  ${GREEN}✓${NC} PKGBUILD"

# Update debian/control
sed -i.bak "s/^Version: .*/Version: $NEW_VERSION/" debian/control && rm debian/control.bak
echo -e "  ${GREEN}✓${NC} debian/control"

# Update README.md
sed -i.bak "s/alias-manager_[0-9]*\.[0-9]*\.[0-9]*_amd64\.deb/alias-manager_${NEW_VERSION}_amd64.deb/g" README.md && rm README.md.bak
sed -i.bak "s/alias-manager-[0-9]*\.[0-9]*\.[0-9]*-linux-amd64\.tar\.gz/alias-manager-${NEW_VERSION}-linux-amd64.tar.gz/g" README.md && rm README.md.bak
echo -e "  ${GREEN}✓${NC} README.md"

# Build and test
echo -e "\n${BLUE}Step 2: Building and testing...${NC}"
make clean
make
./test.sh > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo -e "  ${GREEN}✓${NC} All tests passed"
else
    echo -e "  ${RED}✗${NC} Tests failed"
    exit 1
fi

# Commit
echo -e "\n${BLUE}Step 3: Committing changes...${NC}"
git add Makefile PKGBUILD debian/control README.md
git commit -m "v$NEW_VERSION: Bump version"
echo -e "  ${GREEN}✓${NC} Committed"

# Create tag
echo -e "\n${BLUE}Step 4: Creating git tag...${NC}"
git tag "v$NEW_VERSION"
echo -e "  ${GREEN}✓${NC} Tag created"

# Push
echo -e "\n${BLUE}Step 5: Pushing to GitHub...${NC}"
git push origin master
git push origin "v$NEW_VERSION"
echo -e "  ${GREEN}✓${NC} Pushed"

# Wait for GitHub Actions
echo -e "\n${BLUE}Step 6: Waiting for GitHub Actions to create release...${NC}"
TIMEOUT=180
INTERVAL=10
ELAPSED=0
echo -n "  Waiting"
while [ $ELAPSED -lt $TIMEOUT ]; do
    if gh release view "v$NEW_VERSION" &>/dev/null; then
        echo -e "\n  ${GREEN}✓${NC} Release v$NEW_VERSION is available"
        break
    fi
    echo -n "."
    sleep $INTERVAL
    ELAPSED=$((ELAPSED + INTERVAL))
done

if [ $ELAPSED -ge $TIMEOUT ]; then
    echo -e "\n  ${YELLOW}!${NC} Timeout - check manually: gh release view v$NEW_VERSION"
fi

# Get SHA256
echo -e "\n${BLUE}Step 7: Fetching release tarball and calculating SHA256...${NC}"
TARBALL_URL="https://github.com/kazetachinuu/alias_manager/archive/v$NEW_VERSION.tar.gz"
SHA256=$(curl -sL "$TARBALL_URL" | sha256sum | cut -d' ' -f1)

if [ -z "$SHA256" ] || [ "$SHA256" == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855" ]; then
    echo -e "  ${RED}✗${NC} Failed to fetch release (GitHub may still be building)"
    echo -e "\n${YELLOW}Manual step needed:${NC}"
    echo "  1. Wait a bit longer for GitHub Actions"
    echo "  2. Get SHA256 manually:"
    echo "     wget $TARBALL_URL"
    echo "     sha256sum v$NEW_VERSION.tar.gz"
else
    echo -e "  ${GREEN}✓${NC} SHA256: $SHA256"

    # Update PKGBUILD
    echo -e "\n${BLUE}Step 8: Updating PKGBUILD with SHA256...${NC}"
    sed -i.bak "s/^sha256sums=.*/sha256sums=('$SHA256')/" PKGBUILD && rm PKGBUILD.bak
    echo -e "  ${GREEN}✓${NC} PKGBUILD updated"

    # Update .SRCINFO
    if command -v makepkg &> /dev/null; then
        makepkg --printsrcinfo > .SRCINFO
        echo -e "  ${GREEN}✓${NC} .SRCINFO updated"
    else
        echo -e "  ${YELLOW}!${NC} makepkg not found - update .SRCINFO manually on Arch Linux"
        # Manual update for .SRCINFO
        cat > .SRCINFO << EOF
pkgbase = alias-manager
	pkgdesc = A lightweight CLI tool to manage your shell aliases efficiently
	pkgver = $NEW_VERSION
	pkgrel = 1
	url = https://github.com/kazetachinuu/alias_manager
	arch = x86_64
	arch = i686
	arch = aarch64
	arch = armv7h
	license = MIT
	makedepends = gcc
	makedepends = make
	depends = glibc
	source = alias-manager-$NEW_VERSION.tar.gz::https://github.com/kazetachinuu/alias_manager/archive/v$NEW_VERSION.tar.gz
	sha256sums = $SHA256

pkgname = alias-manager
EOF
        echo -e "  ${GREEN}✓${NC} .SRCINFO generated"
    fi

    # Commit updated PKGBUILD
    git add PKGBUILD .SRCINFO
    git commit -m "Update PKGBUILD sha256sum for v$NEW_VERSION"
    git push origin master
    echo -e "  ${GREEN}✓${NC} Changes pushed"
fi

# Final instructions
echo -e "\n${GREEN}=== Release v$NEW_VERSION Complete! ===${NC}\n"
echo -e "${YELLOW}Next steps:${NC}\n"

echo -e "${BLUE}1. Publish to AUR:${NC}"
echo "   cd ~/path/to/aur-alias-manager"
echo "   cp ~/path/to/alias_manager/PKGBUILD ."
echo "   cp ~/path/to/alias_manager/.SRCINFO ."
echo "   git add PKGBUILD .SRCINFO"
echo "   git commit -m 'Update to v$NEW_VERSION'"
echo "   git push"
echo ""

echo -e "${BLUE}2. Update Homebrew formula:${NC}"
echo "   cd ~/path/to/homebrew-alias-manager"
echo "   # Edit Formula/alias-manager.rb:"
echo "   #   url \"...archive/v$NEW_VERSION.tar.gz\""
echo "   #   sha256 \"$SHA256\""
echo "   git add Formula/alias-manager.rb"
echo "   git commit -m 'Update alias-manager to $NEW_VERSION'"
echo "   git push"
echo ""

echo -e "${BLUE}3. Verify release:${NC}"
echo "   GitHub: https://github.com/kazetachinuu/alias_manager/releases/tag/v$NEW_VERSION"
echo "   AUR: https://aur.archlinux.org/packages/alias-manager"
echo ""

echo -e "${GREEN}Done! 🎉${NC}"
