#pragma once
#include <cn/fs.hpp>

#include <sstream>
#include <string>

namespace cn
{
static const struct System
{
  static constexpr bool os_apple
  {
#if defined(__APPLE__)
    true
#endif
  };

  static constexpr bool os_linux
  {
#if defined(__linux__)
    true
#endif
  };

  static constexpr bool os_windows
  {
#if defined(_WIN32)
    true
#endif
  };

  // Returns true if the file passed in argument exists
  // somewhere in the path
  static bool is_in_path(std::string exe) noexcept
  {
    std::stringstream ss(std::getenv("PATH"));
    std::string folder;
    if constexpr (os_windows)
    {
      while (std::getline(ss, folder, ';'))
        if (fs::exists(folder + "\\" + exe + ".exe"))
          return true;
    }
    else
    {
      while (std::getline(ss, folder, ':'))
        if (fs::exists(folder + "/" + exe))
          return true;
    }
    return false;
  }

  const std::string clang_binary = []() {
    if (is_in_path("clang-11"))
      return "clang-11";
    else if (is_in_path("clang-10"))
      return "clang-10";
    else if (is_in_path("clang-9"))
      return "clang-9";
    else if (is_in_path("clang-8"))
      return "clang-9";
    else if (is_in_path("clang"))
      return "clang";
    else
      return "";
  }();
  const std::string clangpp_binary = []() {
    if (is_in_path("clang++-11"))
      return "clang++-11";
    else if (is_in_path("clang++-10"))
      return "clang++-10";
    else if (is_in_path("clang++-9"))
      return "clang++-9";
    else if (is_in_path("clang++-8"))
      return "clang++-9";
    else if (is_in_path("clang++"))
      return "clang++";
    else
      return "";
  }();
} sys;
}
