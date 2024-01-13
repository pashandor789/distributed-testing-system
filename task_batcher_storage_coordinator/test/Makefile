generate_proto:
	mkdir -p "proto"
	python3 -m grpc_tools.protoc --proto_path=../grpc/proto		\
 		--python_out=proto		 								\
 		--pyi_out=proto			 								\
 		--grpc_python_out=proto  								\
 		../grpc/proto/tabasco_grpc.proto
