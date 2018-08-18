# Project definition
cmake_minimum_required(VERSION 3.5)
project(triangle)

# Source files
set(SOURCES main.cpp)

# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES})
set_property(   TARGET ${PROJECT_NAME} 
                PROPERTY CXX_STANDARD 11 )

# glfw
target_link_libraries(${PROJECT_NAME} glfw "${GLFW_LIBRARIES}")

# glad
target_link_libraries(${PROJECT_NAME} glad "${CMAKE_DL_LIBS}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")