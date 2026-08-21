#include <cstdio>
#include <cstdlib>
#include <string>

int main(int argc, char* argv[]) {
  if (argc <= 1) {
    return 0;
  }
  std::string mode = argv[1];
  if (mode == "ok") {
    return 0;
  } else if (mode == "fail") {
    std::fprintf(stderr, "child error message\n");
    return 42;
  } else if (mode == "signal") {
    std::abort();
  } else {
    return 1;
  }
}
