#include "config.hpp"
#include <cn/fs.hpp>

#include <fstream>

namespace cn
{
std::optional<std::string> read_config_file(const std::string_view name)
{
  auto cur_path = fs::current_path();
  for(;;)
  {
    const auto config_path = cur_path / ".cninja" / name;
    if(fs::exists(config_path))
    {
      std::ifstream stream{config_path};

      return std::string{
        std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>()
      };
    }

    if(cur_path != (cur_path.root_path() / cur_path.root_directory()))
      cur_path = cur_path.parent_path();
    else
      break;
  }

  return std::nullopt;
}
}
