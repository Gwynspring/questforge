#ifndef QUESTFORGE_RENDERER_TYPST_RENDERER_H_
#define QUESTFORGE_RENDERER_TYPST_RENDERER_H_

#include <filesystem>
#include <memory>
#include <vector>

#include "model/question.h"
#include "platform/process_runner.h"

namespace questforge::renderer {

class TypstRenderer {
 public:
  TypstRenderer(std::filesystem::path template_path,
                std::unique_ptr<platform::ProcessRunner> runner);

  explicit TypstRenderer(std::filesystem::path template_path);

  // Throws std::runtime_error if an error occurs
  void Render(const std::vector<questforge::model::Question>& questions,
              const std::filesystem::path& output_path);

 private:
  std::filesystem::path template_path_;
  std::unique_ptr<platform::ProcessRunner> runner_;
};

}  // namespace questforge::renderer

#endif  // QUESTFORGE_RENDERER_TYPST_RENDERER_H_
