#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <format>
#include <stdexcept>
#include <system_error>

#include "platform/process_runner.h"

namespace questforge::platform {
// Runs args[0] as a child process via fork+exec (not system(), to avoid
// shell-injection risk from untrusted arguments) and redirects the child's
// stderr into a pipe so its output can be captured into the exception
// message instead of leaking straight to this process's own stderr.
//
// The pipe is drained (read to EOF) before waitpid() is called. This order
// matters: a pipe has a limited kernel buffer (64 KiB on Linux), and if the
// child wrote more than that before exiting, it would block on write()
// waiting for a reader while we blocked on waitpid() waiting for it to
// exit - a classic pipe deadlock. Reading first means we're always ready to
// drain the pipe while the child is still writing to it.
void PosixProcessRunner::Run(const std::vector<std::string>& args) const {
  std::vector<char*> argv;

  for (const auto& entry : args) {
    argv.push_back(const_cast<char*>(entry.c_str()));
  }
  argv.push_back(nullptr);

  int pipefd[2];
  char buf[256];
  ssize_t n;
  if (pipe(pipefd) == -1) {
    throw std::system_error(errno, std::generic_category(), "pipe");
  }
  pid_t c_pid = fork();

  if (c_pid == -1) {
    throw std::system_error(errno, std::generic_category(), "fork");
  } else if (c_pid > 0) {
    int status;

    std::string message;
    close(pipefd[1]);
    while ((n = read(pipefd[0], buf, sizeof(buf))) > 0) {
      message.append(buf, n);
    }
    close(pipefd[0]);

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
              std::format("failed with code {}\n{}", exit_code, message));
      }
    } else if (WIFSIGNALED(status)) {
      throw std::runtime_error(
          std::format("typst was killed by signal {}", WTERMSIG(status)));
    }
  } else {
    dup2(pipefd[1], STDERR_FILENO);
    close(pipefd[0]);
    close(pipefd[1]);
    execvp(argv[0], argv.data());
    perror("execvp");
    _exit(127);
  }
}

}  // namespace questforge::platform
