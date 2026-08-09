#include "generator/test_generator.h"

#include <algorithm>
#include <format>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string_view>

namespace questforge::generator {

namespace {
std::mt19937 MakeNumberGenerator(uint32_t seed, model::Difficulty difficulty) {
  std::seed_seq seq{seed, static_cast<uint32_t>(difficulty)};

  return std::mt19937{seq};
}
}  // namespace

std::vector<model::Question> TestGenerator::TakeN(
    const std::vector<model::Question>& bucket, int count,
    model::Difficulty difficulty) {
  if (bucket.size() < static_cast<size_t>(count)) {
    std::string_view difficulty_string = model::DifficultyName(difficulty);
    throw std::runtime_error(std::format(
        "not enough {} questions available. Available {} questions: {}",
        difficulty_string, difficulty_string, bucket.size()));
  }
  std::vector<model::Question> result;
  std::copy_n(bucket.begin(), count, std::back_inserter(result));

  return result;
}

std::vector<model::Question> TestGenerator::Generate(
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

  std::shuffle(easy.begin(), easy.end(),
               MakeNumberGenerator(seed, model::Difficulty::kEasy));

  std::shuffle(medium.begin(), medium.end(),
               MakeNumberGenerator(seed, model::Difficulty::kMedium));

  std::shuffle(hard.begin(), hard.end(),
               MakeNumberGenerator(seed, model::Difficulty::kHard));

  auto easy_selected =
      TakeN(easy, filter_criteria.easy_count, model::Difficulty::kEasy);
  auto medium_selected =
      TakeN(medium, filter_criteria.medium_count, model::Difficulty::kMedium);
  auto hard_selected =
      TakeN(hard, filter_criteria.hard_count, model::Difficulty::kHard);

  std::vector<model::Question> selected;

  std::move(easy_selected.begin(), easy_selected.end(),
            std::back_inserter(selected));
  std::move(medium_selected.begin(), medium_selected.end(),
            std::back_inserter(selected));
  std::move(hard_selected.begin(), hard_selected.end(),
            std::back_inserter(selected));

  return selected;
}

}  // namespace questforge::generator
