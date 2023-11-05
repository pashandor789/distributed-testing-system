cmake_minimum_required(VERSION 3.21)
project(testing_processor)

find_package(Protobuf REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(
    testing_processor
    docker_container.h
    docker_container.cpp
)

add_subdirectory(proto)

target_link_libraries(
    testing_processor
    testing_processor_proto
)
