#include "platform/windows_command_line.h"

#include <string>
#include <vector>

#include <gtest/gtest.h>

TEST(WindowsCommandLine, PassesThroughArgumentWithoutSpecialChars) {
  std::vector<std::string> args{R"(C:\typst\bin)"};
  std::string cmd = questforge::platform::BuildCommandLine(args);

  EXPECT_EQ(cmd, R"(C:\typst\bin)");
}

TEST(WindowsCommandLine, QuotesArgumentContainingSpace) {
  std::vector<std::string> args{" "};
  std::string cmd = questforge::platform::BuildCommandLine(args);

  EXPECT_EQ(cmd, "\" \"");
}

TEST(WindowsCommandLine, QuotesArgumentContainingTab) {
  std::vector<std::string> args{"\t"};
  std::string cmd = questforge::platform::BuildCommandLine(args);

  EXPECT_EQ(cmd, "\"\t\"");
}

TEST(WindowsCommandLine, EscapesEmbeddedQuote) {
  std::vector<std::string> args{"\""};
  std::string cmd = questforge::platform::BuildCommandLine(args);

  EXPECT_EQ(cmd, "\"\\\"\"");
}

TEST(WindowsCommandLine, DoublesTrailingBackslashInQuotedArgument) {
  std::vector<std::string> args{R"(C:\out dir\)"};
  std::string cmd = questforge::platform::BuildCommandLine(args);

  EXPECT_EQ(cmd, "\"C:\\out dir\\\\\"");
}

TEST(WindowsCommandLine, DoublesBackslashRunBeforeClosingQuote) {
  std::vector<std::string> args{R"(C:\out dir\\)"};
  std::string cmd = questforge::platform::BuildCommandLine(args);

  EXPECT_EQ(cmd, "\"C:\\out dir\\\\\\\\\"");
}

TEST(WindowsCommandLine, EscapesQuoteAfterBackslash) {
  std::vector<std::string> args{"a\"b"};
  std::string cmd = questforge::platform::BuildCommandLine(args);

  EXPECT_EQ(cmd, "\"a\\\"b\"");
}

TEST(WindowsCommandLine, JoinsArgumentsWithSingleSpace) {
  std::vector<std::string> args{"typst", "compile", "main.typ"};
  std::string cmd = questforge::platform::BuildCommandLine(args);

  EXPECT_EQ(cmd, "typst compile main.typ");
}

TEST(WindowsCommandLine, PreservesEmptyArgument) {
  std::vector<std::string> args{"", "compile", "main.typ"};
  std::string cmd = questforge::platform::BuildCommandLine(args);

  EXPECT_EQ(cmd, "\"\" compile main.typ");
}

TEST(WindowsCommandLine, SeparatesQuotedAndUnquotedArguments) {
  std::vector<std::string> args{"typst", "compile", "a b"};
  std::string cmd = questforge::platform::BuildCommandLine(args);

  EXPECT_EQ(cmd, "typst compile \"a b\"");
}