# Project definition
cmake_minimum_required(VERSION 3.5)
project(LOGL C CXX)

# Source files
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

# preprocessor defines
add_definitions(-DDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

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

# stb
set(STB_DIR "${LIB_DIR}/stb")
add_library(stb "${STB_DIR}/stb_image.h")
set_target_properties(stb PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(src/triangle)
add_subdirectory(src/shaders)
add_subdirectory(src/textures)