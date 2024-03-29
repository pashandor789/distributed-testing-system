cmake_minimum_required(VERSION 3.21)

project(http_tabasco)

set(CMAKE_CXX_STANDARD 23)

add_executable(
    http_tabasco
    server.cpp
    main.cpp
    handlers/builds_handler.cpp
    handlers/build_handler.cpp
    handlers/upload_tests_handler.cpp
    handlers/upload_task_root_dir.cpp
    handlers/upload_build_handler.cpp
)

set(DTS_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(TABASCO_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(COMMON_DIR ${TABASCO_ROOT_DIR}/common)
set(UTILS_DIR ${DTS_ROOT_DIR}/utils)

add_subdirectory(${COMMON_DIR} common)
add_subdirectory(${UTILS_DIR} utils)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${TABASCO_ROOT_DIR})
include_directories(${DTS_ROOT_DIR})

target_link_libraries(
    http_tabasco
    tabasco_common_proto
    tabasco_storage_client
    dts_utils
)
