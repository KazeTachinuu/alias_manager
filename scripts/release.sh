#!/bin/bash
# Release automation script for alias-manager

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Parse arguments
DRY_RUN=false
if [ "$1" == "--dry-run" ] || [ "$1" == "-n" ]; then
    DRY_RUN=true
fi

if $DRY_RUN; then
    echo -e "${BLUE}=== Alias Manager Release (DRY RUN) ===${NC}\n"
else
    echo -e "${BLUE}=== Alias Manager Release ===${NC}\n"
fi

# Preflight checks
BRANCH=$(git rev-parse --abbrev-ref HEAD)
if [ "$BRANCH" != "master" ]; then
    echo -e "${RED}Error: Must be on master branch${NC}"
    exit 1
fi

if ! git diff-index --quiet HEAD --; then
    echo -e "${RED}Error: Uncommitted changes detected${NC}"
    exit 1
fi

# Version input
CURRENT_VERSION=$(grep "^VERSION = " Makefile | cut -d' ' -f3)
echo -e "Current version: ${YELLOW}$CURRENT_VERSION${NC}"
read -p "New version: " NEW_VERSION

# Validate version format
if ! [[ "$NEW_VERSION" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo -e "${RED}Error: Version must be in X.Y.Z format${NC}"
    exit 1
fi

# Confirm
echo -e "\n${YELLOW}Will update:${NC} Makefile, PKGBUILD, debian/control, README.md, am.1"
echo -e "${YELLOW}Will create:${NC} tag v$NEW_VERSION\n"
read -p "Continue? [y/N] " -n 1 -r
echo
[[ ! $REPLY =~ ^[Yy]$ ]] && echo "Aborted." && exit 1

# Update version in all files
echo -e "\n${BLUE}Updating files...${NC}"

if $DRY_RUN; then
    echo -e "  ${YELLOW}Would update:${NC} Makefile, PKGBUILD, debian/control, README.md, am.1"
else
    sed -i.bak "s/^VERSION = .*/VERSION = $NEW_VERSION/" Makefile && rm Makefile.bak
    sed -i.bak "s/^pkgver=.*/pkgver=$NEW_VERSION/" PKGBUILD && rm PKGBUILD.bak
    sed -i.bak "s/^Version: .*/Version: $NEW_VERSION/" debian/control && rm debian/control.bak
    sed -i.bak "s/alias-manager_[0-9]*\.[0-9]*\.[0-9]*_amd64\.deb/alias-manager_${NEW_VERSION}_amd64.deb/g" README.md && rm README.md.bak
    sed -i.bak "s/alias-manager-[0-9]*\.[0-9]*\.[0-9]*-linux-amd64\.tar\.gz/alias-manager-${NEW_VERSION}-linux-amd64.tar.gz/g" README.md && rm README.md.bak
    sed -i.bak "s/^\.TH AM 1 \"[^\"]*\" \"[^\"]*\" \"Alias Manager Manual\"/.TH AM 1 \"$(date +"%B %Y")\" \"$NEW_VERSION\" \"Alias Manager Manual\"/" am.1 && rm am.1.bak
    echo -e "  ${GREEN}✓${NC} Version updated in all files"
fi

# Build and test
echo -e "\n${BLUE}Building and testing...${NC}"
if $DRY_RUN; then
    echo -e "  ${YELLOW}Would run:${NC} make clean && make && ./test.sh"
else
    make clean && make
    if ./test.sh > /dev/null 2>&1; then
        echo -e "  ${GREEN}✓${NC} All tests passed"
    else
        echo -e "  ${RED}✗${NC} Tests failed"
        exit 1
    fi
fi

# Commit and tag
echo -e "\n${BLUE}Creating release...${NC}"
if $DRY_RUN; then
    echo -e "  ${YELLOW}Would commit:${NC} v$NEW_VERSION: Bump version"
    echo -e "  ${YELLOW}Would tag:${NC} v$NEW_VERSION"
    echo -e "  ${YELLOW}Would push:${NC} master + tag"
else
    git add Makefile PKGBUILD debian/control README.md am.1
    git commit -m "v$NEW_VERSION: Bump version"
    git tag "v$NEW_VERSION"
    git push origin master
    git push origin "v$NEW_VERSION"
    echo -e "  ${GREEN}✓${NC} Pushed v$NEW_VERSION"
fi

if $DRY_RUN; then
    echo -e "\n${YELLOW}Dry run complete. No changes made.${NC}"
    exit 0
fi

# Wait for GitHub Actions
echo -e "\n${BLUE}Waiting for GitHub release...${NC}"
for i in {1..18}; do
    if gh release view "v$NEW_VERSION" &>/dev/null; then
        echo -e "  ${GREEN}✓${NC} Release available"
        break
    fi
    echo -n "."
    sleep 10
done
echo

# Get SHA256 and update PKGBUILD
echo -e "${BLUE}Calculating SHA256...${NC}"
TARBALL_URL="https://github.com/kazetachinuu/alias_manager/archive/v$NEW_VERSION.tar.gz"
SHA256=$(curl -sL "$TARBALL_URL" | sha256sum | cut -d' ' -f1)

if [ -z "$SHA256" ] || [ "$SHA256" == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855" ]; then
    echo -e "  ${RED}✗${NC} Failed to fetch tarball"
    echo -e "\nGet SHA256 manually: curl -sL $TARBALL_URL | sha256sum"
    exit 1
fi

echo -e "  ${GREEN}✓${NC} $SHA256"

# Update PKGBUILD with SHA256
sed -i.bak "s/^sha256sums=.*/sha256sums=('$SHA256')/" PKGBUILD && rm PKGBUILD.bak

# Generate .SRCINFO
if command -v makepkg &> /dev/null; then
    makepkg --printsrcinfo > .SRCINFO
else
    # Extract values from PKGBUILD
    PKGDESC=$(grep "^pkgdesc=" PKGBUILD | cut -d'"' -f2)
    URL=$(grep "^url=" PKGBUILD | cut -d'"' -f2)
    LICENSE=$(grep "^license=" PKGBUILD | sed "s/.*('\([^']*\)').*/\1/")

    cat > .SRCINFO << EOF
pkgbase = alias-manager
	pkgdesc = $PKGDESC
	pkgver = $NEW_VERSION
	pkgrel = 1
	url = $URL
	arch = x86_64
	arch = i686
	arch = aarch64
	arch = armv7h
	license = $LICENSE
	makedepends = gcc
	makedepends = make
	depends = glibc
	source = alias-manager-$NEW_VERSION.tar.gz::${URL}/archive/v$NEW_VERSION.tar.gz
	sha256sums = $SHA256

pkgname = alias-manager
EOF
fi

git add PKGBUILD .SRCINFO
git commit -m "Update PKGBUILD sha256sum for v$NEW_VERSION"
git push origin master
echo -e "  ${GREEN}✓${NC} PKGBUILD updated"

# Done
echo -e "\n${GREEN}=== Release v$NEW_VERSION Complete ===${NC}\n"

echo -e "${YELLOW}Next steps:${NC}\n"

echo -e "${BLUE}AUR:${NC}"
echo "  cd <aur-repo> && cp PKGBUILD .SRCINFO . && git commit -am 'v$NEW_VERSION' && git push"
echo

echo -e "${BLUE}Homebrew:${NC}"
echo "  Update Formula/alias-manager.rb:"
echo "    url \"https://github.com/kazetachinuu/alias_manager/archive/v$NEW_VERSION.tar.gz\""
echo "    sha256 \"$SHA256\""
echo

echo -e "${BLUE}Verify:${NC}"
echo "  https://github.com/kazetachinuu/alias_manager/releases/tag/v$NEW_VERSION"
