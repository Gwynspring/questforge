find_package(GTest CONFIG REQUIRED)
include(GoogleTest)
enable_testing()

# Uncomment each test target as you implement the corresponding source:

# add_executable(question_repository_test
#   question_repository_test.cc
# )
# target_include_directories(question_repository_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
# target_link_libraries(question_repository_test PRIVATE
#   GTest::gtest_main
#   yaml-cpp::yaml-cpp
# )
# gtest_discover_tests(question_repository_test)

# add_executable(test_generator_test
#   test_generator_test.cc
# )
# target_include_directories(test_generator_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
# target_link_libraries(test_generator_test PRIVATE
#   GTest::gtest_main
# )
# gtest_discover_tests(test_generator_test)

# add_executable(typst_renderer_test
#   typst_renderer_test.cc
# )
# target_include_directories(typst_renderer_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
# target_link_libraries(typst_renderer_test PRIVATE
#   GTest::gtest_main
#   pantor::inja
# )
# gtest_discover_tests(typst_renderer_test)
