# Force symbols in shared libraries to be loaded on startup instead of lazily loaded.
cninja_require(pre)

if(APPLE)
  add_linker_flags(" -Wl,-z,now")
elseif(MSVC)
  # Nothing special to do here, it's the default
else()
  # For macOS this only works for executables
  string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -Wl,-bind_at_load")
endif()
