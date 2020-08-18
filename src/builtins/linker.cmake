# Discard unused symbols & libraries
cninja_require(pre)

# -ffunction-sections, etc... : Allows to discard unused code more easily with --gc-sections
string(APPEND CMAKE_C_FLAGS_INIT " -ffunction-sections -fdata-sections")
string(APPEND CMAKE_CXX_FLAGS_INIT " -ffunction-sections -fdata-sections")

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
  add_linker_flags(" -Wl,-dead_strip -Wl,-dead_strip_dylibs")
endif()
