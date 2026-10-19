// Runtime override entry point for the thcrap-style offline pack. Every game
// file read may be satisfied from /thcrap/th09/<relative> first; the original
// DAT remains the fallback. A localization-disabled build never looks at the
// override namespace, keeping the strict Japanese regression baseline.
#pragma once
#include "Types.hpp"
#include <vector>
namespace th09 {
namespace RuntimeOverride {
bool Read(const char* relative,std::vector<u8>& out);
}
}

