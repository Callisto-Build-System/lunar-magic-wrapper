cmake_minimum_required(VERSION 3.14)
project(lunar-magic-wrapper)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME}
        lunar_magic_wrapper.cpp
        lunar_magic_wrapper.h)

FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.2.1
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)

enable_testing()

add_executable(
        tests
        tests/tests.cc
)
target_link_libraries(
        tests
        GTest::gtest_main
        ${PROJECT_NAME}
)
target_include_directories(
        tests PUBLIC
        ${PROJECT_SOURCE_DIR}
)

include(GoogleTest)
gtest_discover_tests(tests)
