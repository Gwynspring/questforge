#ifndef QUESTFORGE_SRC_MODEL_QUESTION_H
#define QUESTFORGE_SRC_MODEL_QUESTION_H

#include <cstdint>
#include <filesystem>
#include <string>
#include <tuple>
#include <vector>
namespace questforge::model {

enum class Difficulty { kEasy, kMedium, kHard };

struct Question {
  std::string id;
  std::string topic;
  Difficulty difficulty;
  uint8_t points;
  std::string text;
  std::filesystem::path image;
  std::vector<std::string> tags;
};

}  // namespace questforge::model

#endif  // QUESTFORGE_SRC_MODEL_QUESTION_H
