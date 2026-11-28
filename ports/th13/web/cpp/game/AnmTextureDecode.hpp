#pragma once
#include "Types.hpp"
#include <vector>
namespace th13 {
bool anm_decode_png(const u8* bytes, u32 size, u32& width, u32& height, std::vector<u8>& rgba);
}
