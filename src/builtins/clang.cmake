# Use clang as a compiler (default)
cninja_require(pre)
set(CNINJA_COMPILER clang)

find_program(CMAKE_C_COMPILER NAMES
  clang-25 clang-24 clang-23 clang-22 clang-21 
  clang-20 clang-19 clang-18 clang-17 clang-16
  clang
)
find_program(CMAKE_CXX_COMPILER NAMES
  clang++-25 clang++-24 clang++-23 clang++-22 clang++-21 
  clang++-20 clang++-19 clang++-18 clang++-17 clang++-16
  clang++
)
