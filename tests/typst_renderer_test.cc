#include "renderer/typst_renderer.h"

#include <filesystem>
#include <fstream>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "model/header.h"

std::vector<questforge::model::Question> MakeQuestions() {
  return {
      {"alg-001",
       "algebra",
       questforge::model::Difficulty::kEasy,
       2,
       "Solve: $2x + 3 = 7$",
       std::nullopt,
       {"equations", "linear"}},
      {"geo-001",
       "geometry",
       questforge::model::Difficulty::kEasy,
       2,
       "Calculate the area of a circle with r=3.",
       std::nullopt,
       {"area"}},
      {"alg-002",
       "algebra",
       questforge::model::Difficulty::kMedium,
       4,
       "Solve: $x^2 - 5x + 6 = 0$",
       std::nullopt,
       {"equations", "quadratic"}},
      {"geo-003",
       "geometry",
       questforge::model::Difficulty::kMedium,
       4,
       "Calculate the perimeter of a rectangle with a=3, b=5.",
       std::nullopt,
       {"perimeter"}},
      {"alg-003",
       "algebra",
       questforge::model::Difficulty::kHard,
       6,
       "Solve the system of equations.",
       std::nullopt,
       {"systems"}},
      {"geo-002",
       "geometry",
       questforge::model::Difficulty::kHard,
       6,
       "Prove the Pythagorean theorem.",
       std::nullopt,
       {"proof"}},
  };
}

class RendererTest : public ::testing::Test {
 protected:
  void SetUp() override {
    questions_ = MakeQuestions();
    header_ = {"logo.png",
               {"Höhere Technische Bundes-Lehranstalt", "Graz-Gösting (BULME)",
                "Abteilung für Elektrotechnik"},
               "15.08.2026"};
  };

  std::vector<questforge::model::Question> questions_;

  questforge::model::Header header_;

  std::filesystem::path template_path_ =
      PROJECT_ROOT "/templates/test.typ.jinja";
  std::filesystem::path test_output_path_ =
      std::filesystem::temp_directory_path() / "test_output.pdf";
};

class MockProcessRunner : public questforge::platform::ProcessRunner {
 public:
  MOCK_METHOD(void, Run, (const std::vector<std::string>&), (const, override));
};

TEST_F(RendererTest, FillTemplateSuccessfully) {
  auto owned = std::make_unique<testing::NiceMock<MockProcessRunner>>();

  questforge::renderer::TypstRenderer renderer(template_path_,
                                               std::move(owned));

  renderer.Render(questions_, header_, test_output_path_);

  auto test_output_file = test_output_path_;

  test_output_file.replace_extension(".typ");

  std::ifstream test_file(test_output_file);

  std::string content((std::istreambuf_iterator<char>(test_file)),
                      std::istreambuf_iterator<char>());

  EXPECT_NE(content.find("Solve: $2x + 3 = 7$"), std::string::npos);
  EXPECT_NE(content.find("2 P"), std::string::npos);
}

TEST_F(RendererTest, InvokesRunnerWithTypstArgs) {
  auto owned = std::make_unique<MockProcessRunner>();

  MockProcessRunner* mock = owned.get();

  questforge::renderer::TypstRenderer renderer(template_path_,
                                               std::move(owned));

  EXPECT_CALL(*mock, Run(testing::ElementsAre("typst", "compile", testing::_,
                                              testing::_)));

  renderer.Render(questions_, header_, test_output_path_);
}

TEST_F(RendererTest, RendersHeaderContentSuccessfully) {
  auto owned = std::make_unique<testing::NiceMock<MockProcessRunner>>();

  questforge::renderer::TypstRenderer renderer(template_path_,
                                               std::move(owned));

  renderer.Render(questions_, header_, test_output_path_);

  auto test_output_file = test_output_path_;

  test_output_file.replace_extension(".typ");

  std::ifstream test_file(test_output_file);

  std::string content((std::istreambuf_iterator<char>(test_file)),
                      std::istreambuf_iterator<char>());

  EXPECT_NE(content.find("Höhere Technische Bundes-Lehranstalt"),
            std::string::npos);
  EXPECT_NE(content.find("Graz-Gösting (BULME"), std::string::npos);
  EXPECT_NE(content.find("Abteilung für Elektrotechnik"), std::string::npos);
}
