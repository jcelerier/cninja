#include "options.hpp"
#include <cn/builtins.hpp>
#include <cn/fmt.hpp>

#include <sstream>

namespace cn
{

std::string options_text()
{
  std::stringstream str;

  std::vector<std::string> options;

  // Gather all the options with an help text
  for(auto opt : cn::builtins())
  {
    if(opt.second.size() <= 2)
      continue;

    if(opt.second.front() == '#')
    {
      auto it = opt.second.find_first_of('\n');
      if(it != std::string::npos)
      {
        const auto help_text = std::string_view(opt.second.data() + 2, it - 1);
        options.push_back(fmt::format("  {:<10}\t{}", opt.first, help_text));
      }
    }
  }

  // Put them in alphabetical order
  std::sort(options.begin(), options.end());
  for(const auto& opt : options)
    str << opt;

  return str.str();
}

Options parse_options(int argc, char** argv)
{
  Options options{};
  std::vector<std::string_view> arg_opts;
  std::vector<std::string_view> arg_cmake;

  bool cmake{};
  for(int it = 1; it < argc; ++it) {
    std::string_view arg(argv[it]);
    if(arg == "--")
    {
      cmake = true;
      continue;
    }

    if(!cmake)
      options.options.push_back(arg);
    else
      options.cmake_options.push_back(arg);
  }

  if (std::find(options.options.begin(), options.options.end(), "help") != options.options.end())
  {
    fmt::print(R"_(cninja - an opinionated cmake frontend
               Usage:
               Invoke cninja with the default options:
               $ cninja

               Invoke cninja with custom options:
               $ cninja [OPTIONS...]

               Invoke cninja with custom options, and pass some flags to CMake:
               $ cninja [OPTIONS...] -- [CMake flags...]

               Some options are built-in ; others can be added by putting files in .cninja folders
               somewhere in your directory tree relative to this folder.

               Built-in options:
               {}
               )_", options_text());
    std::exit(0);
  }

  return options;
}

}
