include(CTest)

find_package(GTest REQUIRED)

add_executable(test_structural_tuple test_structural_tuple.cpp)
target_link_libraries(test_structural_tuple PRIVATE structural_tuple
                                                    GTest::GTest GTest::Main)
gtest_discover_tests(test_structural_tuple)
