#include "generator/test_generator.h"

#include <gtest/gtest.h>

#include <algorithm>

std::vector<questforge::model::Question> MakeQuestions() {
  return {
      {"alg-001",
       "algebra",
       questforge::model::Difficulty::kEasy,
       2,
       "Solve: $2x + 3 = 7$",
       std::nullopt,
       {"equations", "linear"}},
      {"alg-002",
       "algebra",
       questforge::model::Difficulty::kMedium,
       4,
       "Solve: $x^2 - 5x + 6 = 0$",
       std::nullopt,
       {"equations", "quadratic"}},
      {"geo-001",
       "geometry",
       questforge::model::Difficulty::kEasy,
       2,
       "Calculate the area of a circle with r=3.",
       std::nullopt,
       {"area"}},
      {"geo-002",
       "geometry",
       questforge::model::Difficulty::kHard,
       6,
       "Prove the Pythagorean theorem.",
       std::nullopt,
       {"proof"}},
      {"alg-003",
       "algebra",
       questforge::model::Difficulty::kHard,
       6,
       "Solve the system of equations.",
       std::nullopt,
       {"systems"}},
      {"geo-003",
       "geometry",
       questforge::model::Difficulty::kMedium,
       4,
       "Calculate the perimeter of a rectangle with a=3, b=5.",
       std::nullopt,
       {"perimeter"}},
  };
}

class TestGeneratorTest : public ::testing::Test {
 protected:
  void SetUp() override { questions_ = MakeQuestions(); }

  std::vector<questforge::model::Question> questions_;

  questforge::generator::TestGenerator generator_;

  questforge::generator::FilterCriteria fc_easy_{
      {{"algebra"}}, questforge::model::Difficulty::kEasy, 10, 42};

  questforge::generator::FilterCriteria fc_no_topic_{
      std::nullopt, questforge::model::Difficulty::kEasy, 10, 42};

  questforge::generator::FilterCriteria fc_no_difficulty_{
      {{"geometry"}}, std::nullopt, 15, 42};

  questforge::generator::FilterCriteria fc_not_enough_points_{
      {{"algebra"}}, questforge::model::Difficulty::kHard, 100, 42};
};

TEST_F(TestGeneratorTest, GenerateQuestionsSuccesful) {
  std::vector<questforge::model::Question> result =
      generator_.Generate(questions_, fc_easy_);

  EXPECT_FALSE(result.empty());

  int points = 0;

  for (const auto& entry : result) {
    EXPECT_EQ(entry.topic, "algebra");
    EXPECT_EQ(entry.image, std::nullopt);
    points += entry.points;
  }
  EXPECT_LE(points, fc_easy_.max_points);
}

TEST_F(TestGeneratorTest, EmptyInputNoOutput) {
  std::vector<questforge::model::Question> result =
      generator_.Generate({}, fc_easy_);

  EXPECT_TRUE(result.empty());
}

TEST_F(TestGeneratorTest, GenerateQuestionsWithoutTopicFilter) {
  std::vector<questforge::model::Question> result =
      generator_.Generate(questions_, fc_no_topic_);

  EXPECT_FALSE(result.empty());

  EXPECT_TRUE(std::any_of(result.begin(), result.end(),
                          [](const auto& q) { return q.topic == "algebra"; }));
  EXPECT_TRUE(std::any_of(result.begin(), result.end(),
                          [](const auto& q) { return q.topic == "geometry"; }));

  int points = 0;

  for (const auto& entry : result) {
    points += entry.points;
  }
  EXPECT_LE(points, fc_no_topic_.max_points);
}

TEST_F(TestGeneratorTest, GenerateQuestionsWithoutDifficultyFilter) {
  std::vector<questforge::model::Question> result =
      generator_.Generate(questions_, fc_no_difficulty_);

  EXPECT_FALSE(result.empty());

  EXPECT_TRUE(std::any_of(result.begin(), result.end(), [](const auto& q) {
    return q.difficulty == questforge::model::Difficulty::kEasy;
  }));

  EXPECT_TRUE(std::any_of(result.begin(), result.end(), [](const auto& q) {
    return q.difficulty == questforge::model::Difficulty::kMedium;
  }));

  int points = 0;

  for (const auto& entry : result) {
    points += entry.points;
  }
  EXPECT_LE(points, fc_no_difficulty_.max_points);
}

TEST_F(TestGeneratorTest, SameSeedGivesSameOrder) {
  std::vector<questforge::model::Question> result_1 =
      generator_.Generate(questions_, fc_easy_);
  std::vector<questforge::model::Question> result_2 =
      generator_.Generate(questions_, fc_easy_);

  EXPECT_EQ(result_1, result_2);
}

TEST_F(TestGeneratorTest, NotEnoughPoints) {
  std::vector<questforge::model::Question> result =
      generator_.Generate(questions_, fc_not_enough_points_);

  EXPECT_FALSE(result.empty());

  int points = 0;
  for (const auto& entry : result) {
    points += entry.points;
  }
  EXPECT_LT(points, fc_not_enough_points_.max_points);
}
