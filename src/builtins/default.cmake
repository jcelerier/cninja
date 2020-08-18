cninja_require(pre)
cninja_require(compiler=clang)
cninja_require(lld)
# cninja_require(libcxx) - not useful as of now as linux distros default to libstdc++
cninja_require(visibility)
cninja_require(linker)
cninja_require(linkerwarnings)

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
set(CMAKE_INSTALL_PREFIX install)

# Some libraries expect -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE 1)

# Useful for running various tools, integrations in IDEs...
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# We are in 2020
set(CMAKE_CXX_STANDARD 17)

# If you CI run looks like
# $ cmake --build .
# $ cmake --build . --target install
# this will make it faster:
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY 1)
