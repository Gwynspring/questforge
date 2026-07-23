#ifndef QUESTFORGE_REPOSITORY_QUESTION_REPOSITORY_H_
#define QUESTFORGE_REPOSITORY_QUESTION_REPOSITORY_H_

#include <filesystem>
#include <vector>

#include "model/question.h"

namespace questforge::repository {

class QuestionRepository {
 public:
  std::vector<model::Question> LoadCatalog(const std::filesystem::path& path);
};

}  // namespace questforge::repository

#endif  // QUESTFORGE_REPOSITORY_QUESTION_REPOSITORY_H_
