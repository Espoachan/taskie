# Maintainer: Espoachan <layoutleo000@gmail.com>
pkgname=taskie
pkgver=1.0.0
pkgrel=1
pkgdesc="A CLI based task manager for personal projects"
arch=('x86_64')
url="https://github.com/Espoachan/taskie"
license=('MIT')
depends=()
source=("main.cpp" "json.hpp")
md5sums=('SKIP' 'SKIP')

build() {
  g++ -std=c++17 -O2 -o taskie main.cpp
}

package() {
  install -Dm755 taskie "$pkgdir/usr/bin/taskie"
}
