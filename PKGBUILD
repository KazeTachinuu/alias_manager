# Maintainer: Hugo Sibony <kazetachinuu@protonmail.com>
pkgname=alias-manager
pkgver=2.1.0
pkgrel=1
pkgdesc="A lightweight CLI tool to manage your shell aliases efficiently"
arch=('x86_64' 'i686' 'aarch64' 'armv7h')
url="https://github.com/kazetachinuu/alias_manager"
license=('MIT')
depends=('glibc')
makedepends=('gcc' 'make')
source=("$pkgname-$pkgver.tar.gz::$url/archive/v$pkgver.tar.gz")
# NOTE: Before publishing to AUR, replace 'SKIP' with the actual SHA256 checksum.
# After creating a GitHub release, run: wget $url/archive/v$pkgver.tar.gz && sha256sum v$pkgver.tar.gz
# Or use: updpkgsums (requires pacman-contrib package)
sha256sums=('SKIP')

build() {
    cd "alias_manager-$pkgver"
    make
}

package() {
    cd "alias_manager-$pkgver"
    make DESTDIR="$pkgdir" PREFIX=/usr install
    install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
