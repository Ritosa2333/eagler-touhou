#pragma once
#include "Types.hpp"
#include <string>
namespace th15 {
// Original score/performance captions promote an SSE float to double, then
// the VC2012 CRT rounds an exact decimal tie away from zero.
std::string legacy_decimal_tenth(float);
}
