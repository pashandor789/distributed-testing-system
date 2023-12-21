cmake_minimum_required(VERSION 3.21)
project(executor)

set(CMAKE_CXX_STANDARD 23)

set(DTS_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${DTS_ROOT_PATH})

add_executable(
    executor
    main.cpp
    executor.cpp
)
