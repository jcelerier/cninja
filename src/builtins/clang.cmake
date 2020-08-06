# Use clang as a compiler (default)
cninja_require(pre)
set(CNINJA_COMPILER clang)

find_program(CMAKE_C_COMPILER NAMES
  clang-20 clang-19 clang-18 clang-17 clang-16
  clang-15 clang-14 clang-13 clang-12 clang-11
  clang-10 clang-9  clang-8  clang-7  clang-6
  clang
)
find_program(CMAKE_CXX_COMPILER NAMES
  clang++-20 clang++-19 clang++-18 clang++-17 clang++-16
  clang++-15 clang++-14 clang++-13 clang++-12 clang++-11
  clang++-10 clang++-9  clang++-8  clang++-7  clang++-6
  clang++
)
