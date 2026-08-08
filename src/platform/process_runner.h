#ifndef QUESTFORGE_PLATFORM_PROCESS_RUNNER_H_
#define QUESTFORGE_PLATFORM_PROCESS_RUNNER_H_

#include <string>
#include <vector>

namespace questforge::platform {

class ProcessRunner {
 public:
  virtual ~ProcessRunner() = default;
  // Runs a child process (args[0] is the program name, searched in PATH) and
  // waits for it. Throws std::system_error on fork/waitpid failure and
  // std::runtime_error on a non-zero exit, a missing binary, or a killing
  // signal.
  virtual void Run(const std::vector<std::string>& args) const = 0;
};

class PosixProcessRunner : public ProcessRunner {
 public:
  void Run(const std::vector<std::string>& args) const override;
};

class WindowsProcessRunner : public ProcessRunner {
 public:
  void Run(const std::vector<std::string>& args) const override;
};

}  // namespace questforge::platform

#endif  // QUESTFORGE_PLATFORM_PROCESS_RUNNER_H_
