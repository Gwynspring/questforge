#ifndef QUESTFORGE_GENERATOR_TEST_GENERATOR_H_
#define QUESTFORGE_GENERATOR_TEST_GENERATOR_H_

#include <optional>
#include <random>
#include <string>
#include <vector>

#include "model/question.h"

namespace questforge::generator {

struct FilterCriteria {
  std::optional<std::vector<std::string>> topics;
  std::optional<model::Difficulty> difficulty;
  int max_points;
  std::mt19937::result_type seed;
};

class TestGenerator {
 public:
  [[nodiscard]] std::vector<model::Question> Generate(
      const std::vector<model::Question>& questions,
      const FilterCriteria& filter_criteria);
};
}  // namespace questforge::generator

#endif  // QUESTFORGE_GENERATOR_TEST_GENERATOR_H_
