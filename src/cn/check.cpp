#include "check.hpp"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <regex>
#include <cinttypes>
namespace cn
{
namespace
{
#if defined(_WIN32) && !defined(__MINGW__)
#define popen _popen
#define pclose _pclose

// let's not fight that too much : https://stackoverflow.com/questions/735126/are-there-alternate-implementations-of-gnu-getline-interface/47229318#47229318

typedef intptr_t ssize_t;

ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
    size_t pos;
    int c;

    if (lineptr == NULL || stream == NULL || n == NULL) {
        errno = EINVAL;
        return -1;
    }

    c = getc(stream);
    if (c == EOF) {
        return -1;
    }

    if (*lineptr == NULL) {
        *lineptr = (char*)malloc(128);
        if (*lineptr == NULL) {
            return -1;
        }
        *n = 128;
    }

    pos = 0;
    while(c != EOF) {
        if (pos + 1 >= *n) {
            size_t new_size = *n + (*n >> 2);
            if (new_size < 128) {
                new_size = 128;
            }
            char *new_ptr = (char*)realloc(*lineptr, new_size);
            if (new_ptr == NULL) {
                return -1;
            }
            *n = new_size;
            *lineptr = new_ptr;
        }

        ((unsigned char *)(*lineptr))[pos ++] = c;
        if (c == '\n') {
            break;
        }
        c = getc(stream);
    }

    (*lineptr)[pos] = '\0';
    return pos;
}
#endif

struct version_number {
  int major{};
  int minor{};

  friend constexpr bool operator<=(version_number lhs, version_number rhs) noexcept {
    if(lhs.major < rhs.major)
      return true;
    else if(lhs.major > rhs.major)
      return false;
    else
      return lhs.minor <= rhs.minor;
  }
};

bool check_system_command(const std::string& command)
{
  fmt::print("Checking: {}\n", command);

  FILE* pipe = popen(command.c_str(), "r");

  if(auto res = pclose(pipe); res != 0)
  {
    fmt::print("... error ! \n");
    return false;
  }
  else
  {
    fmt::print("... ok ! \n");
    return true;
  }
}

bool check_version(const std::string& command, std::optional<version_number> min_version = {})
{
  auto path = get_executable_path(command);
  if (!path)
  {
    fmt::print("... error ! \n");
    return false;
  }

  fmt::print("Checking: {} --version\n", command);

  // Run the process and save its stdout
  std::vector<std::string> lines;
  {
    std::string line;
    FILE* pipe = popen((command + " --version").c_str(), "r");

    {
      char* buffer = nullptr;
      size_t size = 0;
      ssize_t read_size = 0;

      do {
        read_size = getline(&buffer, &size, pipe);
        lines.push_back(std::string(buffer));
      } while(read_size > 0);

      if(buffer)
        ::free(buffer);
    }

    if(int res = pclose(pipe); res != 0)
    {
      fmt::print("... error 1 ! \n");
      return false;
    }
  }

  if(min_version)
  {
    static const std::regex version_check{R"_(([0-9]+)\.([0-9]+))_"};

    for(auto& line : lines)
    {
      std::smatch match;
      if (std::regex_search(line, match, version_check))
      {
        if (match.size() == 3)
        {
          int major = std::stoi(match[1].str());
          int minor = std::stoi(match[2].str());
          if(*min_version <= version_number{major, minor})
          {
            fmt::print("... ok ! \n");
            return true;
          }
          else
          {
            fmt::print("... version {}.{} too old ! \n", major, minor);
            return false;
          }
        }
      }
    }

    fmt::print("... no version number found, assuming it is enough ! \n");
    return true;
  }
  else
  {
    fmt::print("... ok ! \n");
    return true;
  }
}
}

bool check_environment() noexcept
{
  if (!check_version("cmake", version_number{3, 12}))
  {
    fmt::print(
        "cmake not found. Please install cmake: \n"
        "https://cmake.org/download\n");
    return false;
  }

  if (!check_version("ninja"))
  {
    fmt::print(
        "ninja not found. Please install ninja: \n"
        "https://github.com/ninja-build/ninja/releases\n");
    return false;
  }

  const std::string clang_test_command =
#if (_WIN32)
      "echo int main(){} | " + sys.clang_binary + " -x c++ -stdlib=libc++ -fuse-ld=lld - -o nul"
#elif (__APPLE__)
      "echo 'int main(){}' | " + sys.clang_binary + " -x c++ - -o /dev/null"
#else
      "echo 'int main(){}' | " + sys.clang_binary
      + " -x c++ -stdlib=libc++ -fuse-ld=lld - -o /dev/null"
#endif
      ;

  if (sys.clang_binary.empty() || !check_system_command(clang_test_command))
  {
    fmt::print(" ---------------------------------------------------------------- \n");
    if constexpr (sys.os_linux)
    {
      fmt::print("clang toolchain not found. We recommend installing a recent clang/libc++/lld: \n");
      if (fs::exists("/usr/bin/apt"))
        fmt::print("sudo apt update ; "
                   "sudo apt install clang-10 libc++-10-dev libc++abi-10-dev lld-10 \n");
      else if (fs::exists("/usr/bin/pacman"))
        fmt::print("sudo pacman -Syu ; sudo pacman -S clang libc++ lld \n");
      else if (fs::exists("/usr/bin/yum"))
        fmt::print("sudo yum update ; sudo yum install clang libcxx-devel libcxxabi-devel lld \n");
    }
    else if constexpr (sys.os_apple)
    {
      fmt::print(
          "clang toolchain not found. Please install either Xcode through the appstore or the command line "
          "tools.\n");
    }
    else if constexpr (sys.os_windows)
    {
      fmt::print(
          "clang toolchain not found. Please install clang/libc++/lld and put it in your PATH: \n"
          " - either with https://github.com/mstorsjo/llvm-mingw/releases\n"
          " - or with the official LLVM binaries\n"
          " - or with MSYS2");
    }
    fmt::print(" ---------------------------------------------------------------- \n");
  }

  return true;
}

}
