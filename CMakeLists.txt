cmake_minimum_required(VERSION 3.10)
project(zanni-protocol C)

# Define the static library
add_library(zanni-protocol STATIC
    src/zanni_protocol.c
)

# Specify the include directories for the target
target_include_directories(zanni-protocol PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
