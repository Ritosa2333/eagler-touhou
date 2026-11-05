#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

namespace th075 {
using u8=std::uint8_t; using i8=std::int8_t;
using u16=std::uint16_t; using i16=std::int16_t;
using u32=std::uint32_t; using i32=std::int32_t;
struct ByteReader {
    const u8* bytes=nullptr; std::size_t size=0, offset=0; bool valid=true;
    ByteReader(const void* p,std::size_t n):bytes(static_cast<const u8*>(p)),size(n){}
    bool available(std::size_t n)const{return offset<=size&&n<=size-offset;}
    const u8* take(std::size_t n){if(!available(n)){valid=false;return nullptr;}const auto* p=bytes+offset;offset+=n;return p;}
    u8 byte(){const auto* p=take(1);return p?*p:0;}
    u16 word(){const auto* p=take(2);return p?static_cast<u16>(p[0]|(u16(p[1])<<8)):0;}
    u32 dword(){const auto* p=take(4);return p?u32(p[0])|(u32(p[1])<<8)|(u32(p[2])<<16)|(u32(p[3])<<24):0;}
};
inline i32 increment_wrapped(i32 v,int direction){const u32 bits=u32(v)+u32(direction);i32 result;std::memcpy(&result,&bits,4);return result;}
inline i32 subtract_wrapped(i32 value,i32 amount){const u32 bits=u32(value)-u32(amount);i32 result;std::memcpy(&result,&bits,4);return result;}
}
