#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <vector>

namespace th10::adaptation {
struct SourceBytes {
    const std::uint8_t* data=nullptr;std::uint32_t size=0;
    bool has(std::uint32_t offset,std::uint32_t count)const noexcept{return data&&offset<=size&&count<=size-offset;}
    std::uint32_t u32(std::uint32_t at)const noexcept{std::uint32_t v=0;if(has(at,4))std::memcpy(&v,data+at,4);return v;}
    std::uint16_t u16(std::uint32_t at)const noexcept{std::uint16_t v=0;if(has(at,2))std::memcpy(&v,data+at,2);return v;}
    std::uint64_t fnv()const noexcept{std::uint64_t h=14695981039346656037ull;for(std::uint32_t i=0;i<size;++i){h^=data[i];h*=1099511628211ull;}return h;}
};
struct DialogueSources {
    using Row=std::array<std::uint32_t,10>;
    std::uint32_t reason=7,subroutines=0,entries=0;
    std::vector<Row> candidates;
};
// Read only original, already loaded SCPT and MSG bytes. Candidate offsets
// address the real 0x152 instruction and a real skippable MSG opcode-10 wait.
inline DialogueSources dialogue_sources(SourceBytes ecl,SourceBytes msg,std::uint32_t difficulty){
    using Row=DialogueSources::Row;
    DialogueSources out;
    if(!ecl.has(0,0x24)||ecl.u32(0)!=0x54504353||ecl.u16(4)!=1||!msg.has(0,4))return out;
    out.subroutines=ecl.u16(0x10);out.entries=msg.u32(0);
    const std::uint32_t table=0x24+ecl.u16(6);
    if(!out.subroutines||out.subroutines>8192||!ecl.has(table,out.subroutines*4)||
       !out.entries||out.entries>4096||!msg.has(4,out.entries*8))return out;
    std::vector<std::uint32_t> offsets;offsets.reserve(out.subroutines);
    for(std::uint32_t i=0;i<out.subroutines;++i){const auto p=ecl.u32(table+i*4);if(p<table+out.subroutines*4||!ecl.has(p,16))return out;offsets.push_back(p);}
    std::vector<Row> references;
    for(std::uint32_t sub=0;sub<out.subroutines;++sub){
        const auto start=offsets[sub];auto end=ecl.size;
        for(const auto p:offsets)if(p>start)end=std::min(end,p);
        std::uint32_t cursor=start+16,count=0;
        while(cursor<end){
            if(end-cursor<16)return out;
            const auto length=ecl.u16(cursor+6);if(length<16||length>end-cursor||++count>100000)return out;
            const auto opcode=ecl.u16(cursor+4),refs=ecl.u16(cursor+8);const auto bits=ecl.data[cursor+10];
            if(opcode==0x152&&length>=20&&!(refs&1)&&ecl.data[cursor+11]>=1&&(bits&difficulty)){
                const auto entry=ecl.u32(cursor+16);
                if(entry>=out.entries)return out;
                auto found=std::find_if(references.begin(),references.end(),[&](const Row& r){return r[0]==entry;});
                if(found==references.end())references.push_back({entry,sub,ecl.u32(cursor),bits,cursor,1,0,0,0,0});
                else ++(*found)[5];
            }
            cursor+=length;
        }
    }
    for(auto row:references){
        const auto start=msg.u32(4+row[0]*8);if(start<4+out.entries*8||!msg.has(start,4))return out;
        auto end=msg.size;for(std::uint32_t i=0;i<out.entries;++i){const auto p=msg.u32(4+i*8);if(p>start)end=std::min(end,p);}
        std::uint32_t cursor=start,count=0;bool skippable=false,ended=false,waitFound=false;
        while(cursor<end){
            if(end-cursor<4||++count>100000)return out;
            const auto opcode=msg.data[cursor+2],length=msg.data[cursor+3];
            if(opcode>23||length>end-cursor-4)return out;
            if(opcode==9){if(length<1)return out;skippable=(msg.data[cursor+4]&1)!=0;}
            if(opcode==10){
                if(length<4)return out;const auto timeout=msg.u32(cursor+4);
                if(!waitFound&&skippable&&timeout>0&&timeout<0x80000000u){row[6]=cursor;row[7]=msg.u16(cursor);row[8]=timeout;waitFound=true;}
            }
            if(opcode==0){ended=true;break;}
            cursor+=4+length;
        }
        if(!ended)return out;
        if(waitFound){row[9]=count;out.candidates.push_back(row);if(out.candidates.size()>32){out.reason=8;out.candidates.clear();return out;}}
    }
    out.reason=out.candidates.empty()?9:0;return out;
}
}
