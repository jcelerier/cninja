#include "fs.hpp"
#include "system.hpp"
#include <sstream>

namespace cn
{
std::optional<std::string> get_executable_path(std::string exe) noexcept
{
  std::stringstream ss(std::getenv("PATH"));
  std::string folder;
  if constexpr (sys.os_windows)
  {
    while (std::getline(ss, folder, ';'))
      if (auto str = folder + "\\" + exe + ".exe"; fs::exists(str))
        return str;
  }
  else
  {
    while (std::getline(ss, folder, ':'))
      if (auto str = folder + "/" + exe; fs::exists(str))
        return str;
  }
  return {};
}

}
