#include "repository/question_repository.h"

#include <yaml-cpp/yaml.h>

#include <format>
#include <stdexcept>
#include <string>
#include <unordered_set>

#include "model/question.h"

namespace questforge::repository {

namespace {

void ValidateQuestion(const model::Question& q) {
  if (q.id.empty()) {
    throw std::invalid_argument(
        "invalid declaration of question id. The id must not be empty");
  }
  if (q.text.empty()) {
    throw std::invalid_argument(std::format(
        "invalid declaration of question text. Question {} text must not be "
        "empty",
        q.id));
  }
  if (q.points <= 0) {
    throw std::invalid_argument(
        std::format("invalid declaration of question points. Points must be "
                    "greater than 0. Points declared in question {}: {}",
                    q.id, q.points));
  }
}

}  // namespace

std::vector<model::Question> QuestionRepository::LoadCatalog(
    const std::filesystem::path& path) {
  std::unordered_set<std::string> id_set;
  std::vector<model::Question> questions;
  try {
    YAML::Node root = YAML::LoadFile(path.string());
    for (const auto& entry : root["questions"]) {
      model::Question q;
      q.id = entry["id"].as<std::string>();
      q.topic = entry["topic"].as<std::string>();

      std::string difficulty_string = entry["difficulty"].as<std::string>();

      auto difficulty = model::StringToDifficulty(difficulty_string);
      if (!difficulty) {
        throw std::invalid_argument("invalid difficulty declaration: " +
                                    difficulty_string);
      }
      q.difficulty = *difficulty;

      q.points = entry["points"].as<int>();
      q.text = entry["text"].as<std::string>();
      if (entry["image"].IsDefined() && !entry["image"].IsNull()) {
        std::string p = entry["image"].as<std::string>();
        q.image = p;
      }
      q.tags = entry["tags"].as<std::vector<std::string>>();

      ValidateQuestion(q);

      if (!id_set.insert(q.id).second) {
        throw std::invalid_argument(
            std::format("duplicated question id found: {}", q.id));
      }

      questions.push_back(q);
    }
  } catch (const YAML::Exception& e) {
    throw std::runtime_error("Failed to load catalog: " +
                             std::string(e.what()));
  }
  return questions;
}
}  // namespace questforge::repository
