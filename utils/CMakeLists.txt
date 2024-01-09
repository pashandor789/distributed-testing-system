cmake_minimum_required(VERSION 3.21)

include_guard(GLOBAL)

project(utils)

add_library(
    dts_utils
    parse_json.cpp
)
