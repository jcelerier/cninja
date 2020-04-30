# Enable ThreadSanitizer. Incompatible with asan / ubsan.
cninja_optional(asan)
if(NOT "asan" IN_LIST CNINJA_FEATURES)
  string(APPEND CMAKE_C_FLAGS_INIT " -fsanitize=thread")
  string(APPEND CMAKE_CXX_FLAGS_INIT " -fsanitize=thread")
else()
  message(" -- Warning! ASAN and TSAN are incompatible. Disabling TSAN.\n")
endif()

