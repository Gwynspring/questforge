#include "platform/windows_command_line.h"

#include <string>
#include <string_view>
#include <vector>

namespace questforge::platform {

namespace {
bool NeedsQuoting(std::string_view arg) {
  return arg.find_first_of(" \t\"") != std::string_view::npos || arg.empty();
}
}  // namespace

std::string QuoteArg(std::string_view arg) {
  if (!NeedsQuoting(arg)) {
    return std::string{arg};
  }

  std::string out;
  out.push_back('"');

  std::size_t i = 0;
  const std::size_t len = arg.size();

  while (i < len) {
    std::size_t backslash_count = 0;

    while (i < len && arg[i] == '\\') {
      ++backslash_count;
      ++i;
    }

    if (i == len) {
      out.append(backslash_count * 2, '\\');
      break;
    } else if (arg[i] == '"') {
      out.append(backslash_count * 2 + 1, '\\');
      out.push_back('"');
      ++i;
    } else {
      out.append(backslash_count, '\\');
      out.push_back(arg[i]);
      ++i;
    }
  }

  out.push_back('"');
  return out;
}

std::string BuildCommandLine(const std::vector<std::string>& args) {
  std::string out;

  for (bool first = true; const auto& entry : args) {
    if (!first) {
      out += " ";
    }
    first = false;
    out += QuoteArg(entry);
  }
  return out;
}

}  // namespace questforge::platform
