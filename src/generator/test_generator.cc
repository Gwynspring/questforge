#include "generator/test_generator.h"

#include <algorithm>

namespace questforge::generator {

[[nodiscard]] std::vector<model::Question> TestGenerator::Generate(
    const std::vector<model::Question>& questions,
    const FilterCriteria& filter_criteria) {
  std::vector<model::Question> result;

  for (const auto& entry : questions) {
    if (filter_criteria.difficulty.has_value()) {
      if (entry.difficulty != filter_criteria.difficulty.value()) {
        continue;
      }
    }
    if (filter_criteria.topics.has_value()) {
      if (std::find(filter_criteria.topics->begin(),
                    filter_criteria.topics->end(),
                    entry.topic) == filter_criteria.topics->end()) {
        continue;
      }
    }
    result.push_back(entry);
  }

  std::mt19937 mt(filter_criteria.seed);

  std::shuffle(result.begin(), result.end(), mt);

  int current_points = 0;
  size_t index = 0;

  std::vector<model::Question> selected;

  while (current_points < filter_criteria.max_points) {
    if (index < result.size()) {
      if (result[index].points + current_points <= filter_criteria.max_points) {
        current_points += result[index].points;
        selected.push_back(result[index]);
      }
      ++index;
    } else {
      break;
    }
  }
  return selected;
}

}  // namespace questforge::generator
