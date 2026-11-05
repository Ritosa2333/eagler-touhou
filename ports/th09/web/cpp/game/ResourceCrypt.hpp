#pragma once
#include "Types.hpp"
#include <vector>
namespace th09 {
struct CryptParams {u8 key,step;u32 block,limit;};
// Source and destination must not overlap. Limits are in original whole blocks.
bool resource_crypt(const u8* source,u8* destination,u32 size,CryptParams params,bool encrypt) noexcept;
bool resource_unwrap(std::vector<u8>& bytes);
}
