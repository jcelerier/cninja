# Enables generation of a GDB index, which makes debugging faster.
cninja_require(pre)
cninja_optional(compiler=clang)
cninja_optional(compiler=gcc)

if(NOT WIN32 AND NOT APPLE)
  # Make debugging faster
  # Pro tip (Thanks Milian !): also add "set index-cache on" to your .gdbinit
  add_linker_flags(" -Wl,--gdb-index")
endif()
