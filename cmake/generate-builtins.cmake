file(GLOB builtins src/builtins/*.cmake)

set(builtin_text "#pragma once
#include <cn/builtins.hpp>
namespace {
void setup_builtins(cn::builtin_map& map)
{
"
)

foreach(file ${builtins})
  get_filename_component(basename ${file} NAME_WE)

  file(READ "${file}" builtin)
  string(APPEND builtin_text "  map[\"${basename}\"] = R\"_(${builtin})_\";\n\n")
endforeach()
string(APPEND builtin_text "}
}\n")

file(GENERATE
  OUTPUT
    "${CMAKE_CURRENT_BINARY_DIR}/builtin_setup.hpp"
  CONTENT
    "${builtin_text}"
)

