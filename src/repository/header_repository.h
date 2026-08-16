#ifndef QUESTFORGE_REPOSITORY_HEADER_REPOSITORY_H_
#define QUESTFORGE_REPOSITORY_HEADER_REPOSITORY_H_

#include <filesystem>

#include "model/header.h"

namespace questforge::repository {
model::Header LoadHeader(const std::filesystem::path& path);
}  // namespace questforge::repository

#endif  // QUESTFORGE_REPOSITORY_HEADER_REPOSITORY_H_
