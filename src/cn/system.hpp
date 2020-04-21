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

  const std::string clang_binary = []() {
    using namespace std::literals;
    if (auto p = get_executable_path("clang-11"))
      return *p;
    if (auto p = get_executable_path("clang-10"))
      return *p;
    if (auto p = get_executable_path("clang-9"))
      return *p;
    if (auto p = get_executable_path("clang-8"))
      return *p;
    if (auto p = get_executable_path("clang-7"))
      return *p;
    if (auto p = get_executable_path("clang"))
      return *p;
    return ""s;
  }();
  const std::string clangpp_binary = []() {
    using namespace std::literals;
    if (auto p = get_executable_path("clang++-11"))
      return *p;
    if (auto p = get_executable_path("clang++-10"))
      return *p;
    if (auto p = get_executable_path("clang++-9"))
      return *p;
    if (auto p = get_executable_path("clang++-8"))
      return *p;
    if (auto p = get_executable_path("clang++-7"))
      return *p;
    if (auto p = get_executable_path("clang++"))
      return *p;
    return ""s;
  }();
} sys;
}
