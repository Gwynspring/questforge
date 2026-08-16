#include "repository/header_repository.h"

#include <gtest/gtest.h>

#include "model/header.h"

TEST(LoadHeaderTest, LoadsHeaderFileSuccessfully) {
  const std::filesystem::path path =
      PROJECT_ROOT "/tests/fixtures/school_valid.yaml";
  questforge::model::Header header = questforge::repository::LoadHeader(path);

  std::filesystem::path logo_path = path.parent_path() / "logo.png";

  questforge::model::Header header_expected{
      .logo = logo_path,
      .school_lines = {"Höhere Technische Bundes-Lehranstalt",
                       "Graz-Gösting (BULME)", "Abteilung für Elektrotechnik"}};

  EXPECT_EQ(header, header_expected);
}

TEST(LoadHeaderTest, ThrowsOnMissingFile) {
  const std::filesystem::path path = "invalid/path/for/header_config.yaml";

  EXPECT_THROW(questforge::repository::LoadHeader(path), std::runtime_error);
}

TEST(LoadHeaderTest, ThrowsOnMissingSchoolKey) {
  const std::filesystem::path path =
      PROJECT_ROOT "/tests/fixtures/school_missing_school_lines.yaml";

  EXPECT_THROW(questforge::repository::LoadHeader(path), std::invalid_argument);
}

TEST(LoadHeaderTest, ThrowsOnScalarSchool) {
  const std::filesystem::path path =
      PROJECT_ROOT "/tests/fixtures/school_scalar.yaml";

  EXPECT_THROW(questforge::repository::LoadHeader(path), std::invalid_argument);
}

TEST(LoadHeaderTest, LoadsWithoutLogo) {
  const std::filesystem::path path =
      PROJECT_ROOT "/tests/fixtures/school_missing_logo.yaml";

  questforge::model::Header header = questforge::repository::LoadHeader(path);

  questforge::model::Header header_expected{
      .school_lines = {"Höhere Technische Bundes-Lehranstalt",
                       "Graz-Gösting (BULME)", "Abteilung für Elektrotechnik"}};
  EXPECT_EQ(header.logo, std::nullopt);

  EXPECT_EQ(header, header_expected);
}
