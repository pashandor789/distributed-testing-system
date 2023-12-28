cmake_minimum_required(VERSION 3.21)

project(http_tabasco)

set(CMAKE_CXX_STANDARD 23)

add_executable(
    http_tabasco
    load_tests_handler.cpp
    server.cpp
    main.cpp
    handler_callbacks.cpp
)

set(TABASCO_ROOT_DIR ${CMAKE_SOURCE_DIR}/..)
set(COMMON_DIR ${TABASCO_ROOT_DIR}/common)
add_subdirectory(${COMMON_DIR} common)
include_directories(${TABASCO_ROOT_DIR})

target_link_libraries(
    http_tabasco
    tabasco_common_proto
    tabasco_storage_client
    build_data_base
)
