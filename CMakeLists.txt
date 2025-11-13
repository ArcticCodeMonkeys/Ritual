cmake_minimum_required(VERSION 3.15)
project(Ritual VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add efsw library as a subdirectory
add_subdirectory(external/efsw)

# Collect all source files
set(RITUAL_SOURCES
    src/main.cpp
)

# Create the executable
add_executable(ritual ${RITUAL_SOURCES})

# Include directories
target_include_directories(ritual PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

# Link efsw library
target_link_libraries(ritual PRIVATE efsw)

# Compiler warnings
if(MSVC)
    target_compile_options(ritual PRIVATE /W4)
else()
    target_compile_options(ritual PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Set output name
set_target_properties(ritual PROPERTIES
    OUTPUT_NAME "ritual"
)

# Print build information
message(STATUS "Ritual build configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Binary output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
