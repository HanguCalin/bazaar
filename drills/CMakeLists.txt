# Interview drills. One test executable per week, labelled "<week>-drill"
# (e.g. week1-drill). `ctest -L` takes a regex, so `ctest -L drill` runs every
# drill and `ctest -L week1` includes the week 1 drills. A single label avoids
# passing a ;-list through gtest_discover_tests, which older CMake versions
# split apart. Drills do not use the bazaar library.

function(bazaar_add_drills name label)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE bazaar_warnings GTest::gtest_main)
    gtest_discover_tests(${name} DISCOVERY_MODE PRE_TEST PROPERTIES LABELS ${label}-drill)
endfunction()

add_subdirectory(week01)
add_subdirectory(week02)
