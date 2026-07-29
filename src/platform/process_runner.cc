#include "platform/process_runner.h"

#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <format>
#include <stdexcept>
#include <system_error>

namespace questforge::platform {
void PosixProcessRunner::Run(const std::vector<std::string>& args) const {
  std::vector<char*> argv;

  for (const auto& entry : args) {
    argv.push_back(const_cast<char*>(entry.c_str()));
  }
  argv.push_back(nullptr);

  pid_t c_pid = fork();

  if (c_pid == -1) {
    throw std::system_error(errno, std::generic_category(), "fork");
  } else if (c_pid > 0) {
    int status;

    if (waitpid(c_pid, &status, 0) == -1) {
      throw std::system_error(errno, std::generic_category(), "waitpid");
    }
    if (WIFEXITED(status)) {
      int exit_code = WEXITSTATUS(status);
      switch (exit_code) {
        case 0:
          break;
        case 127:
          throw std::runtime_error("typst not found - is it installed?");
        default:
          throw std::runtime_error(
              std::format("failed with code {}", exit_code));
      }
    } else if (WIFSIGNALED(status)) {
      throw std::runtime_error(
          std::format("typst was killed by signal {}", WTERMSIG(status)));
    }
  } else {
    execvp(argv[0], argv.data());
    perror("execvp");
    _exit(127);
  }
}

}  // namespace questforge::platform
