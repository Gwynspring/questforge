#include "cli/arg_parser.h"

#include <gtest/gtest.h>

#include <string>
#include <vector>

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
      ParseArgs({"questforge", "generate", "--catalog", "x", "--out", "y"});

  EXPECT_FALSE(opts.seed.has_value());
  EXPECT_FALSE(opts.topics.has_value());

  // TODO: Templatepath has to be changed when CliOptions struct gets refactored
  EXPECT_EQ(opts.typst_template, "templates/test.typ.jinja");
}
