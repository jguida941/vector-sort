cmake_minimum_required(VERSION 3.14)
project(Vector_Sort)

set(CMAKE_CXX_STANDARD 14)

# Source files from src/ directory
add_executable(Vector_Sort
    src/VectorSorting.cpp
    src/CSVparser.cpp
)

# Include the src directory for headers
target_include_directories(Vector_Sort PRIVATE ${CMAKE_SOURCE_DIR}/src)
