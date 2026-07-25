#include "render/typst_renderer.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <vector>

TEST(RendererTest, RenderQuestionSuccesful) {
  questforge::model::Question q = {"alg-001",
                                   "algebra",
                                   questforge::model::Difficulty::kEasy,
                                   2,
                                   "Solve: $2x + 3 = 7$",
                                   std::nullopt,
                                   {"equations", "linear"}};
  std::filesystem::path template_path =
      PROJECT_ROOT "/templates/test.typ.jinja";

  questforge::renderer::TypstRenderer renderer(template_path);

  std::vector<questforge::model::Question> questions;
  questions.push_back(q);

  renderer.Render(questions,
                  std::filesystem::temp_directory_path() / "test_output.pdf");

  std::ifstream test_file(std::filesystem::temp_directory_path() /
                          "test_output.typ");

  std::string content((std::istreambuf_iterator<char>(test_file)),
                      std::istreambuf_iterator<char>());

  EXPECT_NE(content.find("Solve: $2x + 3 = 7$"), std::string::npos);
  EXPECT_NE(content.find("2 P"), std::string::npos);
}
