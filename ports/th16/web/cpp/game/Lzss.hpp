// th16 LZSS——原版解码 0x457f00 / 编码 0x457b20（档案条目、目录表与回放载荷共用）。
// 13 位字典偏移、4 位长度(+3)、8192 字节字典，字典游标从 1 起步；偏移 0 为终止符。
// 位读取越过输入末尾时以 0 填充（与原版缺省终止符的行为一致）。
// 编码器改编自 th08/th15 同族 MIT 实现，与原版字典树搜索语义对齐。
#pragma once
#include "Types.hpp"

#include <vector>

namespace th16 {

class Lzss {
public:
    u8 dictionary[8192]{};
    bool decode(const u8* input, u32 size, u8* output, u32 capacity, u32& written) noexcept;
    std::vector<u8> encode(const u8* input, u32 size);

private:
    const u8* encoding = nullptr;
    u32 encoding_size = 0, cursor = 0, head = 1;
    i32 available = 0, length = 0, position = 0;
    u8 byte = 0, mask = 0x80;
    bool encoding_done = true;
    std::vector<u8> output;

    void begin_encode(const u8* input, u32 size);
    bool step_encode(u32 budget);
    void encode_bit(bool value);
    void encode_bits(u32 value, u32 count);

    struct Node {
        i32 parent = 0, left = 0, right = 0;
    } tree[8193];
    i32 add(i32 node, i32& match_pos) noexcept;
    void erase(i32 node) noexcept;
    void contract(i32 old_node, i32 new_node) noexcept;
    void replace(i32 old_node, i32 new_node) noexcept;
};

}  // namespace th16
