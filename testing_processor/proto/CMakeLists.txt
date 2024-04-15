find_package(Protobuf 3.17.3 REQUIRED)

set(Protobuf_PROTOC_EXECUTABLE /usr/bin/protoc)
set(
    PROTO_FILES 
    broker_client.proto
    docker_container.proto
    testing_processor_config.proto
)

set(PROTOBUF_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_library(
    testing_processor_proto
    ${PROTO_SRCS} 
    ${PROTO_HDRS}
)

target_link_libraries(
    testing_processor_proto
    protobuf::libprotobuf
)

target_include_directories( 
    testing_processor_proto
    PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..
)
