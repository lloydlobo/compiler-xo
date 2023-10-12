# Usage:
#   cmake -S . -B build && cmake --build build
#   ./build/lumina test.lum
#   echo $?

cmake_minimum_required(VERSION 3.20)

project(lumina)

set(CMAKE_CXX_STANDARD 20)

# Add the source files
add_executable(lumina src/main.c src/print.c)

# Include the source directory to access header files
target_include_directories(lumina PUBLIC ${CMAKE_SOURCE_DIR})

# Link any necessary libraries (none in this example)
# target_link_libraries(lumina PUBLIC some_library)
