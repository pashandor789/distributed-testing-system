cmake_minimum_required(VERSION 3.21)

include_directories(/usr/local/include/mongocxx/v_noabi/)
include_directories(/usr/local/include/bsoncxx/v_noabi/)

add_library(
    tabasco_storage_client
    storage_client.cpp
)

if (NOT TARGET tabasco_common_proto)
    add_subdirectory(${COMMON_DIR} common)
endif() 

target_link_libraries(
    tabasco_storage_client
    tabasco_common_proto
    mongocxx
    bsoncxx
)
