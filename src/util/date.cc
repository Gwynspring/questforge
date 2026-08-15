#include "util/date.h"

#include <chrono>
#include <format>

namespace questforge::util {
std::string FormatToday() {
  const auto now{std::chrono::system_clock::now()};

  const std::chrono::year_month_day ymd{
      std::chrono::floor<std::chrono::days>(now)};

  return std::format("{:0>2}.{:0>2}.{}", static_cast<unsigned>(ymd.day()),
                     static_cast<unsigned>(ymd.month()),
                     static_cast<int>(ymd.year()));
}

}  // namespace questforge::util
