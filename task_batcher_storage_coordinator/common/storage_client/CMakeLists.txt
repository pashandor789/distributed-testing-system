cmake_minimum_required(VERSION 3.21)

include_guard(GLOBAL)

include_directories(/usr/local/include/mongocxx/v_noabi/)
include_directories(/usr/local/include/bsoncxx/v_noabi/)

add_library(
    tabasco_storage_client
    storage_client.cpp
    data_repr.cpp
)

set(COMMON_PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../proto)
set(DTS_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(DTS_UTILS_DIR ${DTS_ROOT_DIR}/utils)

add_subdirectory(${COMMON_PROTO_DIR} common/proto)
add_subdirectory(${DTS_UTILS_DIR} utils)

target_link_libraries(
    tabasco_storage_client
    tabasco_common_proto
    dts_utils
    mongocxx
    bsoncxx
)
