#include "repository/question_repository.h"

#include <yaml-cpp/yaml.h>

#include <stdexcept>

#include "model/question.h"

namespace questforge::repository {

std::vector<model::Question> QuestionRepository::LoadCatalog(
    const std::filesystem::path& path) {
  std::vector<model::Question> questions;
  try {
    YAML::Node root = YAML::LoadFile(path.string());
    for (const auto& entry : root["questions"]) {
      model::Question q;
      q.id = entry["id"].as<std::string>();
      q.topic = entry["topic"].as<std::string>();

      std::string difficulty = entry["difficulty"].as<std::string>();
      if (difficulty == "easy") {
        q.difficulty = model::Difficulty::kEasy;
      } else if (difficulty == "medium") {
        q.difficulty = model::Difficulty::kMedium;
      } else if (difficulty == "hard") {
        q.difficulty = model::Difficulty::kHard;
      } else {
        throw std::invalid_argument("Invalid difficulty declaration");
      }
      q.points = entry["points"].as<int>();
      q.text = entry["text"].as<std::string>();
      if (entry["image"].IsDefined() && !entry["image"].IsNull()) {
        std::string p = entry["image"].as<std::string>();
        q.image = p;
      }
      q.tags = entry["tags"].as<std::vector<std::string>>();
      questions.push_back(q);
    }
  } catch (const YAML::Exception& e) {
    throw std::runtime_error("Failed to load catalog: " +
                             std::string(e.what()));
  }
  return questions;
}
}  // namespace questforge::repository
