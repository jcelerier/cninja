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

  if(NOT APPLE)
    # In conjunction with ffunction-sections / fdata-sections, removes unused code
    add_linker_flags(" -Wl,--gc-sections")

    # Don't link against libraries of which no symbols are used
    add_linker_flags(" -Wl,--as-needed")

    # Make all platforms behave like Windows, which is in itself terrible but will
    # spare you trying to debug why dynamic_cast of inline classes across DLLs
    # doesn't work in MSW - simply don't write code assuming this works anywhere.
    add_linker_flags(" -Bsymbolic -Bsymbolic-functions")
  else()
    # Apple way to strip unneeded libs:
    add_linker_flags(" -Wl,-dead_strip")
  endif()

  if(NOT WIN32 AND NOT APPLE)
    # Make linking faster
    # Except that gold segfaults with -flto and --threads...
    if((CNINJA_LINKER STREQUAL "lld") OR NOT ("lto" IN_LIST CNINJA_FEATURES))
      add_linker_flags(" -Wl,--threads")
    endif()

    # Make debugging faster
    # Pro tip (Thanks Milian !): also add "set index-cache on" to your .gdbinit
    add_linker_flags(" -Wl,--gdb-index")
  endif()
endif()
