cmake_minimum_required(VERSION 3.21)

include_guard(GLOBAL)

add_library(
    build_data_base
    build_data_base.cpp
)

set(COMMON_PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../proto)
add_subdirectory(${COMMON_PROTO_DIR} common/proto)

target_link_libraries(
    build_data_base
    tabasco_common_proto
    pqxx
    pq
)
