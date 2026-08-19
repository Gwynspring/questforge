#include "repository/question_repository.h"

#include <format>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>

#include <yaml-cpp/yaml.h>

#include "model/question.h"

namespace questforge::repository {

namespace {

std::string FormatLocation(std::size_t index, const YAML::Mark& mark) {
  return std::format("question #{}, line {}, column {}", index + 1,
                     mark.line + 1, mark.column + 1);
}

void RequireField(const YAML::Node& entry, std::string_view field,
                  std::string_view location) {
  if (!entry[field].IsDefined() || entry[field].IsNull()) {
    throw std::invalid_argument(std::format(
        "required field '{}' is not defined in {}", field, location));
  }
}

void CheckEntry(const YAML::Node& entry, std::string_view location) {
  RequireField(entry, "id", location);
  RequireField(entry, "topic", location);
  RequireField(entry, "difficulty", location);
  RequireField(entry, "points", location);
  RequireField(entry, "text", location);
  RequireField(entry, "tags", location);
}

void ValidateQuestion(const model::Question& q, std::string_view location) {
  if (q.id.empty()) {
    throw std::invalid_argument(
        std::format("invalid declaration of question id. The id must not be "
                    "empty ({})",
                    location));
  }
  if (q.text.empty()) {
    throw std::invalid_argument(
        std::format("invalid declaration of question text. Question {} text "
                    "must not be empty ({})",
                    q.id, location));
  }
  if (q.points <= 0) {
    throw std::invalid_argument(
        std::format("invalid declaration of question points. Points must be "
                    "greater than 0. Points declared in question {}: {} ({})",
                    q.id, q.points, location));
  }

  if (q.topic.empty()) {
    throw std::invalid_argument(std::format(
        "invalid declaration of question topic. Topic must not be empty ({})",
        location));
  }
}

void ValidateQuestionNode(const YAML::Node& node) {
  if (!node.IsSequence()) {
    switch (node.Type()) {
      case YAML::NodeType::Null:
        throw std::invalid_argument(
            "the 'questions' key is present but empty; expected a list of "
            "questions");
      case YAML::NodeType::Scalar:
        throw std::invalid_argument(
            "expected 'questions' to be a list, but found a scalar value");
      case YAML::NodeType::Map:
        throw std::invalid_argument(
            "expected 'questions' to be a list, but found a map value");
      case YAML::NodeType::Undefined:
        throw std::invalid_argument(
            "catalog is missing the required 'questions' key");
      default:
        throw std::invalid_argument(
            "unexpected error occurred while loading node from file");
    }
  }
}

}  // namespace

std::vector<model::Question> QuestionRepository::LoadCatalog(
    const std::filesystem::path& path) {
  std::unordered_set<std::string> id_set;
  std::vector<model::Question> questions;
  try {
    YAML::Node root = YAML::LoadFile(path.string());
    try {
      ValidateQuestionNode(root["questions"]);
    } catch (const std::exception& e) {
      throw std::runtime_error("Failed to load catalog: " +
                               std::string(e.what()));
    }
    std::size_t i = 0;
    for (const auto& entry : root["questions"]) {
      const std::string location = FormatLocation(i, entry.Mark());
      CheckEntry(entry, location);
      model::Question q;

      q.id = entry["id"].as<std::string>();
      q.topic = entry["topic"].as<std::string>();

      std::string difficulty_string = entry["difficulty"].as<std::string>();

      auto difficulty = model::StringToDifficulty(difficulty_string);
      if (!difficulty) {
        throw std::invalid_argument(
            std::format("invalid difficulty declaration: {} ({})",
                        difficulty_string, location));
      }
      q.difficulty = *difficulty;
      q.points = entry["points"].as<int>();
      q.text = entry["text"].as<std::string>();
      q.tags = entry["tags"].as<std::vector<std::string>>();

      if (entry["image"].IsDefined() && !entry["image"].IsNull()) {
        std::string p = entry["image"].as<std::string>();
        q.image = std::filesystem::absolute(path.parent_path()) / p;
      }

      if (entry["solution"].IsDefined() && !entry["solution"].IsNull()) {
        q.solution = entry["solution"].as<std::string>();
      }

      ValidateQuestion(q, location);

      if (!id_set.insert(q.id).second) {
        throw std::invalid_argument(std::format(
            "duplicated question id found: {} ({})", q.id, location));
      }

      questions.push_back(q);
      ++i;
    }
  }
  // Only structural parse errors (malformed YAML) are caught and wrapped
  // here. std::invalid_argument (semantic validation failures, e.g. an
  // empty question id) is intentionally left uncaught so it propagates
  // unwrapped, letting callers tell a broken catalog file apart from a
  // well-formed but invalid question.
  catch (const YAML::Exception& e) {
    throw std::runtime_error("Failed to load catalog: " +
                             std::string(e.what()));
  }
  return questions;
}
}  // namespace questforge::repository
