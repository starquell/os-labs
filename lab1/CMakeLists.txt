cmake_minimum_required(VERSION 3.9)
project(lab1)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty)

find_package(Threads REQUIRED)

set(SRC_LIST main.cpp
             src/Cancelator.cpp)

add_executable(executor ${SRC_LIST})
target_link_libraries(executor Threads::Threads)