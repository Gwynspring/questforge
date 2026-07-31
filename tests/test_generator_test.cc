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

class TestGeneratorTest : public ::testing::Test {
 protected:
  void SetUp() override { questions_ = MakeQuestions(); }

  std::vector<questforge::model::Question> questions_;
  questforge::generator::TestGenerator generator_;

  questforge::generator::FilterCriteria fc_no_seed_no_topic{2, 2, 2};
  questforge::generator::FilterCriteria fc_no_seed_no_topic_different_count{
      2, 0, 1};
  questforge::generator::FilterCriteria fc_no_seed_with_topic{
      1, 1, 1, {{"algebra", "geometry"}}, std::nullopt};
  questforge::generator::FilterCriteria fc_with_seed_with_topic{
      1, 1, 1, {{"algebra"}}, 42};
  questforge::generator::FilterCriteria fc_not_enough_questions{10, 10, 10};
  questforge::generator::FilterCriteria fc_seed_a{2, 2, 2, std::nullopt, 1};
  questforge::generator::FilterCriteria fc_seed_b{2, 2, 2, std::nullopt, 2};
};

TEST_F(TestGeneratorTest, GenerateQuestionsSuccessful) {
  std::vector<questforge::model::Question> result =
      generator_.Generate(questions_, fc_with_seed_with_topic);

  EXPECT_FALSE(result.empty());

  for (const auto& entry : result) {
    EXPECT_EQ(entry.topic, "algebra");
    EXPECT_EQ(entry.image, std::nullopt);
  }
}

TEST_F(TestGeneratorTest, EmptyInputThrowsException) {
  EXPECT_THROW((void)generator_.Generate({}, fc_no_seed_no_topic),
               std::runtime_error);
}

TEST_F(TestGeneratorTest, GenerateQuestionsWithoutTopicFilter) {
  std::vector<questforge::model::Question> result =
      generator_.Generate(questions_, fc_no_seed_no_topic);

  EXPECT_FALSE(result.empty());

  EXPECT_TRUE(std::any_of(result.begin(), result.end(),
                          [](const auto& q) { return q.topic == "algebra"; }));
  EXPECT_TRUE(std::any_of(result.begin(), result.end(),
                          [](const auto& q) { return q.topic == "geometry"; }));
}

TEST_F(TestGeneratorTest, SameSeedGivesSameOrder) {
  std::vector<questforge::model::Question> result_1 =
      generator_.Generate(questions_, fc_with_seed_with_topic);
  std::vector<questforge::model::Question> result_2 =
      generator_.Generate(questions_, fc_with_seed_with_topic);

  EXPECT_EQ(result_1, result_2);
}

TEST_F(TestGeneratorTest, DifferentSeedGivesDifferentOrder) {
  std::vector<questforge::model::Question> result_1 =
      generator_.Generate(questions_, fc_seed_a);
  std::vector<questforge::model::Question> result_2 =
      generator_.Generate(questions_, fc_seed_b);

  EXPECT_NE(result_1, result_2);
}

TEST_F(TestGeneratorTest, NotEnoughQuestionsAfterFiltering) {
  EXPECT_THROW((void)generator_.Generate(questions_, fc_not_enough_questions),
               std::runtime_error);
}

TEST_F(TestGeneratorTest, SelectsExactCountPerDifficulty) {
  std::vector<questforge::model::Question> result =
      generator_.Generate(questions_, fc_no_seed_no_topic_different_count);

  int count_easy =
      std::count_if(result.begin(), result.end(), [](const auto& q) {
        return q.difficulty == questforge::model::Difficulty::kEasy;
      });

  int count_medium =
      std::count_if(result.begin(), result.end(), [](const auto& q) {
        return q.difficulty == questforge::model::Difficulty::kMedium;
      });

  int count_hard =
      std::count_if(result.begin(), result.end(), [](const auto& q) {
        return q.difficulty == questforge::model::Difficulty::kHard;
      });

  EXPECT_EQ(count_easy, 2);
  EXPECT_EQ(count_medium, 0);
  EXPECT_EQ(count_hard, 1);
};
