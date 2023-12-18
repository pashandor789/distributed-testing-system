find_package(Protobuf REQUIRED)

set(Protobuf_PROTOC_EXECUTABLE /usr/bin/protoc)
set(
    PROTO_FILES 
    tabasco_grpc.proto
)

set(PROTOBUF_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_library(
    tabasco_grpc_proto
    ${PROTO_SRCS} 
    ${PROTO_HDRS}
)

target_link_libraries(
    tabasco_grpc_proto
    protobuf::libprotobuf
)