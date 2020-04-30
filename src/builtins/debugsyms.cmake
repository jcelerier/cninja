# Enable generation of debug symbols

# -g3 allows debugging into macros unlike the default of -g
string(APPEND CMAKE_C_FLAGS_INIT " -g3")
string(APPEND CMAKE_CXX_FLAGS_INIT " -g3")

# TODO : gdb index ?
