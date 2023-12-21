cmake_minimum_required(VERSION 3.21)
project(testing_processor)

set(CMAKE_CXX_STANDARD 23)

set(DTS_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${DTS_ROOT_PATH})

add_executable(
    testing_processor
    brocker_client.cpp
    docker_container.cpp
    testing_processor.cpp
)

add_subdirectory(proto)

target_link_libraries(
    testing_processor
    testing_processor_proto
)
