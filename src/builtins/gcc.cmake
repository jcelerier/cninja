# Use gcc as a compiler
set(CNINJA_COMPILER gcc)

find_program(CMAKE_C_COMPILER NAMES
  gcc-20 gcc-19 gcc-18 gcc-17 gcc-16
  gcc-15 gcc-14 gcc-13 gcc-12 gcc-11
  gcc-10 gcc-9  gcc-8  gcc-7  gcc-6
  gcc
)
find_program(CMAKE_CXX_COMPILER NAMES
  g++-20 g++-19 g++-18 g++-17 g++-16
  g++-15 g++-14 g++-13 g++-12 g++-11
  g++-10 g++-9  g++-8  g++-7  g++-6
  g++
)
