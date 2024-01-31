cmake_minimum_required(VERSION 3.14)
project(lunar-magic-wrapper)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LM_WRAP_SOURCE_FILES "")

list(
    APPEND LM_WRAP_SOURCE_FILES
    src/lunar_magic_wrapper.cpp
)

option(LM_WRAP_BUILD_TESTS "Build Lunar Magic Wrapper tests" ON)
option(LM_WRAP_BUILD_DLL "Build Lunar Magic Wrapper as a dynamic library" ON)
option(LM_WRAP_BUILD_LIB "Build Lunar Magic Wrapper as a static library" ON)

if (LM_WRAP_BUILD_LIB)
    add_library(${PROJECT_NAME}_static STATIC ${LM_WRAP_SOURCE_FILES})
endif()

if (LM_WRAP_BUILD_DLL)
    add_library(${PROJECT_NAME} SHARED ${LM_WRAP_SOURCE_FILES})
endif()

target_include_directories(${PROJECT_NAME}_static
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.2.1
)
FetchContent_MakeAvailable(fmt)

target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)
target_link_libraries(${PROJECT_NAME}_static PUBLIC fmt::fmt)

if (LM_WRAP_BUILD_TESTS)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(
            tests
            tests/tests.cc
    )
    target_link_libraries(
            tests
            GTest::gtest_main
            ${PROJECT_NAME}_static
    )

    include(GoogleTest)
    gtest_discover_tests(tests)
endif()
