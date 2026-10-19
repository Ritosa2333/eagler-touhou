#pragma once
#include "Types.hpp"
namespace th09 {
inline u16 read16(const u8* p) noexcept {return u16(p[0])|(u16(p[1])<<8);}
inline u32 read32(const u8* p) noexcept {return u32(p[0])|(u32(p[1])<<8)|(u32(p[2])<<16)|(u32(p[3])<<24);}
inline void write16(u8* p,u16 v) noexcept {p[0]=u8(v);p[1]=u8(v>>8);}
inline void write32(u8* p,u32 v) noexcept {for(u32 i=0;i<4;++i)p[i]=u8(v>>(8*i));}
inline constexpr u32 fourcc(char a,char b,char c,char d) noexcept {return u32(u8(a))|(u32(u8(b))<<8)|(u32(u8(c))<<16)|(u32(u8(d))<<24);}
}
