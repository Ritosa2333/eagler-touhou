#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace th17::multiplayer {
// Snapshot identity includes every byte, including dormant fields and padding.
// memcpy permits unaligned objects and keeps the loads free of aliasing UB.
// Fixed-size 32-byte groups avoid the byte-order work of a general memcmp.
template<std::size_t Size> inline bool snapshot_equal(const void* a,const void* b)noexcept{
    const auto* first=static_cast<const unsigned char*>(a);
    const auto* second=static_cast<const unsigned char*>(b);
    std::size_t at=0;
    for(;at+32<=Size;at+=32){
        std::uint64_t different=0;
        for(std::size_t offset=0;offset<32;offset+=8){
            std::uint64_t x,y;
            std::memcpy(&x,first+at+offset,sizeof x);
            std::memcpy(&y,second+at+offset,sizeof y);
            different|=x^y;
        }
        if(different)return false;
    }
    for(;at<Size;++at)if(first[at]!=second[at])return false;
    return true;
}
}
