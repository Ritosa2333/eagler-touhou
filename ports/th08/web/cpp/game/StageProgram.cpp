#include "StageProgram.hpp"
#include <algorithm>
namespace th08 {
bool StageProgram::load(const u8* bytes,u32 size){
    clear();if(!bytes||size<sizeof(StageHeader)+20||size>16*1024*1024)return false;
    StageHeader h;std::memcpy(&h,bytes,sizeof(h));
    if(h.object_count<0||h.quad_count<0||u32(h.object_count)>(size-sizeof(h))/4)return false;
    const u32 first=sizeof(h)+u32(h.object_count)*4;
    if(h.instances_offset<first||h.instances_offset>size-16||h.script_offset<h.instances_offset+16||h.script_offset>size-20||(h.instances_offset&3)||(h.script_offset&3))return false;
    std::vector<u32> offsets(h.object_count);if(!offsets.empty())std::memcpy(offsets.data(),bytes+sizeof(h),offsets.size()*4);
    auto sorted=offsets;std::sort(sorted.begin(),sorted.end());u32 quads=0;
    for(u32 i=0;i<sorted.size();++i){
        const u32 start=sorted[i],end=i+1<sorted.size()?sorted[i+1]:h.instances_offset;
        if(start<first||end>h.instances_offset||start>end||end-start<32||(start&3))return false;
        u32 p=start+28;bool ended=false;
        while(p<=end-4){i16 type,length;std::memcpy(&type,bytes+p,2);std::memcpy(&length,bytes+p+2,2);
            if(type<0){ended=true;break;}
            if(length<8||u32(length)>end-p||(length&3)||(type==0&&length<28)||(type==1&&length<36))return false;
            i16 script;std::memcpy(&script,bytes+p+4,2);if(script<0)return false;
            ++quads;p+=length;
        }
        if(!ended)return false;
    }
    if(quads!=u32(h.quad_count))return false;
    u32 placements=0;bool ended=false;
    for(u32 p=h.instances_offset;p<=h.script_offset-16;p+=16){i16 object;std::memcpy(&object,bytes+p,2);if(object<0){ended=true;break;}if(object>=h.object_count)return false;++placements;}
    if(!ended)return false;
    u32 instructions=0;ended=false;
    for(u32 p=h.script_offset;p<=size-20;p+=20){StageInstruction ins;std::memcpy(&ins,bytes+p,20);++instructions;if(ins.frame==-1){ended=true;break;}if(ins.size!=12)return false;}
    if(!ended)return false;
    for(u32 i=0;i<instructions;++i){StageInstruction ins;std::memcpy(&ins,bytes+h.script_offset+i*20,20);if(ins.opcode==4&&(ins.integer(0)<0||ins.args[0]>=instructions))return false;}
    data.assign(bytes,bytes+size);for(u32 offset:offsets)object_list.push_back(reinterpret_cast<StageObject*>(data.data()+offset));
    instruction_count=instructions;instance_count=placements;return true;
}
}
