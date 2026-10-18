# Interview drills. One test executable per week, labelled "drill" and the
# week, so `ctest -L drill` runs every drill and `ctest -L week1` includes the
# week 1 drills. Drills do not use the bazaar library.

function(bazaar_add_drills name label)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE bazaar_warnings GTest::gtest_main)
    gtest_discover_tests(${name} DISCOVERY_MODE PRE_TEST PROPERTIES LABELS "drill;${label}")
endfunction()

add_subdirectory(week01)
add_subdirectory(week02)
