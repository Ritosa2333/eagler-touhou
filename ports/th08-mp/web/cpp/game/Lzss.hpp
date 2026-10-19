#pragma once
#include "Types.hpp"
#include <vector>
namespace th08 {
// TH08 PBG/replay codec. Adapted from the MIT reference, checked against 1.00d.
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
