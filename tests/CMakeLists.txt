find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

# Uncomment each test target as you implement the corresponding source:

add_executable(question_repository_test
  question_repository_test.cc
)
target_include_directories(question_repository_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(question_repository_test PRIVATE
  GTest::gtest_main
  questforge_lib
)
gtest_discover_tests(question_repository_test
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_executable(test_generator_test
  test_generator_test.cc
)
target_include_directories(test_generator_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(test_generator_test PRIVATE
  GTest::gtest_main
  questforge_lib
)
gtest_discover_tests(test_generator_test
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_executable(typst_renderer_test
  typst_renderer_test.cc
)
target_include_directories(typst_renderer_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(typst_renderer_test PRIVATE
  GTest::gtest_main
  questforge_lib
)
gtest_discover_tests(typst_renderer_test
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

target_compile_definitions(typst_renderer_test PRIVATE
  PROJECT_ROOT="${PROJECT_SOURCE_DIR}"
)
