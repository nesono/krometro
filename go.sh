#!/bin/sh

export PATH=$PATH:/usr/local/bin/
mkdir -p build && cd build && conan install ../ && cmake ../ && make && ctest
