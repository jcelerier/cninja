#include "check.hpp"

namespace cn
{
namespace
{
bool check_command(const std::string& command)
{
  fmt::print("Checking: {}\n", command);
  int res = system(command.c_str());
  fmt::print("\n");
  return res == 0;
}
}

bool check_environment() noexcept
{
  if(!check_command("cmake --version"))
  {
    fmt::print("cmake not found. Please install cmake: \n"
               "https://cmake.org/download\n");
    return false;
  }

  if(!check_command("ninja --version"))
  {
    fmt::print("ninja not found. Please install ninja: \n"
               "https://github.com/ninja-build/ninja/releases\n");
    return false;
  }

  const std::string clang_test_command =
    #if (_WIN32)
      "echo int main(){} | " + sys.clangpp_binary + " -x c++ -stdlib=libc++ -fuse-ld=lld - -o nul"
    #elif (__APPLE__)
      "echo 'int main(){}' | " + sys.clangpp_binary + " -x c++ - -o /dev/null"
    #else
      "echo 'int main(){}' | " + sys.clangpp_binary + " -x c++ -stdlib=libc++ -fuse-ld=lld - -o /dev/null"
    #endif
      ;

  if(sys.clangpp_binary.empty() || !check_command(clang_test_command))
  {
    if constexpr(sys.os_linux)
    {
      fmt::print("clang not found. Please install clang 9 or later: \n");
      if(fs::exists("/usr/bin/apt"))
        fmt::print("sudo apt update ; sudo apt install clang-9 libc++-9-dev libc++abi-9-dev lld-9 \n");
      else if(fs::exists("/usr/bin/pacman"))
        fmt::print("sudo apt update ; sudo apt install clang libc++ lld \n");
      else if(fs::exists("/usr/bin/yum"))
        fmt::print("sudo yum update ; sudo yum install clang libcxx-devel libcxxabi-devel lld \n");
    }
    else if constexpr(sys.os_apple)
    {
      fmt::print("clang not found. Please install either Xcode through the appstore or the command line tools.\n");
    }
    else if constexpr(sys.os_windows)
    {
      fmt::print("clang not found. Please install clang and put it in your PATH: \n"
                 "https://github.com/mstorsjo/llvm-mingw/releases\n");
    }
    return false;
  }

  if(!fs::exists("CMakeLists.txt") && !fs::exists("build.ninja"))
  {
    fmt::print("CMakeLists.txt not found. Run cninja from a CMake source or binary dir.\n");
    return false;
  }

  return true;
}

}
