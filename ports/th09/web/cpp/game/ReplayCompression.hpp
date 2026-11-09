#pragma once
#include "Types.hpp"
#include <vector>

namespace th09::replay_compression {
std::vector<u8> deflate(const u8* data, u32 size);
bool inflate(const u8* data, u32 size, u8* output, u32 output_size);

inline void append_varint(std::vector<u8>& out,u32 value){
    while(value>=128){out.push_back(u8(value)|128);value>>=7;}
    out.push_back(u8(value));
}
inline bool read_varint(const u8*& cursor,const u8* end,u32& value){
    value=0;
    for(u32 shift=0;cursor!=end;shift+=7){
        const u8 part=*cursor++;
        if(shift==28&&part>15)return false;
        value|=u32(part&127)<<shift;
        if(!(part&128))return true;
    }
    return false;
}
}
