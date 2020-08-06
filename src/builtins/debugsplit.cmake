# Split debug info in separate files.
cninja_optional(asan)

# TODO: also add those somewhere as they are nice to have ?
# -Wa,--compress-debug-sections
# -Wl,--compress-debug-sections=zlib
if(NOT asan IN_LIST CNINJA_FEATURES)
  string(APPEND CMAKE_C_FLAGS_INIT " -gsplit-dwarf")
  string(APPEND CMAKE_CXX_FLAGS_INIT " -gsplit-dwarf")
  add_linker_flags(" -gsplit-dwarf -Wl,--gdb-index")
endif()
