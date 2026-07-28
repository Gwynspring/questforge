#include "repository/question_repository.h"

#include <yaml-cpp/yaml.h>

#include <stdexcept>

#include "model/question.h"

namespace questforge::repository {

// TODO: Add semantic validation beyond type checks: reject empty id/text,
// non-positive points, and duplicate ids across the catalog.
std::vector<model::Question> QuestionRepository::LoadCatalog(
    const std::filesystem::path& path) {
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
      questions.push_back(q);
    }
  } catch (const YAML::Exception& e) {
    throw std::runtime_error("Failed to load catalog: " +
                             std::string(e.what()));
  }
  return questions;
}
}  // namespace questforge::repository
