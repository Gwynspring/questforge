#include "render/typst_renderer.h"

#include <cstdlib>
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

    entry.image.has_value() ? obj["image"] = entry.image->string()
                            : obj["image"] = nullptr;

    total_points += entry.points;
    arr.push_back(obj);
  }

  nlohmann::json data = {{"questions", arr}, {"total_points", total_points}};

  inja::Environment env;

  auto typ_path = output_path;
  typ_path.replace_extension(".typ");
  std::ofstream output_file(typ_path);

  if (!output_file.is_open()) {
    throw std::runtime_error("Outputfile could not be opened." +
                             output_path.string());
  }

  output_file << env.render_file(template_path_, data);
  output_file.close();

  std::string command =
      "typst compile " + typ_path.string() + " " + output_path.string();

  int result = std::system(command.c_str());

  if (result != 0) {
    throw std::runtime_error("Could not process command " + command);
  }
};

}  // namespace questforge::renderer
