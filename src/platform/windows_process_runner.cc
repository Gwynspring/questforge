#include <format>
#include <stdexcept>
#include <system_error>

#include <windows.h>

#include "platform/process_runner.h"
#include "platform/windows_command_line.h"

namespace questforge::platform {

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
}  // namespace questforge::platform
