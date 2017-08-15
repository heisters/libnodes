cmake_minimum_required(VERSION 3.5.1)

project(libnodes-tests)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/../src/libnodes/Node.cpp"
        )


include_directories("${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/../include")

add_executable(libnodes-tests main.cpp test_nodes.cpp "${SOURCE_FILES}")
