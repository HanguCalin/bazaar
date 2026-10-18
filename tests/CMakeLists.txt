# One test executable per module. Each is labelled with the week it belongs to,
# so `ctest -L week1` runs only that week's tests.

function(bazaar_add_test name label)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name}
        PRIVATE bazaar bazaar_warnings nlohmann_json::nlohmann_json GTest::gtest_main
    )
    target_compile_definitions(${name} PRIVATE BAZAAR_DATA_DIR="${PROJECT_SOURCE_DIR}/data")
    gtest_discover_tests(${name} DISCOVERY_MODE PRE_TEST PROPERTIES LABELS ${label})
endfunction()

bazaar_add_test(test_model week1)
bazaar_add_test(test_json_io week1)
bazaar_add_test(test_catalog week2)
bazaar_add_test(test_browse_engine week2)
bazaar_add_test(test_end_to_end week2)

# The command line tool must run on the sample data and exit with status 0.
add_test(
    NAME BrowseCli.RunsOnSampleData
    COMMAND browse_cli
        ${PROJECT_SOURCE_DIR}/data/categories.json
        ${PROJECT_SOURCE_DIR}/data/products.json
        ${PROJECT_SOURCE_DIR}/data/requests.jsonl
        ${CMAKE_CURRENT_BINARY_DIR}/results.json
)
set_tests_properties(BrowseCli.RunsOnSampleData PROPERTIES LABELS week2)
