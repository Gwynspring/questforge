cmake_minimum_required(VERSION 3.25)
project(questforge CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies (installed via vcpkg)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(inja CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(questforge
  src/main.cc
  # Uncomment each file as you implement it:
  # src/cli/arg_parser.cc
  # src/model/question.cc
  # src/repository/question_repository.cc
  # src/generator/test_generator.cc
  # src/render/typst_renderer.cc
)

target_include_directories(questforge PRIVATE src)

target_link_libraries(questforge PRIVATE
  yaml-cpp::yaml-cpp
  pantor::inja
  CLI11::CLI11
  spdlog::spdlog
)

add_subdirectory(tests)
