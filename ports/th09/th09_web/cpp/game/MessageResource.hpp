#pragma once
#include "Binary.hpp"
#include "Rng.hpp"
#include <string>
#include <vector>
namespace th09 {
struct MessageEntry {u32 offset=0,weight=0;};
struct MessageInstruction {
    u16 time=0;u8 opcode=0,size=0;const u8* arguments=nullptr;
    i16 short_value(u32 n)const noexcept{return n*2+2<=size?i16(read16(arguments+n*2)):0;}
    i32 integer(u32 n)const noexcept{return n*4+4<=size?signed_bits(read32(arguments+n*4)):0;}
};
class MessageResource {
    std::vector<u8> bytes;
public:
    std::vector<MessageEntry> entries;
    bool load(const u8*,u32 size);
    bool instruction(u32 offset,MessageInstruction&)const noexcept;
    static bool decode_text(const MessageInstruction&,u32 start,std::string&);
    i32 victory_script(i32 opponent,Rng&)const;
    std::vector<i32> music_for(i32 first,i32 count)const;
};
}
