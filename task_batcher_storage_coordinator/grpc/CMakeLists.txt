cmake_minimum_required(VERSION 3.21)

project(grpc_tabasco)

set(CMAKE_CXX_STANDARD 23)

add_executable(
    grpc_tabasco
    main.cpp
)

add_subdirectory(proto)

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/../common)
add_subdirectory(${COMMON_DIR} common)
include_directories(${CMAKE_BINARY_DIR})

target_link_libraries(
    grpc_tabasco
    tabasco_common_proto
    tabasco_grpc_proto
)
