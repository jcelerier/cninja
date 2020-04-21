#pragma once
#include <ghc/filesystem.hpp>
#include <string>
#include <optional>

namespace fs = ghc::filesystem;

namespace cn
{
// Returns true if the file passed in argument exists
// somewhere in the path
std::optional<std::string> get_executable_path(std::string exe) noexcept;
}
