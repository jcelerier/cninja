# Enforce existence of linked-to functions at compile time
# Note: does not work with asan / ubsan.
# See https://github.com/google/sanitizers/issues/380

cninja_require(pre)
cninja_optional(lld)

if(NOT "asan" IN_LIST CNINJA_FEATURES)
  if(NOT "ubsan" IN_LIST CNINJA_FEATURES)
    if(APPLE)
      # -Wl,-warn_weak_exports could also be useful
      set(temp_LINKER_WARNINGS "-Wl,-fatal_warnings -Wl,-undefined,dynamic_lookup")
    elseif(NOT WIN32)
      if((CNINJA_LINKER STREQUAL "lld") OR (CNINJA_LINKER STREQUAL "gold"))
        set(temp_LINKER_WARNINGS
             "-Wl,-z,defs \
    -Wl,-z,now \
    -Wl,--unresolved-symbols,report-all \
    -Wl,--no-undefined \
    -Wl,--no-allow-shlib-undefined \
    -Wl,--no-allow-multiple-definition")
      endif()
    endif()

    add_linker_flags(" ${temp_LINKER_WARNINGS}")
    unset(temp_LINKER_WARNINGS)
  endif()
endif()
