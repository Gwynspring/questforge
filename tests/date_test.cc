#include "util/date.h"

#include <algorithm>
#include <cctype>
#include <string>

#include <gtest/gtest.h>

TEST(FormatTodayTest, DateFormatsSuccessfully) {
  std::string date = questforge::util::FormatToday();

  EXPECT_EQ(date.at(2), '.');
  EXPECT_EQ(date.at(5), '.');
  EXPECT_EQ(date.size(), 10);

  EXPECT_TRUE(std::all_of(date.begin(), date.begin() + 2,
                          [](unsigned char c) { return std::isdigit(c); }));
  EXPECT_TRUE(std::all_of(date.begin() + 3, date.begin() + 5,
                          [](unsigned char c) { return std::isdigit(c); }));
  EXPECT_TRUE(std::all_of(date.begin() + 6, date.end(),
                          [](unsigned char c) { return std::isdigit(c); }));
}
