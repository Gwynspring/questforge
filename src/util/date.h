#ifndef QUESTFORGE_UTIL_DATE_H_
#define QUESTFORGE_UTIL_DATE_H_

#include <string>

namespace questforge::util {
// UTC is used instead of local time
// std::chrono::zoned_time{std::chrono::current_zone(), now} needs the
// IANA-TZ-Database
// Returns DD.MM.YYYY
std::string FormatToday();
}  // namespace questforge::util

#endif  // QUESTFORGE_UTIL_DATE_H_
