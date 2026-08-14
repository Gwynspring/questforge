#ifndef QUESTFORGE_PLATFORM_WINDOWS_COMMAND_LINE_H_
#define QUESTFORGE_PLATFORM_WINDOWS_COMMAND_LINE_H_
#include <string>
#include <string_view>
#include <vector>

// Quotes and joins argument strings into a single Windows command-line
// string, such that a child process parsing it via the CRT/
// CommandLineToArgvW rules reconstructs each argument exactly as given here.
// See:
// https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-commandlinetoargvw
//
// Compiled unconditionally (not just on `_WIN32`) so this logic can be
// unit-tested on any platform.
//
// Follows Windows quoting rules, not POSIX shell quoting rules — do not use
// this to quote/escape arguments for POSIX shells (sh, bash, etc.).
namespace questforge::platform {

// Returns `arg` unchanged if it contains none of ' ', '\t', '"', and is
// non-empty; otherwise returns a quoted, escaped form such that
// CommandLineToArgvW parses it back to exactly `arg`. An empty `arg` is
// quoted as `""` so it survives as a distinct (empty) argument.
std::string QuoteArg(std::string_view arg);

// Quotes each element of `args` via QuoteArg and joins the results with
// single spaces into one command-line string.
// Returns an empty string for an empty `args`.
std::string BuildCommandLine(const std::vector<std::string>& args);

}  // namespace questforge::platform
#endif  // QUESTFORGE_PLATFORM_WINDOWS_COMMAND_LINE_H_
