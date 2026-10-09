cmake_minimum_required(VERSION 3.10)
project(zanni-protocol VERSION 0.1.0 LANGUAGES C)

option(ZANNI_VERSION_IN_OUTPUT_NAME "Append version to output library filename" OFF)

# Generate version header from template
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/zanni_version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/zanni_version.h
    @ONLY
)

# Define the static library
add_library(zanni-protocol STATIC
    src/zanni_protocol.c
)

# Specify include directories
target_include_directories(zanni-protocol PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Set library output name if versioned filename option is enabled
if(ZANNI_VERSION_IN_OUTPUT_NAME)
    set_target_properties(zanni-protocol PROPERTIES 
        OUTPUT_NAME "zanni-protocol-${PROJECT_VERSION}"
    )
endif()
