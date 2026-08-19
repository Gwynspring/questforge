#include "cli/arg_parser.h"

#include <format>
#include <string>

namespace questforge::cli {

namespace {
std::string CheckPath(const std::string& path, const std::string& file) {
  const std::filesystem::path p = path;
  if (p.extension().string() == ".pdf") {
    return static_cast<std::string>("");
  } else {
    return std::format("{} file must be '.pdf' not {}", file,
                       p.extension().string());
  }
}

}  // namespace

CliOptions ArgParser::Parse(int argc, char* argv[]) {
  CliOptions opts;

  CLI::App* gen =
      app_.add_subcommand("generate", "generate a test without solutions");
  gen->add_option("-c,--catalog", opts.catalog)->required();
  gen->add_option("--template", opts.typst_template);
  gen->add_option("-o,--out", opts.output)
      ->required()
      ->check(
          [](const std::string& value) { return CheckPath(value, "Output"); });
  gen->add_option("--easy", opts.easy_count)->check(CLI::NonNegativeNumber);
  gen->add_option("--medium", opts.medium_count)->check(CLI::NonNegativeNumber);
  gen->add_option("--hard", opts.hard_count)->check(CLI::NonNegativeNumber);
  gen->add_option("-s,--seed", opts.seed);
  gen->add_option("--topics", opts.topics)->delimiter(',');
  gen->add_option("--date", opts.date);
  gen->add_option("--config", opts.config);
  gen->add_option("--solutions", opts.solutions)
      ->check([](const std::string& value) {
        return CheckPath(value, "Solutions");
      });

  app_.require_subcommand(1);
  app_.parse(argc, argv);
  return opts;
}

int ArgParser::Exit(const CLI::ParseError& e) { return app_.exit(e); }

}  // namespace questforge::cli
