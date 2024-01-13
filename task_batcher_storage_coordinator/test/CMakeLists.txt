add_custom_target(
    generate_python_proto_tabasco ALL
    COMMAND make generate_tabasco_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
