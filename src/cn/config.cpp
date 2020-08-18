#include "config.hpp"

#include <cn/fs.hpp>
#include <cn/fmt.hpp>

#include <fstream>

namespace cn
{
namespace
{
// Read the whole content of a file in a string
std::string read_file(const fs::path& path)
{
  std::ifstream stream{path};

  return {
    std::istreambuf_iterator<char>(stream),
    std::istreambuf_iterator<char>()
  };
}

// Get a path pointing to the content of the HOME variable
// (or the equivalent on other systems)
fs::path get_home_folder()
{
  if(const char* const home = getenv("HOME"))
    return home;

  if(const char* const up = getenv("USERPROFILE"))
    return up;

  const char* const homedrive = getenv("HOMEDRIVE");
  const char* const homepath = getenv("HOMEPATH");
  if(homedrive && homepath)
  {
    return fmt::format("{}/{}", homedrive, homepath);
  }

  // Apparently we're missing a case on macOS - GUI apps don't define
  // HOME. Thankfully we aren't a GUI app.

  return {};
}
}

std::optional<std::string> read_config_file(const std::string& name, const std::string& source_dir)
{
  fs::path cur_path = source_dir;
  const std::string full_name = name + ".cmake";

  // First check the arborescence going from the current folder to the root.
  const auto root_path = (cur_path.root_path() / cur_path.root_directory());
  for (;;)
  {
    const auto config_path = cur_path / ".cninja" / full_name;
    if (fs::exists(config_path))
      return read_file(config_path);

    if (cur_path != root_path)
      cur_path = cur_path.parent_path();
    else
      break;
  }

  // If we haven't found anyhting, look into the home folder.
  if(auto home = get_home_folder(); !home.empty())
  {
    const auto config_path = home / ".config" / "cninja" / full_name;
    if (fs::exists(config_path))
      return read_file(config_path);
  }

  return std::nullopt;
}
}
