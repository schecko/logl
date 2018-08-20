# Project definition
cmake_minimum_required(VERSION 3.5)
project(LOGL)

# Source files
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

# GLFW
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory("${GLFW_DIR}")

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library(glad "${GLAD_DIR}/src/glad.c" "${GLAD_DIR}/include/glad/glad.h")
target_include_directories(glad PRIVATE "${GLAD_DIR}/include")

add_subdirectory(src/triangle)
add_subdirectory(src/shaders)