cmake_minimum_required(VERSION 3.21)

include_guard(GLOBAL)

add_library(
    tabasco_storage_client
    storage_client.cpp
    data_repr.cpp
)

add_subdirectory(impl)

target_link_libraries(
    tabasco_storage_client
    tabasco_storage_client_impl
)
