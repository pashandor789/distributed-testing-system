generate_proto:
	mkdir -p "proto"
	python3 -m grpc_tools.protoc --proto_path=../grpc/proto		\
 		--python_out=proto		 								\
 		--pyi_out=proto			 								\
 		--grpc_python_out=proto  								\
 		../grpc/proto/tabasco_grpc.proto

generate_zip_root_dir_data:
	mkdir -p "gen"
	zip gen/test_cmake.zip data/*

generate_tabasco_test: generate_proto generate_zip_root_dir_data
