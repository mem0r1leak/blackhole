cmake_minimum_required(VERSION 3.20)
project(blackhole)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(blackhole src/main.cpp
        src/bit-stream.h
        src/io.h
        src/huffman.h
)

enable_testing()

add_executable(blackhole_tests
        tests/io_tests.cpp
        tests/bitstream_tests.cpp
)

target_link_libraries(blackhole_tests
        PRIVATE
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(blackhole_tests)
