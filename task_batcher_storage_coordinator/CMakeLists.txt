cmake_minimum_required(VERSION 3.21)

project(task-batcher-storage-coordinator)

add_subdirectory(common)
add_subdirectory(grpc)
add_subdirectory(http)

if (DTS_TEST)
    add_subdirectory(test)
endif ()
