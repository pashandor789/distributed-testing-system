cmake_minimum_required(VERSION 3.21)

include_guard(GLOBAL)

set(CMAKE_CXX_STANDARD 23)
set(DTS_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

project(utils)

add_library(
    dts_utils
    parse_json.cpp
)

target_include_directories(dts_utils PUBLIC ${DTS_ROOT_DIR})
