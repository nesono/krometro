#!/bin/sh

export PATH=$PATH:/usr/local/bin/
echo "PATH: $PATH"
mkdir -p build && cd build && /usr/local/bin/conan install ../ && /usr/local/bin/cmake ../ && make && /usr/local/bin/ctest
