cmake_minimum_required(VERSION 3.3)
project(krometro)

set(CMAKE_CXX_STANDARD 11)

include(conanbuildinfo.cmake)
conan_basic_setup()

set(SOURCE_FILES main.cpp krometro/v1/krometro.h)
add_executable(krometro ${SOURCE_FILES})

include(FindGTest)
enable_testing()
include_directories(${CONAN_INCLUDE_DIRS})
add_executable(statistics_ut statistics_ut.cpp)
target_link_libraries(statistics_ut ${CONAN_LIBS})
GTEST_ADD_TESTS(statistics_ut "" statistics_ut.cpp)