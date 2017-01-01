cmake_minimum_required(VERSION 3.3)
project(krometro)

set(CMAKE_CXX_STANDARD 11)

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(include)

set(SOURCE_FILES
        include/krometro/v1/krometro.hpp
        include/krometro/v1/krometro_main.hpp
        )

include(FindGTest)
enable_testing()

add_executable(statistics_ut statistics_ut.cpp)
target_link_libraries(statistics_ut ${CONAN_LIBS})
GTEST_ADD_TESTS(statistics_ut "" statistics_ut.cpp)
