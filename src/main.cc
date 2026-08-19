#include <cstdlib>
#include <format>
#include <iostream>
#include <vector>

#include <CLI/App.hpp>

#include "cli/arg_parser.h"
#include "generator/test_generator.h"
#include "model/header.h"
#include "renderer/typst_renderer.h"
#include "repository/header_repository.h"
#include "repository/question_repository.h"
#include "util/date.h"

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

    questforge::model::Header header;
    if (opts.config.has_value()) {
      header = questforge::repository::LoadHeader(opts.config->string());
    }

    if (opts.date) {
      header.date =
          (*opts.date == "auto") ? questforge::util::FormatToday() : *opts.date;
    }

    questforge::generator::TestGenerator generator;

    questforge::generator::FilterCriteria filter_criteria;

    filter_criteria.easy_count = opts.easy_count;
    filter_criteria.medium_count = opts.medium_count;
    filter_criteria.hard_count = opts.hard_count;
    filter_criteria.topics = opts.topics;
    filter_criteria.seed = opts.seed;

    std::vector<questforge::model::Question> selected_questions =
        generator.Generate(questions, filter_criteria);

    if (opts.solutions) {
      for (const auto& entry : selected_questions) {
        if (!entry.solution.has_value()) {
          throw std::runtime_error(std::format(
              "Question with Id {} does not have a solution!", entry.id));
        }
      }
      questforge::renderer::TypstRenderer renderer_solution(
          opts.solutions_template);
      renderer_solution.Render(selected_questions, header, *opts.solutions);
    }

    questforge::renderer::TypstRenderer renderer(opts.typst_template);

    renderer.Render(selected_questions, header, opts.output);

  } catch (const std::exception& e) {
    // TODO: Implement spdlog instead of cerr
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return 0;
}
