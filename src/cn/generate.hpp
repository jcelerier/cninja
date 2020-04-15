#pragma once
#include <cn/options.hpp>
#include <string>

namespace cn
{
std::string generate_cmake_call(Options options);
std::string generate_build_path(Options options);
std::string generate_toolchain(Options options);
}
