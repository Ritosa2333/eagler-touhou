#pragma once
#include "Types.hpp"
#include <string>
#include <vector>
namespace th19 {
// ECL SCPT container (thtk thecl10.c, th10+; th19 uses the th13 engine).
struct EclInstruction {
    u32 time=0;u16 id=0,size=0,param_mask=0;u8 rank_mask=0,param_count=0;u32 zero=0;
    u32 offset=0,param_bytes=0;
};
struct EclSub {
    std::string name;u32 offset=0,instr_bytes=0;
};
struct EclProgram {
    std::vector<std::string> anims;   // ANIM list
    std::vector<std::string> includes; // ECLI list
    std::vector<EclSub> subs;
    std::vector<u8> bytecode;          // concatenated sub streams (sub.offset indexes here)
    bool load(const u8* data,u32 size);
private:
    u32 read_u32(const u8* b) const noexcept {return u32(b[0])|(u32(b[1])<<8)|(u32(b[2])<<16)|(u32(b[3])<<24);}
    u16 read_u16(const u8* b) const noexcept {return u16(b[0])|(u16(b[1])<<8);}
};
// Decode one sub's instruction stream into instructions (th13+ layout).
// time u32, id u16, size u16, param_mask u16, rank_mask u8, param_count u8, zero u32, then size-16 params.
void decode_instructions(const u8* stream,u32 len,std::vector<EclInstruction>& out);
}
