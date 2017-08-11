cmake_minimum_required(VERSION 3.8)
project(libnodes)
add_subdirectory(test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_FILES
  src/libnodes/Node.cpp
)

include_directories(
  "include"
)
