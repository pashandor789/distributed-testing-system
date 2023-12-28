find_package(Catch2 3 REQUIRED)

# include_directories()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

add_executable(
    tabasco_storage_client_test
    test.cpp
)

target_link_libraries(
    tabasco_storage_client_test
    tabasco_storage_client 
    Catch2::Catch2WithMain
)
