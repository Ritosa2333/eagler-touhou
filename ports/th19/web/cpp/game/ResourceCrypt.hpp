#pragma once
#include "Types.hpp"
#include <vector>
namespace th19 {
struct CryptParams {u8 key,step;u32 block,limit;};
// th14+ keyset (thtk dattypes.h th14_crypt_params), indexed by (sum of name bytes) & 7.
// th19 uses variant 14, same as th14..th20.
bool resource_crypt(const u8* source,u8* destination,u32 size,CryptParams params,bool encrypt) noexcept;
inline CryptParams crypt_params_for(const char* name) noexcept {
    static constexpr CryptParams table[8]={
        {0x1b,0x73,0x100,0x3800},{0x12,0x43,0x200,0x3e00},
        {0x35,0x79,0x400,0x3c00},{0x03,0x91,0x80,0x6400},
        {0xab,0xdc,0x80,0x7000},{0x51,0x9e,0x100,0x4000},
        {0xc1,0x15,0x400,0x2c00},{0x99,0x7d,0x80,0x4400}};
    u32 index=0;while(*name)index+=u8(*name++);
    return table[index&7];
}
}
