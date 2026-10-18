#include "EclProgram.hpp"
#include <algorithm>
namespace th08 {
bool EclProgram::has_instruction(const EclInstruction* instruction)const noexcept {
    const auto address=reinterpret_cast<std::uintptr_t>(instruction),base=reinterpret_cast<std::uintptr_t>(storage.data());
    if(address<base||address-base>=storage.size())return false;
    if(practice_instructions){
        // thprac replaces instructions with different lengths, including jump
        // destinations inside former instructions. Validate within the owning
        // subroutine, never against unrelated data or a timeline.
        const u32 offset=u32(address-base);if(offset&3)return false;
        for(u32 i=0;i<subs.size();i++){const u32 start=u32(reinterpret_cast<const u8*>(subs[i])-storage.data()),end=start+sub_lengths[i];
            if(offset>=start&&offset<end){if(end-offset<12)return false;EclInstruction header;std::memcpy(&header,storage.data()+offset,12);return header.size>=12&&!(header.size&3)&&u32(header.size)<=end-offset;}}
        return false;
    }
    return std::binary_search(instruction_offsets.begin(),instruction_offsets.end(),u32(address-base));
}
bool EclProgram::load(const u8* input,u32 size){
    release();if(!input||size<sizeof(EclHeader))return false;
    EclHeader header;std::memcpy(&header,input,sizeof(header));
    if(header.version!=0x800||header.sub_count<0||header.timeline_count<1||header.timeline_count>15)return false;
    const u32 table_end=72+u32(header.sub_count)*4;if(table_end>size)return false;
    std::vector<u32> positions,offsets(header.sub_count);if(!offsets.empty())std::memcpy(offsets.data(),input+72,offsets.size()*4);
    for(u32 i=0;i<=u32(header.timeline_count);++i){const u32 offset=header.timeline_offsets[i];if(offset<table_end||offset>size||(offset&3)||(i&&offset<=header.timeline_offsets[i-1]))return false;}
    for(u32 i=0;i<offsets.size();++i){const u32 start=offsets[i],end=i+1<offsets.size()?offsets[i+1]:header.timeline_offsets[0];
        if(start<table_end||(start&3)||end>size||end<=start)return false;bool terminated=false;
        for(u32 cursor=start;cursor<end;){
            if(end-cursor<sizeof(EclInstruction))return false;EclInstruction instruction;std::memcpy(&instruction,input+cursor,sizeof(instruction));
            if(instruction.size<12||(instruction.size&3)||u32(instruction.size)>end-cursor)return false;
            positions.push_back(cursor);cursor+=instruction.size;if(instruction.opcode==-1){terminated=true;break;}
        }if(!terminated)return false;
    }
    for(u32 i=0;i<u32(header.timeline_count);++i){const u32 start=header.timeline_offsets[i],end=header.timeline_offsets[i+1];bool terminated=false;
        for(u32 cursor=start;cursor<end;){
            if(end-cursor<sizeof(EclTimelineInstruction))return false;EclTimelineInstruction instruction;std::memcpy(&instruction,input+cursor,sizeof(instruction));
            if(instruction.time<0){terminated=true;break;}
            if(instruction.size<8||(instruction.size&3)||u32(instruction.size)>end-cursor)return false;cursor+=instruction.size;
        }if(!terminated)return false;
    }
    storage.assign(input,input+size);instruction_offsets=std::move(positions);subs.reserve(offsets.size());sub_lengths.reserve(offsets.size());
    for(u32 i=0;i<offsets.size();++i){subs.push_back(reinterpret_cast<EclInstruction*>(storage.data()+offsets[i]));sub_lengths.push_back((i+1<offsets.size()?offsets[i+1]:header.timeline_offsets[0])-offsets[i]);}
    for(u32 i=0;i<u32(header.timeline_count);++i){timelines[i]=reinterpret_cast<EclTimelineInstruction*>(storage.data()+header.timeline_offsets[i]);timeline_lengths[i]=header.timeline_offsets[i+1]-header.timeline_offsets[i];}
    return true;
}
}
