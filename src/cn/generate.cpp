#include "generate.hpp"

#include <cn/config.hpp>
#include <cn/fmt.hpp>
#include <cn/graph.hpp>
#include <cn/system.hpp>
#include <cn/util.hpp>

namespace cn
{

std::string generate_cmake_call(Options options)
{
  std::string cmd;
  cmd += fmt::format("cmake {}", options.source_folder);
  cmd += " -GNinja -Wno-dev -DCMAKE_TOOLCHAIN_FILE=cninja-toolchain.cmake ";

  for (auto opt : options.cmake_options)
  {
    cmd += opt;
    cmd += ' ';
  }

  return cmd;
}

std::string generate_build_path(Options options)
{
  std::string p = "build-";
  auto options_names = options.options;
  std::sort(options_names.begin(), options_names.end());

  for (const auto& opt : options_names)
    p += fmt::format("{}-", opt);

  // Remove last dash character
  p.pop_back();
  return p;
}

std::string generate_toolchain(Options options)
{
  std::string toolchain_file;

  // First write down the commands used
  toolchain_file += "#cninja-options: ";
  for(auto& opt : options.options)
  {
    toolchain_file += opt;
    toolchain_file += " ";
  }

  // Then write down the cmake call used if any
  toolchain_file += "\n#cninja-cmake-invocation: ";
  toolchain_file += generate_cmake_call(options);
  toolchain_file += "\n";

  // Append the toolchain content
  toolchain_file += graph{options.options}.generate();

  return toolchain_file;
}

}
