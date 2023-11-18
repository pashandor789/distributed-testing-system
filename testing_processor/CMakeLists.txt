cmake_minimum_required(VERSION 3.21)
project(testing_processor)

set(CMAKE_CXX_STANDARD 23)

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
