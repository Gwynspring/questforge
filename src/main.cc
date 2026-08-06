#include <cstdlib>
#include <iostream>
#include <vector>

#include <CLI/App.hpp>

#include "cli/arg_parser.h"
#include "generator/test_generator.h"
#include "renderer/typst_renderer.h"
#include "repository/question_repository.h"

int main(int argc, char* argv[]) {
  questforge::cli::ArgParser parser;
  questforge::cli::CliOptions opts;

  try {
    opts = parser.Parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    return parser.Exit(e);
  }

  try {
    questforge::repository::QuestionRepository question_repository;
    std::vector<questforge::model::Question> questions =
        question_repository.LoadCatalog(opts.catalog);

    questforge::generator::TestGenerator generator;

    questforge::generator::FilterCriteria filter_criteria;

    filter_criteria.easy_count = opts.easy_count;
    filter_criteria.medium_count = opts.medium_count;
    filter_criteria.hard_count = opts.hard_count;
    filter_criteria.topics = opts.topics;
    filter_criteria.seed = opts.seed;

    std::vector<questforge::model::Question> selected_questions =
        generator.Generate(questions, filter_criteria);

    questforge::renderer::TypstRenderer renderer(opts.typst_template);

    renderer.Render(selected_questions, opts.output);

  } catch (const std::exception& e) {
    // TODO: Implement spdlog instead of cerr
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return 0;
}
