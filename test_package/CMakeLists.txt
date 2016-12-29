cmake_minimum_required(VERSION 3.3)
project(test_krometro_conan)

set(CMAKE_CXX_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(${CONAN_INCLUDE_DIRS})
set(SOURCE_FILES
        main.cpp
        )
add_executable(krometro ${SOURCE_FILES})

