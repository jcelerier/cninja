cninja_require(default)
cninja_optional(fast)
cninja_optional(small)
cninja_optional(debugsyms)
cninja_optional(compiler)
cninja_optional(lto)

if(NOT CMAKE_BUILD_TYPE AND NOT CNINJA_PLAIN)
  if("fast" IN_LIST CNINJA_FEATURES)
    if("debugsyms" IN_LIST CNINJA_FEATURES)
      set_cache(CMAKE_BUILD_TYPE RelWithDebInfo)
    else()
      set_cache(CMAKE_BUILD_TYPE Release)
    endif()
  else()
    if("small" IN_LIST CNINJA_FEATURES)
      # Believe it or not, clang++ -flto -Os fails:
      # https://reviews.llvm.org/D63976
      if("clang" STREQUAL "${CNINJA_COMPILER}")
        if("lto" IN_LIST CNINJA_FEATURES)
          set_cache(CMAKE_BUILD_TYPE Release)
        else()
          set_cache(CMAKE_BUILD_TYPE MinSizeRel)
        endif()
      else()
        set_cache(CMAKE_BUILD_TYPE MinSizeRel)
      endif()

      if("debugsyms" IN_LIST CNINJA_FEATURES)
        string(APPEND CMAKE_C_FLAGS_INIT " -g")
        string(APPEND CMAKE_CXX_FLAGS_INIT " -g")
      else()
        add_linker_flags(" -g0 ")
        if(NOT APPLE)
          add_linker_flags(" -Wl,-s -Wl,--strip-all")
        else()
          add_linker_flags(" -Wl,-S -Wl,-X")
        endif()
      endif()
    else()
      if("debugsyms" IN_LIST CNINJA_FEATURES)
        set_cache(CMAKE_BUILD_TYPE RelWithDebInfo)
      else()
        set_cache(CMAKE_BUILD_TYPE Release)
      endif()
    endif()
  endif()
endif()
