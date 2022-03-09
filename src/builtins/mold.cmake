# Use lld as a linker when available (Windows & Linux). Super fast. Default.
cninja_require(pre)
cninja_optional(compiler=clang)

if(NOT WIN32 AND NOT APPLE)
  # GCC does not have robust support for that yet
  if(CNINJA_COMPILER STREQUAL "clang")
    # mold is a much faster linker: https://github.com/rui314/mold
    add_linker_flags(" -fuse-ld=mold")
    set(CNINJA_LINKER mold)
  endif()
endif()

