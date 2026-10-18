#include "EclProgram.hpp"
#include "Binary.hpp"
#include <algorithm>
namespace th09 {
void EclProgram::release(){storage.clear();subs.clear();timelines.clear();sub_lengths.clear();timeline_lengths.clear();instructions.clear();}
EclInstruction* EclProgram::sub(i32 index) noexcept{return index>=0&&u32(index)<subs.size()?reinterpret_cast<EclInstruction*>(storage.data()+subs[index]):nullptr;}
EclTimelineInstruction* EclProgram::timeline(i32 index) noexcept{return index>=0&&u32(index)<timelines.size()?reinterpret_cast<EclTimelineInstruction*>(storage.data()+timelines[index]):nullptr;}
bool EclProgram::has_instruction(const EclInstruction* p) const noexcept {
    const auto a=reinterpret_cast<std::uintptr_t>(p),base=reinterpret_cast<std::uintptr_t>(storage.data());
    return a>=base&&a-base<storage.size()&&std::binary_search(instructions.begin(),instructions.end(),u32(a-base));
}
bool EclProgram::load(const u8* data,u32 size){
    release();if(!data||size<8||read32(data)!=0x900)return false;
    const u32 sub_count=read16(data+4),timeline_count=read16(data+6),table_end=8+4*(sub_count+timeline_count);
    if(table_end>size||timeline_count>16||sub_count>32767)return false;
    std::vector<u32> s(sub_count),t(timeline_count),sl,tl,positions;
    for(u32 i=0;i<timeline_count;++i)t[i]=read32(data+8+i*4);
    for(u32 i=0;i<sub_count;++i)s[i]=read32(data+8+(timeline_count+i)*4);
    const auto region_end=[&](u32 start){u32 end=size;for(u32 offset:s)if(offset>start)end=std::min(end,offset);for(u32 offset:t)if(offset>start)end=std::min(end,offset);return end;};
    for(u32 offset:s){
        if(offset<table_end||offset>=size||(offset&3))return false;
        const u32 end=region_end(offset);bool terminated=false;
        for(u32 cursor=offset;cursor<end;){
            if(end-cursor<12)return false;EclInstruction i;std::memcpy(&i,data+cursor,12);
            if(i.size<12||(i.size&3)||u32(i.size)>end-cursor)return false;
            positions.push_back(cursor);cursor+=i.size;if(i.opcode==-1){terminated=true;break;}
        }
        if(!terminated)return false;sl.push_back(end-offset);
    }
    for(u32 offset:t){
        if(offset<table_end||offset>=size||(offset&3))return false;
        const u32 end=region_end(offset);bool terminated=false;
        for(u32 cursor=offset;cursor<end;){
            if(end-cursor<8)return false;EclTimelineInstruction i;std::memcpy(&i,data+cursor,8);
            if(i.time<0){terminated=true;break;}
            if(i.size<8||(i.size&3)||u32(i.size)>end-cursor)return false;cursor+=i.size;
        }
        if(!terminated)return false;tl.push_back(end-offset);
    }
    storage.assign(data,data+size);subs=std::move(s);timelines=std::move(t);sub_lengths=std::move(sl);timeline_lengths=std::move(tl);
    std::sort(positions.begin(),positions.end());instructions=std::move(positions);return true;
}
}
