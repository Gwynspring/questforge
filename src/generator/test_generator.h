#ifndef QUESTFORGE_GENERATOR_TEST_GENERATOR_H_
#define QUESTFORGE_GENERATOR_TEST_GENERATOR_H_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "model/question.h"

namespace questforge::generator {

struct FilterCriteria {
  int easy_count = 0;
  int medium_count = 0;
  int hard_count = 0;
  std::optional<std::vector<std::string>> topics;
  std::optional<uint32_t> seed;
};

class TestGenerator {
 public:
  [[nodiscard]] std::vector<model::Question> Generate(
      const std::vector<model::Question>& questions,
      const FilterCriteria& filter_criteria);
};
}  // namespace questforge::generator

#endif  // QUESTFORGE_GENERATOR_TEST_GENERATOR_H_
