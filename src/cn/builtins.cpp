#include "builtins.hpp"

#include <cn/fmt.hpp>
#include <cn/system.hpp>

#include "builtin_setup.hpp"
namespace cn
{
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
