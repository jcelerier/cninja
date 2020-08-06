#include "options.hpp"

#include <cn/builtins.hpp>
#include <cn/fmt.hpp>
#include <cn/fs.hpp>

#include <sstream>

namespace cn
{
namespace
{
std::string options_text()
{
  std::stringstream str;

  std::vector<std::string> options;

  // Gather all the options with an help text
  for (auto opt : cn::builtins())
  {
    if (opt.second.size() <= 2)
      continue;

    if (opt.second.front() == '#')
    {
      auto it = opt.second.find_first_of('\n');
      if (it != std::string::npos)
      {
        const auto help_text = std::string_view(opt.second.data() + 2, it - 1);
        options.push_back(fmt::format("  {:<10}\t{}", opt.first, help_text));
      }
    }
  }

  // Put them in alphabetical order
  std::sort(options.begin(), options.end());
  for (const auto& opt : options)
    str << opt;

  return str.str();
}

using actions_map = std::map<std::string, std::function<void(const Options&)>, std::less<>>;
const actions_map create_actions_map()
{
  actions_map map;

  auto help_action = [] (const Options&) {
      fmt::print(
          R"_(cninja - an opinionated cmake frontend
Usage:
Invoke cninja with the default options:
$ cninja

This help:
$ cninja help

Invoke cninja with custom options:
$ cninja [OPTIONS...]

Invoke cninja with custom options outside of source tree:
$ cninja ../some/source/folder [OPTIONS...]

Invoke cninja with custom options, and pass some flags to CMake:
$ cninja [OPTIONS...] -- [CMake flags...]

Some options are built-in ; others can be added by putting files in .cninja folders
somewhere in your directory tree relative to this folder.

Built-in options:
{}
)_", options_text());
  };

  map["-h"] = help_action;
  map["help"] = help_action;
  map["-help"] = help_action;
  map["--help"] = help_action;

  auto reserved_action = [] (const Options&) {
    fmt::print("Reserved for future use !\n");
  };
  map["ide"] = reserved_action;
  map["open"] = reserved_action;
  map["create"] = reserved_action;
  map["new"] = reserved_action;
  map["status"] = reserved_action;
  map["list"] = reserved_action;
  map["log"] = reserved_action;
  map["append"] = reserved_action;
  map["delete"] = reserved_action;
  map["remove"] = reserved_action;
  map["rm"] = reserved_action;
  map["erase"] = reserved_action;
  map["configure"] = reserved_action;
  map["build"] = reserved_action;
  map["clean"] = reserved_action;
  map["install"] = reserved_action;
  map["deploy"] = reserved_action;
  map["package"] = reserved_action;
  map["source"] = reserved_action;

  using namespace std::literals;
  for(char c = 'a' ; c <= 'z'; c++)
      map["-"s + c] = reserved_action;
  for(char c = 'A' ; c <= 'Z'; c++)
      map["-"s + c] = reserved_action;

  return map;
}

void parse_special_options(const Options& options)
{
  static const actions_map actions = create_actions_map();

  for(const std::string_view& opt : options.options)
  {
    if(auto it = actions.find(opt); it != actions.end())
    {
      (it->second)(options);
      std::exit(0);
    }
  }
}
}

Options parse_options(int argc, char** argv)
{
  Options options{};
  std::optional<std::string_view> source_folder;

  // Parse the options - anything after -- goes to cmake.
  bool cmake{};
  for (int it = 1; it < argc; ++it)
  {
    // If one of the argument is a folder, it is considered as the source folder.
    std::string_view arg(argv[it]);
    if (!source_folder && fs::is_directory(std::string(arg)))
    {
      source_folder = arg;
      continue;
    }

    if (arg == "--")
    {
      cmake = true;
      continue;
    }

    if (!cmake)
      options.options.push_back(arg);
    else
      options.cmake_options.push_back(arg);
  }

  // No source folder -> we use the working directory.
  if (source_folder)
    options.source_folder = fs::absolute(std::string(*source_folder));
  else
    options.source_folder = fs::absolute(".");

  // Check if we have something like help, etc.
  parse_special_options(options);

  return options;
}

}
