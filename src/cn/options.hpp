#pragma once
#include <vector>
#include <string>
#include <string_view>

namespace cn
{
struct Options
{
  std::vector<std::string_view> options;
  std::vector<std::string_view> cmake_options;
};

std::string options_text();
Options parse_options(int argc, char** argv);
}
