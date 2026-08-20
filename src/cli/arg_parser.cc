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
      app_.add_subcommand("generate", "generate a test, optionally with a solutions sheet");
  gen->add_option("-c,--catalog", opts.catalog, "path to the question catalog")
      ->required();
  gen->add_option("--template", opts.typst_template,
                  "path to the Typst template (default: templates/test.typ.jinja)");
  gen->add_option("-o,--out", opts.output,
                  "output path for the generated PDF file")
      ->required()
      ->check(
          [](const std::string& value) { return CheckPath(value, "Output"); });
  gen->add_option("--easy", opts.easy_count,
                  "number of requested easy questions")
      ->check(CLI::NonNegativeNumber, "number of requested easy questions");
  gen->add_option("--medium", opts.medium_count,
                  "number of requested medium questions")
      ->check(CLI::NonNegativeNumber, "number of requested medium questions");
  gen->add_option("--hard", opts.hard_count, "number of requested hard questions")
      ->check(CLI::NonNegativeNumber, "number of requested hard questions");
  gen->add_option("-s,--seed", opts.seed,
                  "random seed for reproducible question selection");
  gen->add_option("--topics", opts.topics,
                  "comma-separated list of topics to restrict selection to")
      ->delimiter(',');
  gen->add_option("--date", opts.date,
                  "date printed on the PDF ('auto' = today)");
  gen->add_option("--config", opts.config,
                  "path to the school header configuration (YAML)");
  gen->add_option("--solutions", opts.solutions,
                  "output path for the generated solutions PDF")
      ->check([](const std::string& value) {
        return CheckPath(value, "Solutions");
      });

  app_.require_subcommand(1);
  app_.footer("Run 'questforge generate --help' for detailed options");
  app_.parse(argc, argv);
  return opts;
}

int ArgParser::Exit(const CLI::ParseError& e) { return app_.exit(e); }

}  // namespace questforge::cli
