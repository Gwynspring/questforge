#ifndef QUESTFORGE_MODEL_QUESTION_H_
#define QUESTFORGE_MODEL_QUESTION_H_

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace questforge::model {

enum class Difficulty { kEasy, kMedium, kHard };

struct Question {
  std::string id;
  std::string topic;
  Difficulty difficulty;
  int points;
  std::string text;
  std::optional<std::filesystem::path> image;
  std::vector<std::string> tags;
};

}  // namespace questforge::model

#endif  // QUESTFORGE_MODEL_QUESTION_H_
