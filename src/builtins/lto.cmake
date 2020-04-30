# Enable -flto={thin,full}. E.g. pass lto=full.
cninja_optional(compiler)
set(CNINJA_LTO_KIND "%lto%")

string(APPEND CMAKE_C_FLAGS_INIT " -flto=%lto%")
string(APPEND CMAKE_CXX_FLAGS_INIT " -flto=%lto%")

if("clang" STREQUAL "${CNINJA_COMPILER}")
  string(APPEND CMAKE_C_FLAGS_INIT " -fwhole-program-vtables")
  string(APPEND CMAKE_CXX_FLAGS_INIT " -fwhole-program-vtables")
endif()
# Will only be available in clang-11:
# -fvirtual-function-elimination
