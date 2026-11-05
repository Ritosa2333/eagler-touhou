#pragma once
#include "Types.hpp"
namespace th10 {
struct CodecMemory {
    virtual u8* allocate_bytes(u32 bytes)=0;
    virtual void release_bytes(void* bytes)=0;
};
// Resource/save/replay byte permutation (0x44b0d0, 0x44b220).
u8* transform_resource(u8* data,i32 length,u8 key,u8 step,i32 block,i32 limit,bool encrypt,CodecMemory& memory);
// The original dictionary survives decoding calls and is not zeroed here.
i32 decode_lzss(const u8* source,u32 length,u8* destination,u32 capacity,u8* dictionary) noexcept;
struct LzssSearchNode {u32 parent,left,right;};
struct LzssSearch {
    LzssSearchNode* nodes;
    u8* dictionary;
    void initialize() noexcept;
    u32 insert(u32 index,u32& match) noexcept;
    void remove(u32 index) noexcept;
    void replace(u32 destination,u32 source) noexcept;
    void promote(u32 destination,u32 source) noexcept;
    u32 predecessor(u32 index) const noexcept;
};
u8* encode_lzss(const u8* source,i32 length,u32& packed_length,LzssSearch search,CodecMemory& memory);
}
