cmake_minimum_required(VERSION 3.25)
project(questforge CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# Standalone tools/tests of fetched dependencies are irrelevant to us and
# only cost build time, so they're disabled before declaring each one.
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG        yaml-cpp-0.9.0
  GIT_SHALLOW    TRUE
)

set(INJA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  inja
  GIT_REPOSITORY https://github.com/pantor/inja.git
  GIT_TAG        v3.5.0
  GIT_SHALLOW    TRUE
)

FetchContent_Declare(
  CLI11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG        v2.7.2
  GIT_SHALLOW    TRUE
)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.17.0
  GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(yaml-cpp inja CLI11 spdlog)

add_library(questforge_lib STATIC
  src/cli/arg_parser.cc
  src/repository/question_repository.cc
  src/generator/test_generator.cc
  src/platform/process_runner.cc
  src/renderer/typst_renderer.cc
)

target_include_directories(questforge_lib PUBLIC src)

target_link_libraries(questforge_lib PUBLIC
  yaml-cpp::yaml-cpp
  pantor::inja
  spdlog::spdlog
  CLI11::CLI11
)

add_executable(questforge src/main.cc)

target_link_libraries(questforge PRIVATE
  questforge_lib
  CLI11::CLI11
)

enable_testing()
add_subdirectory(tests)
