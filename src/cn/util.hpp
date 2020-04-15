#pragma once
#include <string>

namespace cn
{
static
std::string uppercase(std::string s) noexcept
{
  std::transform(
        s.begin(), s.end(),
        s.begin(),
        [] (unsigned char c) { return std::toupper(c); }
  );
  return s;
}
}
