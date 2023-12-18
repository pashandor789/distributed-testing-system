# Find gRPC and protobuf packages
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

set(PROTO_FILES 
    tabasco_grpc.proto
)

set(PROTOBUF_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Generate C++ code from your .proto files
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_library(
    tabasco_grpc_proto
    ${PROTO_SRCS} 
    ${PROTO_HDRS}
    ${PROTO_FILES}
)

target_link_libraries(
    tabasco_grpc_proto
    protobuf::libprotobuf
    gRPC::grpc
    gRPC::grpc++
)


get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET tabasco_grpc_proto LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")
