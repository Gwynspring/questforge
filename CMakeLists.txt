cmake_minimum_required(VERSION 3.25)
project(questforge CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(yaml-cpp CONFIG REQUIRED)
find_package(inja CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_library(questforge_lib STATIC
  src/repository/question_repository.cc
  # Uncomment each file as you implement it:
  # src/cli/arg_parser.cc
  src/generator/test_generator.cc
  src/platform/process_runner.cc
  src/renderer/typst_renderer.cc
)

target_include_directories(questforge_lib PUBLIC src)

target_link_libraries(questforge_lib PUBLIC
  yaml-cpp::yaml-cpp
  pantor::inja
  spdlog::spdlog
)

add_executable(questforge src/main.cc)

target_link_libraries(questforge PRIVATE
  questforge_lib
  CLI11::CLI11
)

enable_testing()
add_subdirectory(tests)
