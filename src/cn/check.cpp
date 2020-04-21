#include "check.hpp"
#define BOOST_ASIO_DISABLE_THREADS 1
#define BOOST_ERROR_CODE_HEADER_ONLY 1
#define BOOST_REGEX_NO_LIB 1
#define BOOST_DATE_TIME_NO_LIB 1
#define BOOST_SYSTEM_NO_LIB 1

#include <iostream>
#include <sstream>
#include <regex>
#include <boost/process.hpp>
namespace cn
{
namespace
{
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
  namespace bp = boost::process;
  fmt::print("Checking: {}\n", command);
  int res = bp::system(command.c_str(), bp::std_out > bp::null, bp::std_err > bp::null);
  if(res != 0)
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
  namespace bp = boost::process;
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
    bp::ipstream is;
    std::string line;
    bp::child process = min_version
        ? bp::child{*path, "--version", bp::std_out > is, bp::std_err > bp::null}
        : bp::child{*path, "--version", bp::std_out > bp::null, bp::std_err > bp::null};

    if (min_version)
    {
      while (process.running() && std::getline(is, line) && !line.empty())
        lines.push_back(line);
    }
    process.wait();

    if(process.exit_code() != 0)
    {
      fmt::print("... error ! \n");
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
      "echo int main(){} | " + sys.clangpp_binary + " -x c++ -stdlib=libc++ -fuse-ld=lld - -o nul"
#elif (__APPLE__)
      "echo 'int main(){}' | " + sys.clangpp_binary + " -x c++ - -o /dev/null"
#else
      "echo 'int main(){}' | " + sys.clangpp_binary
      + " -x c++ -stdlib=libc++ -fuse-ld=lld - -o /dev/null"
#endif
      ;

  if (sys.clangpp_binary.empty() || !check_system_command(clang_test_command))
  {
    if constexpr (sys.os_linux)
    {
      fmt::print("clang not found. Please install clang 9 or later: \n");
      if (fs::exists("/usr/bin/apt"))
        fmt::print(
            "sudo apt update ; sudo apt install clang-9 libc++-9-dev libc++abi-9-dev lld-9 \n");
      else if (fs::exists("/usr/bin/pacman"))
        fmt::print("sudo apt update ; sudo apt install clang libc++ lld \n");
      else if (fs::exists("/usr/bin/yum"))
        fmt::print("sudo yum update ; sudo yum install clang libcxx-devel libcxxabi-devel lld \n");
    }
    else if constexpr (sys.os_apple)
    {
      fmt::print(
          "clang not found. Please install either Xcode through the appstore or the command line "
          "tools.\n");
    }
    else if constexpr (sys.os_windows)
    {
      fmt::print(
          "clang not found. Please install clang and put it in your PATH: \n"
          "https://github.com/mstorsjo/llvm-mingw/releases\n");
    }
    return false;
  }

  if (!fs::exists("CMakeLists.txt") && !fs::exists("build.ninja"))
  {
    fmt::print("CMakeLists.txt not found. Run cninja from a CMake source or binary dir.\n");
    return false;
  }

  return true;
}

}
