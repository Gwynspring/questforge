#include "repository/question_repository.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

class QuestionRepositoryTest : public ::testing::Test {
 protected:
  questforge::repository::QuestionRepository repo_;
};

TEST_F(QuestionRepositoryTest, LoadsCatalogSuccessfully) {
  auto questions = repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/valid.yaml");

  EXPECT_EQ(questions.size(), 2u);
  EXPECT_EQ(questions.at(0).id, "alg-001");
  EXPECT_EQ(questions.at(0).topic, "algebra");
  EXPECT_EQ(questions.at(0).difficulty, questforge::model::Difficulty::kEasy);
  EXPECT_EQ(questions.at(0).points, 2);
  EXPECT_EQ(questions.at(0).tags.at(0), "equations");
}

// This test with try - catch is only for personal demonstration of a different
// kind of testing strategy
TEST_F(QuestionRepositoryTest, ThrowsInvalidArgumentForDifficulty) {
  try {
    repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/invalid_difficulty.yaml");
    FAIL();
  } catch (const std::invalid_argument& e) {
    EXPECT_THAT(e.what(), testing::HasSubstr("difficulty"));
  }
}

TEST_F(QuestionRepositoryTest, ThrowsInvalidArgumentForPoints) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT
                          "/tests/fixtures/non_positive_points.yaml");
      },
      testing::ThrowsMessage<std::invalid_argument>(
          testing::HasSubstr("Points must be")));
}

TEST_F(QuestionRepositoryTest, ThrowsInvalidArgumentForDuplicateID) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/duplicate_id.yaml");
      },
      testing::ThrowsMessage<std::invalid_argument>(
          testing::HasSubstr("duplicated question")));
}

TEST_F(QuestionRepositoryTest, ThrowsInvalidArgumentForEmptyText) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/empty_text.yaml");
      },
      testing::ThrowsMessage<std::invalid_argument>(
          testing::HasSubstr("text must not be")));
}

TEST_F(QuestionRepositoryTest, ThrowsInvalidArgumentForEmptyID) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/empty_id.yaml");
      },
      testing::ThrowsMessage<std::invalid_argument>(
          testing::HasSubstr("id must not be")));
}

TEST_F(QuestionRepositoryTest, ThrowsInvalidArgumentForEmptyDifficulty) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/empty_difficulty.yaml");
      },
      testing::ThrowsMessage<std::invalid_argument>(
          testing::HasSubstr("invalid difficulty")));
}

TEST_F(QuestionRepositoryTest, ThrowsRuntimeErrorForInvalidPoints) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/not_a_number.yaml");
      },
      testing::ThrowsMessage<std::runtime_error>(
          testing::HasSubstr("Failed to load catalog:")));
}

TEST_F(QuestionRepositoryTest, ThrowsRuntimeErrorForInvalidPath) {
  try {
    repo_.LoadCatalog("this/is/an/invalid/path.yaml");
    FAIL();
  } catch (const std::runtime_error& e) {
    EXPECT_THAT(e.what(), testing::HasSubstr("Failed to load catalog"));
  }
}

TEST_F(QuestionRepositoryTest, ThrowsRuntimeErrorForNullQuestions) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/null_questions.yaml");
      },
      testing::ThrowsMessage<std::runtime_error>(
          testing::HasSubstr("the 'questions' key is present but empty")));
}

TEST_F(QuestionRepositoryTest, ThrowsRuntimeErrorForScalarQuestions) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/scalar_questions.yaml");
      },
      testing::ThrowsMessage<std::runtime_error>(
          testing::HasSubstr("but found a scalar value")));
}

TEST_F(QuestionRepositoryTest, ThrowsRuntimeErrorForMapQuestions) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/map_questions.yaml");
      },
      testing::ThrowsMessage<std::runtime_error>(
          testing::HasSubstr("but found a map value")));
}

TEST_F(QuestionRepositoryTest, ThrowsRuntimeErrorForMissingKey) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT
                          "/tests/fixtures/missing_questions_key.yaml");
      },
      testing::ThrowsMessage<std::runtime_error>(
          testing::HasSubstr("missing the required 'questions' key")));
}

TEST_F(QuestionRepositoryTest, ThrowsInvalidArgumentForEmptyTopic) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/empty_topic.yaml");
      },
      testing::ThrowsMessage<std::invalid_argument>(
          testing::HasSubstr("invalid declaration of question topic")));
}

TEST_F(QuestionRepositoryTest, ThrowsInvalidArgumentForMissingID) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/missing_id.yaml");
      },
      testing::ThrowsMessage<std::invalid_argument>(
          testing::HasSubstr("required field 'id' is not defined")));
}

TEST_F(QuestionRepositoryTest, ThrowsInvalidArgumentForMissingTopic) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/missing_topic.yaml");
      },
      testing::ThrowsMessage<std::invalid_argument>(
          testing::HasSubstr("required field 'topic' is not defined")));
}

TEST_F(QuestionRepositoryTest, ThrowsInvalidArgumentForMissingDifficulty) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT
                          "/tests/fixtures/missing_difficulty.yaml");
      },
      testing::ThrowsMessage<std::invalid_argument>(
          testing::HasSubstr("required field 'difficulty' is not defined")));
}

TEST_F(QuestionRepositoryTest, ThrowsInvalidArgumentForMissingPoints) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/missing_points.yaml");
      },
      testing::ThrowsMessage<std::invalid_argument>(
          testing::HasSubstr("required field 'points' is not defined")));
}

TEST_F(QuestionRepositoryTest, ThrowsInvalidArgumentForMissingText) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/missing_text.yaml");
      },
      testing::ThrowsMessage<std::invalid_argument>(
          testing::HasSubstr("required field 'text' is not defined")));
}

TEST_F(QuestionRepositoryTest, ThrowsInvalidArgumentForMissingTags) {
  EXPECT_THAT(
      [this] {
        repo_.LoadCatalog(PROJECT_ROOT "/tests/fixtures/missing_tags.yaml");
      },
      testing::ThrowsMessage<std::invalid_argument>(
          testing::HasSubstr("required field 'tags' is not defined")));
}

TEST_F(QuestionRepositoryTest, LoadsImagePathSuccessfully) {
  const std::filesystem::path path =
      PROJECT_ROOT "/tests/fixtures/question_with_image.yaml";

  auto questions = repo_.LoadCatalog(path);

  const auto path_expected = path.parent_path() / "graph.png";

  EXPECT_EQ(questions.at(0).image, path_expected);
}
