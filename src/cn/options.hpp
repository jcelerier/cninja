#pragma once
#include <string>
#include <vector>
#include <string_view>
#include <optional>

namespace cn
{
struct Options
{
  std::string source_folder;
  std::string build_folder;
  std::vector<std::string_view> options;
  std::vector<std::string_view> cmake_options;
};

std::string options_text();
Options parse_options(int argc, char** argv);
}
