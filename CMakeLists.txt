project(lunar-magic-wrapper)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME}
        lunar_magic_wrapper.cpp
        lunar_magic_wrapper.h)
