cmake_minimum_required(VERSION 3.10)
project(GraphAlgorithms)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# List all source files
set(SOURCES
    main.cpp
    common1.cpp
)

# Create executable
add_executable(GraphAlgorithms ${SOURCES})
