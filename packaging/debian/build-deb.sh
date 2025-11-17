#!/bin/bash
# Build .deb package for Debian/Ubuntu

set -e

VERSION="2.1.0"
PKGNAME="alias-manager"
BUILDDIR="deb-build/${PKGNAME}_${VERSION}"

echo "Building ${PKGNAME} ${VERSION} .deb package..."

# Clean previous builds
rm -rf deb-build
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
cp packaging/debian/control "${BUILDDIR}/DEBIAN/"

# Build package
dpkg-deb --build --root-owner-group "${BUILDDIR}"

# Move to packaging directory
mv "${BUILDDIR}.deb" "packaging/debian/${PKGNAME}_${VERSION}_amd64.deb"

echo "✓ Package built: packaging/debian/${PKGNAME}_${VERSION}_amd64.deb"
