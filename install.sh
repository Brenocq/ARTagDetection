#!/bin/bash
set -e

cd build
rm -rf vcpkg.linux
git clone https://github.com/Microsoft/vcpkg.git vcpkg.linux
cd vcpkg.linux
git checkout 2020.07
./bootstrap-vcpkg.sh

./vcpkg install \
	libjpeg-turbo:x64-linux
