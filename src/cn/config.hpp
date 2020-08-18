#pragma once
#include <optional>

#include <string_view>

namespace cn
{
std::optional<std::string> read_config_file(const std::string& name, const std::string& source_dir);
}
