include(FetchContent)

# Matches the dynamic MSVC runtime (/MD) questforge_lib links against by
# default, so gtest doesn't cause a runtime-library mismatch on Windows.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# Uncomment each test target as you implement the corresponding source:

add_executable(arg_parser_test
  arg_parser_test.cc
)
target_include_directories(arg_parser_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(arg_parser_test PRIVATE
  GTest::gtest_main
  questforge_lib
)
gtest_discover_tests(arg_parser_test)

add_executable(question_repository_test
  question_repository_test.cc
)
target_include_directories(question_repository_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(question_repository_test PRIVATE
  GTest::gmock
  GTest::gtest_main
  questforge_lib
)
target_compile_definitions(question_repository_test PRIVATE
  PROJECT_ROOT="${PROJECT_SOURCE_DIR}"
)
gtest_discover_tests(question_repository_test)

add_executable(test_generator_test
  test_generator_test.cc
)
target_include_directories(test_generator_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(test_generator_test PRIVATE
  GTest::gtest_main
  questforge_lib
)
gtest_discover_tests(test_generator_test)

add_executable(typst_renderer_test
  typst_renderer_test.cc
)
target_include_directories(typst_renderer_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(typst_renderer_test PRIVATE
  GTest::gmock
  GTest::gtest_main
  questforge_lib
)
gtest_discover_tests(typst_renderer_test)

target_compile_definitions(typst_renderer_test PRIVATE
  PROJECT_ROOT="${PROJECT_SOURCE_DIR}"
)

add_executable(date_test
  date_test.cc
)
target_include_directories(date_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(date_test PRIVATE
  GTest::gtest_main
  questforge_lib
)
gtest_discover_tests(date_test)

add_executable(windows_command_line_test
  windows_command_line_test.cc
)
target_include_directories(windows_command_line_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(windows_command_line_test PRIVATE
  GTest::gtest_main
  questforge_lib
)
gtest_discover_tests(windows_command_line_test)
