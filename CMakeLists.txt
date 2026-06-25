cmake_minimum_required(VERSION 3.10)
project(OrbitusEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Add glad.c to our list of source files to compile
set(SOURCES
    src/main.cpp
    src/glad.c
)

add_executable(Orbitus ${SOURCES})

# 2. Tell CMake where to find our new "include" folder
target_include_directories(Orbitus PRIVATE include)

find_package(OpenGL REQUIRED)
target_link_libraries(Orbitus PRIVATE OpenGL::GL)

find_package(glfw3 REQUIRED)
target_link_libraries(Orbitus PRIVATE glfw)