build:
	mkdir -p builds
	cd builds
	cmake --build . -- -j $(nproc)
	mkdir -p shared-libs
	cat .github/workflows/shared-libs.txt | xargs -I {} cp {} shared-libs

build_services: build
	docker-compose build

launch_services:
	docker-compose up -d build_data_base storage http_tabasco grpc_tabasco

test:
	docker-compose run tabasco_test

launch_build_services: build_services launch_services

launch_build_test_services: build_services launch_services test
