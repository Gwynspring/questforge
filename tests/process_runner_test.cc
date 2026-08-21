#include "platform/process_runner.h"

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "gmock/gmock.h"

#ifdef _WIN32
using Runner = questforge::platform::WindowsProcessRunner;
#else
using Runner = questforge::platform::PosixProcessRunner;
#endif  // _WIN32

TEST(ProcessRunnerTest, RunSuccessful) {
  std::vector<std::string> args{TEST_CHILD_PATH};
  const Runner runner;
  EXPECT_NO_THROW(runner.Run(args));
}

TEST(ProcessRunnerTest, RunFailsWithMessage) {
  std::vector<std::string> args{TEST_CHILD_PATH, "fail"};
  const Runner runner;
  try {
    runner.Run(args);
    FAIL();
  } catch (const std::exception& e) {
    EXPECT_THAT(e.what(), testing::HasSubstr("42"));
    EXPECT_THAT(e.what(), testing::HasSubstr("child error message\n"));
  }
}

TEST(ProcessRunnerTest, RunFailsWithNotFound) {
  std::vector<std::string> args{"questforge-definitely-not-a-real-binary"};
  const Runner runner;
  try {
    runner.Run(args);
    FAIL();
  } catch (const std::exception& e) {
    EXPECT_THAT(e.what(), testing::HasSubstr("not found"));
  }
}

TEST(ProcessRunnerTest, RunKilledBySignal) {
#ifdef _WIN32
  GTEST_SKIP() << "WIFSIGNALED is POSIX-only";
#endif  // _WIN32
  std::vector<std::string> args{TEST_CHILD_PATH, "signal"};
  const Runner runner;
  try {
    runner.Run(args);
    FAIL();
  } catch (const std::exception& e) {
    EXPECT_THAT(e.what(), testing::HasSubstr("killed by signal"));
  }
}
