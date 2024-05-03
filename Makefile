prepare:
	mkdir -p shared-libs
	cat .github/workflows/shared-libs.txt | xargs -I {} cp {} shared-libs
	mkdir -p build

build: prepare
	cmake -S . -B ./build -DDTS_TEST=ON
	cmake --build ./build -- -j $(nproc)

build_services: build
	docker-compose build

launch_services:
	docker-compose up -d storage broker http_tabasco grpc_tabasco testing_processor userver

test:
	docker-compose run tabasco_test

build_launch_services: build_services launch_services

build_launch_test_services: build_services launch_services test
