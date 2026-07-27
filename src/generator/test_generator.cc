#include "generator/test_generator.h"

#include <algorithm>
#include <format>
#include <iterator>
#include <random>
#include <stdexcept>

namespace questforge::generator {

[[nodiscard]] std::vector<model::Question> TestGenerator::Generate(
    const std::vector<model::Question>& questions,
    const FilterCriteria& filter_criteria) {
  std::vector<model::Question> easy, medium, hard;

  for (const auto& entry : questions) {
    if (filter_criteria.topics.has_value()) {
      if (std::find(filter_criteria.topics->begin(),
                    filter_criteria.topics->end(),
                    entry.topic) == filter_criteria.topics->end()) {
        continue;
      }
    }
    switch (entry.difficulty) {
      case model::Difficulty::kEasy:
        easy.push_back(entry);
        break;
      case model::Difficulty::kMedium:
        medium.push_back(entry);
        break;
      case model::Difficulty::kHard:
        hard.push_back(entry);
        break;
    }
  }

  uint32_t seed;

  if (!filter_criteria.seed.has_value()) {
    std::random_device rd;
    seed = rd();
  } else {
    seed = filter_criteria.seed.value();
  }
  std::mt19937 mt(seed);

  std::shuffle(easy.begin(), easy.end(), mt);
  std::shuffle(medium.begin(), medium.end(), mt);
  std::shuffle(hard.begin(), hard.end(), mt);

  if (easy.size() < static_cast<size_t>(filter_criteria.easy_count)) {
    throw std::runtime_error(
        std::format("Not enough easy questions available after filtering. Easy "
                    "questions available after filtering: {}",
                    easy.size()));
  } else if (medium.size() <
             static_cast<size_t>(filter_criteria.medium_count)) {
    throw std::runtime_error(std::format(
        "Not enough medium questions available after filtering. Medium "
        "questions available after filtering: {}",
        medium.size()));
  } else if (hard.size() < static_cast<size_t>(filter_criteria.hard_count)) {
    throw std::runtime_error(
        std::format("Not enough hard questions available after filtering. Hard "
                    "questions available after filtering: {}",
                    hard.size()));
  }

  std::vector<model::Question> selected;

  std::copy_n(easy.begin(), filter_criteria.easy_count,
              std::back_inserter(selected));
  std::copy_n(medium.begin(), filter_criteria.medium_count,
              std::back_inserter(selected));
  std::copy_n(hard.begin(), filter_criteria.hard_count,
              std::back_inserter(selected));

  return selected;
}

}  // namespace questforge::generator
