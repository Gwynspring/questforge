#include "cli/arg_parser.h"

#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <gtest/gtest.h>

questforge::cli::CliOptions ParseArgs(std::vector<std::string> args) {
  std::vector<char*> argv;
  for (auto& arg : args) {
    argv.push_back(arg.data());
  }

  questforge::cli::ArgParser parser;

  return parser.Parse(static_cast<int>(argv.size()), argv.data());
}

TEST(ArgParserTest, ParsesAllFieldsSuccessfully) {
  questforge::cli::CliOptions opts = ParseArgs(
      {"questforge", "generate", "--catalog", "cat.yaml", "--out", "t.pdf",
       "--easy", "2", "--medium", "2", "--seed", "42", "--topics", "a,b"});

  EXPECT_EQ(opts.catalog, "cat.yaml");
  EXPECT_EQ(opts.easy_count, 2);
  EXPECT_EQ(opts.medium_count, 2);
  EXPECT_EQ(opts.hard_count, 0);
  ASSERT_TRUE(opts.seed.has_value());
  EXPECT_EQ(opts.seed.value(), 42u);
  ASSERT_EQ(opts.topics->size(), 2u);
  EXPECT_EQ(opts.topics->at(0), "a");
  EXPECT_EQ(opts.topics->at(1), "b");
  EXPECT_EQ(opts.output, "t.pdf");
}

TEST(ArgParserTest, ThrowErrorForMissingCatalogOpt) {
  EXPECT_THROW(ParseArgs({"questforge", "generate", "--out", "t.pdf"}),
               CLI::ParseError);
}

TEST(ArgParserTest, DefaultOptsSetCorrectly) {
  questforge::cli::CliOptions opts =
      ParseArgs({"questforge", "generate", "--catalog", "x", "--out", "y.pdf"});

  EXPECT_FALSE(opts.seed.has_value());
  EXPECT_FALSE(opts.topics.has_value());

  // TODO: Templatepath has to be changed when CliOptions struct gets refactored
  EXPECT_EQ(opts.typst_template, "templates/test.typ.jinja");
}

TEST(ArgParserTest, ThrowErrorForNegativeNumber) {
  EXPECT_THROW(ParseArgs({"questforge", "generate", "--catalog", "cat.yaml",
                          "--out", "t.pdf", "--easy", "-2", "--medium", "2",
                          "--seed", "42", "--topics", "a,b"}),
               CLI::ParseError);
}

TEST(ArgParserTest, ParsesDateAndConfigSuccessfully) {
  questforge::cli::CliOptions opts = ParseArgs(
      {"questforge", "generate", "--catalog", "cat.yaml", "--out", "t.pdf",
       "--date", "15.08.2026", "--config", "data/school/test.yaml"});

  EXPECT_EQ(opts.date, "15.08.2026");
  EXPECT_EQ(opts.config, "data/school/test.yaml");
}

TEST(ArgParserTest, ParsesDateWithoutConfigSuccessfully) {
  questforge::cli::CliOptions opts =
      ParseArgs({"questforge", "generate", "--catalog", "cat.yaml", "--out",
                 "t.pdf", "--date", "15.08.2026"});

  EXPECT_EQ(opts.date, "15.08.2026");
  EXPECT_EQ(opts.config, std::nullopt);
}

TEST(ArgParserTest, ParsesConfigWithoutDateSuccessfully) {
  questforge::cli::CliOptions opts =
      ParseArgs({"questforge", "generate", "--catalog", "cat.yaml", "--out",
                 "t.pdf", "--config", "data/school/test.yaml"});

  EXPECT_EQ(opts.date, std::nullopt);
  EXPECT_EQ(opts.config, "data/school/test.yaml");
}

TEST(ArgParserTest, ParsesSolutionsSuccessfully) {
  questforge::cli::CliOptions opts =
      ParseArgs({"questforge", "generate", "--catalog", "cat.yaml", "--out",
                 "t.pdf", "--solutions", "solutions.pdf"});
  EXPECT_EQ(opts.solutions, "solutions.pdf");
  EXPECT_EQ(opts.solutions_template, "templates/solutions.typ.jinja");
}

TEST(ArgParserTest, ThrowErrorForWrongSolutionsFileFormat) {
  EXPECT_THROW(ParseArgs({"questforge", "generate", "--catalog", "cat.yaml",
                          "--out", "t.pdf", "--solutions", "solutions.txt"}),
               CLI::ValidationError);
}
