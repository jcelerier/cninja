# Discard unused symbols & libraries
cninja_require(pre)

if(APPLE)
  # Apple way to strip unneeded libs:
  add_linker_flags(" -Wl,-dead_strip -Wl,-dead_strip_dylibs")
endif()
