#include "builtins.hpp"

#include <cn/fmt.hpp>
#include <cn/system.hpp>

#include "builtin_setup.hpp"
namespace cn
{
namespace
{
#if defined(_WIN32)
std::string to_cmake_path(std::string input)
{
  for(char& c : input)
    if(c == '\\')
      c = '/';
  return input;
}
#else
#define to_cmake_path(str) str
#endif
}
const builtin_map& builtins()
{
  static const builtin_map map = [] {
    builtin_map map;
    setup_builtins(map);
    return map;
  }();
  return map;
}
}
