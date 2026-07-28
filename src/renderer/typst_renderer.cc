#include "renderer/typst_renderer.h"

#include <cstdlib>
#include <format>
#include <fstream>
#include <inja/inja.hpp>
#include <stdexcept>

namespace questforge::renderer {

void TypstRenderer::Render(
    const std::vector<questforge::model::Question>& questions,
    const std::filesystem::path& output_path) {
  nlohmann::json arr = nlohmann::json::array();

  int total_points = 0;

  for (const auto& entry : questions) {
    nlohmann::json obj;

    obj["text"] = entry.text;
    obj["points"] = entry.points;

    if (entry.image.has_value()) {
      obj["image"] = entry.image->string();
    } else {
      obj["image"] = nullptr;
    }

    total_points += entry.points;
    arr.push_back(obj);
  }

  nlohmann::json data = {{"questions", arr}, {"total_points", total_points}};

  inja::Environment env;

  auto typ_path = output_path;
  typ_path.replace_extension(".typ");
  std::ofstream output_file(typ_path);

  if (!output_file.is_open()) {
    throw std::runtime_error(
        std::format("Output file could not be opened. {}", typ_path.string()));
  }

  output_file << env.render_file(template_path_.string(), data);

  output_file.close();
  if (output_file.rdstate() & std::ios::failbit) {
    throw std::runtime_error(std::format(
        "Output file {} was not closed successfully.", typ_path.string()));
  }

  // TODO: Harden the subprocess call: escape/quote paths (spaces, shell
  // metacharacters), evaluate the real exit code (WEXITSTATUS on POSIX) instead
  // of the raw std::system return value, and capture typst's stderr so failures
  // report why. Keep it cross-platform.
  std::string command =
      "typst compile " + typ_path.string() + " " + output_path.string();

  int result = std::system(command.c_str());

  if (result != 0) {
    throw std::runtime_error("Could not process command " + command);
  }
}

}  // namespace questforge::renderer
