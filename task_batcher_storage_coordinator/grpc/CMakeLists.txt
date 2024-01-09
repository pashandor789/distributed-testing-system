cmake_minimum_required(VERSION 3.21)

project(grpc_tabasco)

set(CMAKE_CXX_STANDARD 23)

add_executable(
    grpc_tabasco
    server.cpp
    main.cpp
)

add_subdirectory(proto)

set(TABASCO_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(COMMON_DIR ${TABASCO_ROOT_DIR}/common)
add_subdirectory(${COMMON_DIR} common)
include_directories(${TABASCO_ROOT_DIR})


target_link_libraries(
    grpc_tabasco
    tabasco_common_proto
    tabasco_grpc_proto
    tabasco_storage_client
)
