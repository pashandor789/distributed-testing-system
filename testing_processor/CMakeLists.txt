cmake_minimum_required(VERSION 3.21)
project(testing_processor)

set(CMAKE_CXX_STANDARD 23)

include_directories(${CMAKE_BINARY_DIR})

set(DTS_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(GRPC_TABASCO_PATH ${DTS_ROOT_PATH}/task_batcher_storage_coordinator/grpc/proto)
include_directories(${DTS_ROOT_PATH})

add_executable(
    testing_processor
    broker_client.cpp
    docker_container.cpp
    tabasco_request_task.cpp
    testing_processor.cpp
    main.cpp
)

add_subdirectory(proto)
add_subdirectory(${GRPC_TABASCO_PATH} task_batcher_storage_coordinator/grpc/proto)

target_link_libraries(
    testing_processor
    testing_processor_proto
    tabasco_grpc_proto
    amqpcpp
    ev
)
