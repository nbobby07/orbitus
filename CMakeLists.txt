cmake_minimum_required(VERSION 3.10)
project(OrbitusEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
)

add_executable(Orbitus ${SOURCES})

find_package(OpenGL REQUIRED)
target_link_libraries(Orbitus PRIVATE OpenGL::GL)

find_package(glfw3 REQUIRED)
target_link_libraries(Orbitus PRIVATE glfw)

include paths
