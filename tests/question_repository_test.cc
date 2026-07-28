#include "repository/question_repository.h"

#include <gtest/gtest.h>

// TODO: Cover image parsing (null vs. set path) and an empty catalog. Use the
// PROJECT_ROOT macro for fixture paths instead of CWD-relative strings.
TEST(QuestionRepositoryTest, LoadsCatalogSuccessfully) {
  questforge::repository::QuestionRepository repo;
  auto questions = repo.LoadCatalog("tests/fixtures/valid.yaml");

  EXPECT_EQ(questions.size(), 2u);
  EXPECT_EQ(questions.at(0).id, "alg-001");
  EXPECT_EQ(questions.at(0).topic, "algebra");
  EXPECT_EQ(questions.at(0).difficulty, questforge::model::Difficulty::kEasy);
  EXPECT_EQ(questions.at(0).points, 2);
  EXPECT_EQ(questions.at(0).tags.at(0), "equations");
}

TEST(QuestionRepositoryTest, ThrowsInvalidArgumentForDifficulty) {
  questforge::repository::QuestionRepository repo;

  EXPECT_THROW(repo.LoadCatalog("tests/fixtures/invalid_difficulty.yaml"),
               std::invalid_argument);
}

TEST(QuestionRepositoryTest, ThrowsRuntimeErrorForPoints) {
  questforge::repository::QuestionRepository repo;
  EXPECT_THROW(repo.LoadCatalog("tests/fixtures/invalid_points.yaml"),
               std::runtime_error);
}

TEST(QuestionRepositoryTest, ThrowsRuntimeErrorForInvalidPath) {
  questforge::repository::QuestionRepository repo;
  EXPECT_THROW(repo.LoadCatalog("load/invalid/path.yaml"), std::runtime_error);
}
