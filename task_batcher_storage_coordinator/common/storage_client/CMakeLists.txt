cmake_minimum_required(VERSION 3.21)

add_library(
    tabasco_storage_client
    storage_client.cpp
)

if (NOT TARGET tabasco_common_proto)
    set(COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../common)
    add_subdirectory(${COMMON_DIR} common)
endif() 

target_link_libraries(
    tabasco_storage_client
    tabasco_common_proto
    miniocpp
    pugixml
    curlpp
    curl
    ssl
    crypto
    z
)
