cninja_require(pre)
cninja_require(compiler=clang)
cninja_require(linker=lld)
cninja_require(gdbindex)
# cninja_require(libcxx) - not useful as of now as linux distros default to libstdc++
cninja_require(visibility)
cninja_require(linker)
cninja_require(linker-warnings)

# -pipe: Potentially makes the build faster
string(APPEND CMAKE_C_FLAGS_INIT " -pipe")
string(APPEND CMAKE_CXX_FLAGS_INIT " -pipe")

if(WIN32)
  # Remove obnoxious default <windows.h> features
  # - NOMINMAX: min() / max() macros
  # - _CRT_SECURE_NO_WARNINGS: secure versions are not portable, don't use them
  # - WIN32_LEAN_AND_MEAN: makes including <windows.h> faster
  string(APPEND CMAKE_C_FLAGS_INIT " -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN")
  string(APPEND CMAKE_CXX_FLAGS_INIT " -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN")
endif()

# Don't try to install in protected directories by default
set_cache(CMAKE_INSTALL_PREFIX install)

# Some libraries expect -fPIC
set_cache(CMAKE_POSITION_INDEPENDENT_CODE 1)

# Useful for running various tools, integrations in IDEs...
set_cache(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# We are in 2020
set_cache(CMAKE_CXX_STANDARD 20)

# Useful for avoiding rebuilds with IDEs which set this like QtCreator
set_cache(CMAKE_COLOR_DIAGNOSTICS 1)

# If you CI run looks like
# $ cmake --build .
# $ cmake --build . --target install
# this will make it faster:
set_cache(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY 1)
