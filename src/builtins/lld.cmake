# Use lld as a linker when available (Windows & Linux). Super fast. Default.
cninja_require(pre)
cninja_optional(compiler=clang)
cninja_optional(compiler=gcc)
cninja_optional(lto)

if(NOT APPLE)
  # GCC does not have robust support for that yet
  if(CNINJA_COMPILER STREQUAL "clang")
    # LLD is a much faster linker : https://lld.llvm.org
    add_linker_flags(" -fuse-ld=lld")
    set(CNINJA_LINKER lld)
  else()
    # LLD does not work well yet with gcc, let's try to revert back to GNU Gold
    add_linker_flags(" -fuse-ld=gold")
    set(CNINJA_LINKER gold)
  endif()
endif()

if(NOT WIN32 AND NOT APPLE)
  # Make linking faster
  # Except that gold segfaults with -flto and --threads...
  if((CNINJA_LINKER STREQUAL "gold") AND NOT ("lto" IN_LIST CNINJA_FEATURES) AND NOT (CNINJA_COMPILER STREQUAL "gcc"))
    add_linker_flags(" -Wl,--threads")
  endif()
endif()
