#pragma once
#include <map>
#include <string>

namespace cn
{
// Two commands :
// - cninja_require(gcc) : will find and include gcc
// - cninja_optional(gcc) : will put after gcc if gcc is included

// arguments: %cmd-name%

// some useful default construction phases which will always be here:
// - start / compiler / ...
// - init
// - default
// - post
// - finish
// print them... # cninja begin: start

// all used feature names are added to the CNINJA_FEATURES variable
using builtin_map = std::map<std::string, std::string, std::less<>>;
const builtin_map& builtins();
}
