#include "renderer/typst_renderer.h"

#include <format>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <inja/inja.hpp>

namespace questforge::renderer {

TypstRenderer::TypstRenderer(std::filesystem::path template_path,
                             std::unique_ptr<platform::ProcessRunner> runner)
    : template_path_(std::move(template_path)), runner_(std::move(runner)) {}

TypstRenderer::TypstRenderer(std::filesystem::path template_path)
    : TypstRenderer(std::move(template_path),
                    std::make_unique<platform::PosixProcessRunner>()) {}

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
  if (typ_path.extension() != ".pdf") {
    throw std::runtime_error(std::format(
        "Invalid output file extension. Extension must be '.pdf' not {}",
        typ_path.extension().string()));
  }
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

  std::vector<std::string> args{"typst", "compile", typ_path.string(),
                                output_path.string()};

  runner_->Run(args);
}

}  // namespace questforge::renderer
