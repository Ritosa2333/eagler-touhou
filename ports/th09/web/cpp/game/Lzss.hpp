#pragma once
#include "Types.hpp"
#include <vector>
namespace th09 {
// Cooperative decoder used only by preloading. Each step stops at a token
// boundary, retaining the exact dictionary and bit position for the next step.
class LzssStream {
    u8 dictionary[8192]{};u32 cursor=0,head=1,written=0;u8 mask=0x80,byte=0;bool complete=false,failed=false;
public:
    bool step(const u8*,u32,u8*,u32,u32 budget=131072) noexcept;
    bool done()const{return complete;}bool valid()const{return !failed;}u32 size()const{return written;}
};
// PBGZ/replay codec. Adapted from the existing MIT-derived TH08 implementation.
// TH09 1.50a oracle addresses: decode 0x433aa0, encode 0x434020.
// Dictionary contents survive decode calls, as in the original shared codec.
class Lzss {
public:
    u8 dictionary[8192]{};
    bool decode(const u8* input,u32 size,u8* output,u32 capacity,u32& written) noexcept;
    std::vector<u8> encode(const u8* input,u32 size);
private:
    struct Node {i32 parent=0,left=0,right=0;} tree[8193];
    i32 add(i32 node,i32& position) noexcept;
    void erase(i32 node) noexcept;
    void contract(i32 old_node,i32 new_node) noexcept;
    void replace(i32 old_node,i32 new_node) noexcept;
};
}
