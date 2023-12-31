find_package(Catch2 3 REQUIRED)

enable_testing()

add_executable(
    build_data_base_test
    test.cpp
)

target_link_libraries(
    build_data_base_test
    build_data_base
    Catch2::Catch2WithMain
)

add_test(
    NAME build_data_base_test
    COMMAND build_data_base_test
)