# Creates a build optimized for this machine
cninja_require(pre)
cninja_require(lto=full)
cninja_optional(debugsyms)
cninja_optional(compiler)
cninja_optional(linker=lld)
cninja_optional(linker=mold)

# Not put in the _INIT as it isoverriden by the GNU Cmake module
string(APPEND CMAKE_C_FLAGS " -Ofast -march=native")
string(APPEND CMAKE_CXX_FLAGS " -Ofast -march=native")

# Identical code folding reduces size and improves performance
if(CNINJA_LINKER STREQUAL "lld")
  add_linker_flags(" -Wl,--icf=all")
endif()

if(NOT APPLE)
  add_linker_flags(" -Wl,-O3")

  if(NOT "debugsyms" IN_LIST CNINJA_FEATURES)
    add_linker_flags(" -Wl,--strip-all")
  endif()
endif()
