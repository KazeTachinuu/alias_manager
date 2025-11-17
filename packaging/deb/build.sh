#!/bin/bash
# Build .deb package for Debian/Ubuntu

set -e

# Get version from Makefile
VERSION=$(grep "^VERSION = " Makefile | cut -d' ' -f3)
PKGNAME="alias-manager"
BUILDDIR="build-deb/${PKGNAME}_${VERSION}"

echo "Building ${PKGNAME} ${VERSION} .deb package..."

# Clean previous builds
rm -rf build-deb
mkdir -p "${BUILDDIR}/DEBIAN"
mkdir -p "${BUILDDIR}/usr/bin"
mkdir -p "${BUILDDIR}/usr/share/man/man1"
mkdir -p "${BUILDDIR}/usr/share/doc/${PKGNAME}"

# Build the binary
make clean
make

# Copy files
cp am "${BUILDDIR}/usr/bin/"
cp am.1 "${BUILDDIR}/usr/share/man/man1/"
gzip -9 "${BUILDDIR}/usr/share/man/man1/am.1"
cp LICENSE "${BUILDDIR}/usr/share/doc/${PKGNAME}/"
cp README.md "${BUILDDIR}/usr/share/doc/${PKGNAME}/"
cp packaging/deb/control "${BUILDDIR}/DEBIAN/"

# Build package
dpkg-deb --build --root-owner-group "${BUILDDIR}"

# Move to packaging directory
mkdir -p packaging/deb
mv "${BUILDDIR}.deb" "packaging/deb/${PKGNAME}_${VERSION}_amd64.deb"

echo "✓ Package built: packaging/deb/${PKGNAME}_${VERSION}_amd64.deb"
