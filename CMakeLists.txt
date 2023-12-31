cmake_minimum_required(VERSION 3.21)

project(distributed-testing-system)

add_subdirectory(executor)
add_subdirectory(task_batcher_storage_coordinator)
add_subdirectory(testing_processor)
