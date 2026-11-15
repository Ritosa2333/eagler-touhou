#include "EclProgram.hpp"
namespace th19 {
void decode_instructions(const u8* stream,u32 len,std::vector<EclInstruction>& out) {
    out.clear();
    u32 off=0;
    while(off+16<=len){
        EclInstruction ins;
        ins.time=u32(stream[off])|(u32(stream[off+1])<<8)|(u32(stream[off+2])<<16)|(u32(stream[off+3])<<24);
        ins.id=u16(stream[off+4])|(u16(stream[off+5])<<8);
        ins.size=u16(stream[off+6])|(u16(stream[off+7])<<8);
        ins.param_mask=u16(stream[off+8])|(u16(stream[off+9])<<8);
        ins.rank_mask=stream[off+10];
        ins.param_count=stream[off+11];
        ins.zero=u32(stream[off+12])|(u32(stream[off+13])<<8)|(u32(stream[off+14])<<16)|(u32(stream[off+15])<<24);
        if(ins.size<16){return;} // corrupt
        ins.offset=off;
        ins.param_bytes=ins.size-16;
        out.push_back(ins);
        off+=ins.size;
        if(off>len)return;
    }
}
static std::string sjis_name(const u8* b,u32 n) {
    std::string s;for(u32 i=0;i<n&&b[i];++i)s+=char(b[i]);return s;
}
bool EclProgram::load(const u8* data,u32 size) {
    anims.clear();includes.clear();subs.clear();bytecode.clear();
    if(size<20||data[0]!='S'||data[1]!='C'||data[2]!='P'||data[3]!='T')return false;
    const u32 include_length=read_u16(data+6);
    const u32 include_offset=read_u32(data+8);
    const u32 sub_count=read_u32(data+16);
    if(include_offset+4>size||sub_count>100000)return false;
    // ANIM list
    const u8* p=data+include_offset;
    if(p[0]!='A'||p[1]!='N'||p[2]!='I'||p[3]!='M')return false;
    const u32 anim_count=read_u32(p+4);p+=8;
    for(u32 i=0;i<anim_count;++i){const u8* s=p;while(p<data+size&&*p)++p;anims.push_back(sjis_name(s,u32(p-s)));p++;if(p>data+size)return false;}
    p=(const u8*)((uintptr_t(p)+3)&~uintptr_t(3));
    // ECLI list
    if(p[0]!='E'||p[1]!='C'||p[2]!='L'||p[3]!='I')return false;
    const u32 incl_count=read_u32(p+4);p+=8;
    for(u32 i=0;i<incl_count;++i){const u8* s=p;while(p<data+size&&*p)++p;includes.push_back(sjis_name(s,u32(p-s)));p++;if(p>data+size)return false;}
    p=(const u8*)((uintptr_t(p)+3)&~uintptr_t(3));
    // sub offset table
    std::vector<u32> offsets;offsets.reserve(sub_count);
    for(u32 i=0;i<sub_count;++i){if(uintptr_t(p)+4>uintptr_t(data+size))return false;offsets.push_back(read_u32(p));p+=4;}
    // sub names
    std::vector<std::string> names;names.reserve(sub_count);
    for(u32 i=0;i<sub_count;++i){const u8* s=p;while(p<data+size&&*p)++p;names.push_back(sjis_name(s,u32(p-s)));p++;if(p>data+size)return false;}
    // sub streams (ECLH blocks) — copy each instruction stream into bytecode,
    // with sub.offset indexing into bytecode.
    bytecode.clear();
    for(u32 i=0;i<sub_count;++i){
        const u32 off=offsets[i];
        if(off+8>size)return false;
        if(data[off]!='E'||data[off+1]!='C'||data[off+2]!='L'||data[off+3]!='H')return false;
        const u32 data_offset=read_u32(data+off+4); // ==16
        const u32 end=(i+1<sub_count)?offsets[i+1]:size;
        if(off+data_offset>end)return false;
        EclSub sub;sub.name=names[i];sub.offset=u32(bytecode.size());sub.instr_bytes=end-(off+data_offset);
        bytecode.insert(bytecode.end(),data+off+data_offset,data+end);
        subs.push_back(std::move(sub));
    }
    return !subs.empty();
}
}
