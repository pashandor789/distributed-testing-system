cmake_minimum_required(VERSION 3.21)

project(grpc_tabasco)

set(CMAKE_CXX_STANDARD 23)

add_executable(
    grpc_tabasco
    server.cpp
    main.cppz
)

include_directories(${CMAKE_BINARY_DIR})

add_subdirectory(proto)

set(TABASCO_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)
set(COMMON_DIR ${TABASCO_ROOT_DIR}/common)
add_subdirectory(${COMMON_DIR} common)
include_directories(${TABASCO_ROOT_DIR})


target_link_libraries(
    grpc_tabasco
    tabasco_common_proto
    tabasco_grpc_proto
    build_data_base
    tabasco_storage_client
)

target_compile_options(grpc_tabasco PRIVATE -O3)
# target_compile_options(grpc_tabasco PRIVATE -fsanitize=undefined)
# target_link_options(grpc_tabasco PRIVATE -fsanitize=undefined)