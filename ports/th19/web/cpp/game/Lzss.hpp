#pragma once
#include "Types.hpp"
#include <vector>
namespace th19 {
// ZUN LZSS decoder (thtk thlzss.c). Dictionary contents survive decode calls,
// as in the original shared codec. Identical algorithm to th09's.
class Lzss {
public:
    u8 dictionary[8192]{};
    bool decode(const u8* input,u32 size,u8* output,u32 capacity,u32& written) noexcept;
};
}
