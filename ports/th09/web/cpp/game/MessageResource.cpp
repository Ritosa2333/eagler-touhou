#include "MessageResource.hpp"
#include <algorithm>
namespace th09 {
bool MessageResource::instruction(u32 offset,MessageInstruction& out)const noexcept{
    if(offset>bytes.size()||bytes.size()-offset<4)return false;const u8* p=bytes.data()+offset;
    if(bytes.size()-offset-4<p[3])return false;out={read16(p),p[2],p[3],p+4};return true;
}
bool MessageResource::load(const u8* data,u32 size){
    bytes.clear();entries.clear();if(!data||size<4)return false;const u32 count=read32(data);
    if(count>4096||4+u64(count)*8>size)return false;bytes.assign(data,data+size);
    for(u32 i=0;i<count;++i){MessageEntry e{read32(data+4+i*8),read32(data+8+i*8)};
        if(e.offset){if(e.offset<4+count*8||e.offset>=size){bytes.clear();entries.clear();return false;}
            MessageInstruction ins;u32 offset=e.offset;bool ended=false;while(instruction(offset,ins)){if(ins.opcode==0){ended=true;break;}offset+=4+ins.size;}
            if(!ended){bytes.clear();entries.clear();return false;}}
        entries.push_back(e);
    }return true;
}
bool MessageResource::decode_text(const MessageInstruction& i,u32 start,std::string& out){
    out.clear();u8 key=0x77,step=7;for(u32 n=start;n<i.size;++n){u8 ch=i.arguments[n]^key;if(!ch)return true;out.push_back(char(ch));key=u8(key+step);step=u8(step+16);}return false;
}
i32 MessageResource::victory_script(i32 opponent,Rng& rng)const{
    const auto choose=[&](i32 begin,i32 end){i32 weight=rng.below(256);for(i32 i=begin;i<end;++i)if(i>=0&&u32(i)<entries.size()&&entries[i].offset){if(weight<signed_bits(entries[i].weight))return i;weight=wrapping_sub(weight,signed_bits(entries[i].weight));}return -1;};
    i32 result=choose(opponent*10,opponent*10+10);if(result>=0)return result;result=choose(210,220);return result<0?210:result;
}
std::vector<i32> MessageResource::music_for(i32 first,i32 count)const{
    std::vector<i32> tracks;for(i32 n=first;n<first+count&&n>=0&&u32(n)<entries.size();++n){u32 off=entries[n].offset;MessageInstruction i;if(!off)continue;
        while(instruction(off,i)&&i.opcode){if(i.opcode==7){i32 track=i.integer(0);if(std::find(tracks.begin(),tracks.end(),track)==tracks.end())tracks.push_back(track);}off+=4+i.size;}}
    return tracks;
}
}
