#!/bin/bash

# simple script to run cmake for both static and shared versions of libhelix
# be sure to run from same dir as this script

build_dir() {
    if [ -d build ]; then
        rm -rf build
    fi

    mkdir build && cd build
}

build_dir

cmake -DBUILD_SHARED_LIBS=OFF ..
make
make install

cd ..

build_dir

cmake ..
make
make install

cd ..
rm -rf build
