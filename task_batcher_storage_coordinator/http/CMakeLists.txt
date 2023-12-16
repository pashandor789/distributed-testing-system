cmake_minimum_required(VERSION 3.21)

project(http_tabasco)

set(CMAKE_CXX_STANDARD 23)

add_executable(
    http_tabasco
    load_tests_handler.cpp
    server.cpp
    main.cpp
)

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/../common)
add_subdirectory(${COMMON_DIR} common)
include_directories(${CMAKE_BINARY_DIR})

target_link_libraries(
    http_tabasco
    tabasco_common_proto
    miniocpp
    pugixml
    curlpp
    curl
    ssl
    crypto
    z
)
