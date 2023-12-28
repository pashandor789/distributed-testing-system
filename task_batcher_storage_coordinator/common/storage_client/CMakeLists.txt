cmake_minimum_required(VERSION 3.21)

include_guard(GLOBAL)

include_directories(/usr/local/include/mongocxx/v_noabi/)
include_directories(/usr/local/include/bsoncxx/v_noabi/)

add_library(
    tabasco_storage_client
    storage_client.cpp
)

set(COMMON_PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../proto)
add_subdirectory(${COMMON_PROTO_DIR} common/proto)

target_link_libraries(
    tabasco_storage_client
    tabasco_common_proto
    mongocxx
    bsoncxx
)

if (TEST)
    add_subdirectory(test)
endif()
