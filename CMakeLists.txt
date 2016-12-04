cmake_minimum_required(VERSION 3.6)
project(krometro)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp krometro/v1/krometro.h)
add_executable(krometro ${SOURCE_FILES})