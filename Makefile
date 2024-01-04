build:
	mkdir -p builds
	cd builds
	cmake --build . -- -j $(nproc)
	mkdir -p shared-libs
	cat .github/workflows/shared-libs.txt | xargs -I {} cp {} shared-libs

build_services: build
	docker-compose build

launch_services:
	docker-compose up -d build_data_base storage brocker http_tabasco grpc_tabasco testing_processor

test:
	docker-compose run tabasco_test

build_launch_services: build_services launch_services

build_launch_test_services: build_services launch_services test
