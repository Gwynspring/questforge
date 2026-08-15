#ifndef QUESTFORGE_MODEL_HEADER_H_
#define QUESTFORGE_MODEL_HEADER_H_

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace questforge::model {

struct Header {
  std::optional<std::filesystem::path> logo;
  std::vector<std::string> school_lines;
  std::optional<std::string> date;

  bool operator==(const Header& other) const = default;
};

}  // namespace questforge::model

#endif  // QUESTFORGE_MODEL_HEADER_H_
