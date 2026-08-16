#ifndef QUESTFORGE_CLI_ARG_PARSER_H_
#define QUESTFORGE_CLI_ARG_PARSER_H_

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>

namespace questforge::cli {

// TODO: Change typst_template path with cmake install path
// (CMAKE_INSTALL_PREFIX/share-directory) before deploying the software
struct CliOptions {
  std::filesystem::path catalog;
  std::filesystem::path typst_template = "templates/test.typ.jinja";
  std::filesystem::path output;
  std::optional<std::filesystem::path> config;
  std::optional<std::string> date;
  int easy_count = 0;
  int medium_count = 0;
  int hard_count = 0;
  std::optional<uint32_t> seed;
  std::optional<std::vector<std::string>> topics;
};

class ArgParser {
 public:
  CliOptions Parse(int argc, char* argv[]);
  int Exit(const CLI::ParseError& e);

 private:
  CLI::App app_{"questforge"};
};

}  // namespace questforge::cli

#endif  // QUESTFORGE_CLI_ARG_PARSER_H_
