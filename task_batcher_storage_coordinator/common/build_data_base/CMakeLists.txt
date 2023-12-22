cmake_minimum_required(VERSION 3.21)

add_library(
    build_data_base
    build_data_base.cpp
)

if (NOT TARGET tabasco_common_proto)
    add_subdirectory(${COMMON_DIR} common)
endif() 

target_link_libraries(
    build_data_base
    tabasco_common_proto
    pqxx
    pq
)
