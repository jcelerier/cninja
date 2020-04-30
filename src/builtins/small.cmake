# Creates a small build
cninja_require(pre)
cninja_require(lto=full)
cninja_optional(debugsyms)
cninja_optional(compiler)
cninja_optional(lld)

if("lld" STREQUAL "${CNINJA_LINKER}")
  add_linker_flags(" -Wl,--icf=all")
endif()

add_linker_flags(" -Wl,-O3")

if(NOT "debugsyms" IN_LIST CNINJA_FEATURES)
  add_linker_flags(" -Wl,--strip-all")
endif()
