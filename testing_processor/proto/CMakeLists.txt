set(Protobuf_PROTOC_EXECUTABLE /usr/bin/protoc)
set(PROTO_FILES docker_container.proto)
set(PROTOBUF_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR})

find_package(Protobuf REQUIRED)

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