#ifndef QUESTFORGE_MODEL_QUESTION_H_
#define QUESTFORGE_MODEL_QUESTION_H_

#include <filesystem>
#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace questforge::model {

enum class Difficulty { kEasy, kMedium, kHard };

inline std::string_view DifficultyName(Difficulty d) {
  switch (d) {
    case Difficulty::kEasy:
      return "easy";
    case Difficulty::kMedium:
      return "medium";
    case Difficulty::kHard:
      return "hard";
  }
  throw std::invalid_argument(
      std::format("unknown difficulty value: {}", static_cast<int>(d)));
}

inline std::optional<Difficulty> StringToDifficulty(std::string_view s) {
  if (s == "easy") return Difficulty::kEasy;
  if (s == "medium") return Difficulty::kMedium;
  if (s == "hard") return Difficulty::kHard;
  return std::nullopt;
}

struct Question {
  std::string id;
  std::string topic;
  Difficulty difficulty = Difficulty::kEasy;
  int points = 0;
  std::string text;
  std::optional<std::filesystem::path> image;
  std::vector<std::string> tags;

  bool operator==(const Question& other) const = default;
};

}  // namespace questforge::model

#endif  // QUESTFORGE_MODEL_QUESTION_H_
