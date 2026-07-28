#ifndef QUESTFORGE_RENDERER_TYPST_RENDERER_H_
#define QUESTFORGE_RENDERER_TYPST_RENDERER_H_

#include <filesystem>
#include <vector>

#include "model/question.h"

namespace questforge::renderer {

class TypstRenderer {
 public:
  explicit TypstRenderer(const std::filesystem::path& template_path)
      : template_path_(template_path) {}

  // Throws std::runtime_error if an error occurs
  void Render(const std::vector<questforge::model::Question>& questions,
              const std::filesystem::path& output_path);

 private:
  std::filesystem::path template_path_;
};

}  // namespace questforge::renderer

#endif  // QUESTFORGE_RENDERER_TYPST_RENDERER_H_
