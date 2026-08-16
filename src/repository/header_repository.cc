#include "repository/header_repository.h"

#include <filesystem>
#include <string>

#include <yaml-cpp/yaml.h>

#include "model/header.h"

namespace questforge::repository {

model::Header LoadHeader(const std::filesystem::path& path) {
  model::Header header;

  try {
    YAML::Node root = YAML::LoadFile(path.string());

    if (root["logo"].IsDefined() && !root["logo"].IsNull()) {
      auto logo = root["logo"].as<std::string>();
      header.logo = std::filesystem::absolute(path.parent_path()) / logo;
    }

    if (!root["school"].IsSequence()) {
      switch (root["school"].Type()) {
        case YAML::NodeType::Null:
          throw std::invalid_argument(
              "the 'school' key is present but empty; expected a list of "
              "school lines");
        case YAML::NodeType::Scalar:
          throw std::invalid_argument(
              "expected 'school' to be a list, but found a scalar value");
        case YAML::NodeType::Map:
          throw std::invalid_argument(
              "expected 'school' to be a list, but found a map value");
        case YAML::NodeType::Undefined:
          throw std::invalid_argument(
              "header is missing the required 'school' key");
        default:
          throw std::invalid_argument(
              "unexpected error occurred while loading node from file");
      }
    }

    header.school_lines = root["school"].as<std::vector<std::string>>();
  } catch (const YAML::Exception& e) {
    throw std::runtime_error("Failed to load header from config: " +
                             std::string(e.what()));
  }
  return header;
}

}  // namespace questforge::repository
