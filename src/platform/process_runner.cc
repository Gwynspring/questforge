#include "platform/process_runner.h"

#ifdef _WIN32

#include <cerrno>
#include <format>
#include <stdexcept>
#include <system_error>

#include <windows.h>

#include "platform/windows_command_line.h"

#else

#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <format>
#include <stdexcept>
#include <system_error>

#endif  // _WIN32

namespace questforge::platform {

#ifdef _WIN32

void WindowsProcessRunner::Run(const std::vector<std::string>& args) const {
  std::string command_line = BuildCommandLine(args);

  std::vector<char> command_line_buf(command_line.begin(), command_line.end());
  command_line_buf.push_back('\0');

  SECURITY_ATTRIBUTES pipe_attributes{};
  pipe_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
  pipe_attributes.bInheritHandle = TRUE;
  pipe_attributes.lpSecurityDescriptor = nullptr;

  HANDLE stderr_read = nullptr;
  HANDLE stderr_write = nullptr;
  if (!CreatePipe(&stderr_read, &stderr_write, &pipe_attributes, 0)) {
    throw std::system_error(GetLastError(), std::system_category(),
                            "CreatePipe");
  }

  if (!SetHandleInformation(stderr_read, HANDLE_FLAG_INHERIT, 0)) {
    throw std::system_error(GetLastError(), std::system_category(),
                            "SetHandleInformation");
  }

  STARTUPINFOA startup_info{};
  startup_info.cb = sizeof(STARTUPINFOA);
  startup_info.dwFlags |= STARTF_USESTDHANDLES;
  startup_info.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
  startup_info.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  startup_info.hStdError = stderr_write;

  PROCESS_INFORMATION process_info{};

  BOOL created = CreateProcessA(
      /*lpApplicationName=*/nullptr,
      /*lpCommandLine=*/command_line_buf.data(),
      /*lpProcessAttributes=*/nullptr,
      /*lpThreadAttributes=*/nullptr,
      /*bInheritHandles=*/TRUE,
      /*dwCreationFlags=*/0,
      /*lpEnvironment=*/nullptr,
      /*lpCurrentDirectory=*/nullptr, &startup_info, &process_info);

  if (!created) {
    DWORD error = GetLastError();
    CloseHandle(stderr_read);
    CloseHandle(stderr_write);
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
      throw std::runtime_error("typst not found - is it installed?");
    }
    throw std::system_error(error, std::system_category(), "CreateProcessA");
  }

  CloseHandle(stderr_write);

  std::string message;
  char buffer[256];
  DWORD bytes_read = 0;
  while (ReadFile(stderr_read, buffer, sizeof(buffer), &bytes_read, nullptr) &&
         bytes_read > 0) {
    message.append(buffer, bytes_read);
  }
  CloseHandle(stderr_read);

  DWORD wait_result = WaitForSingleObject(process_info.hProcess, INFINITE);
  if (wait_result == WAIT_FAILED) {
    DWORD error = GetLastError();
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
    throw std::system_error(error, std::system_category(),
                            "WaitForSingleObject");
  }

  DWORD exit_code = 0;
  if (!GetExitCodeProcess(process_info.hProcess, &exit_code)) {
    DWORD error = GetLastError();
    CloseHandle(process_info.hProcess);
    CloseHandle(process_info.hThread);
    throw std::system_error(error, std::system_category(),
                            "GetExitCodeProcess");
  }
  CloseHandle(process_info.hProcess);
  CloseHandle(process_info.hThread);

  if (exit_code != 0) {
    throw std::runtime_error(
        std::format("failed with code {}\n{}", exit_code, message));
  }
}
#else
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
#endif  // _WIN32

}  // namespace questforge::platform
