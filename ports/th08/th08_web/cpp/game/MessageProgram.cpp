#include "MessageProgram.hpp"
#include <algorithm>
namespace th08 {
bool MessageProgram::load(const u8* source,u32 size){
    clear();if(!source||size<8||size>16*1024*1024)return false;u32 count;std::memcpy(&count,source,4);if(!count||count>(size-4)/4||count>4096)return false;
    std::vector<u32> entries(count);std::memcpy(entries.data(),source+4,count*4);auto sorted=entries;std::sort(sorted.begin(),sorted.end());sorted.erase(std::unique(sorted.begin(),sorted.end()),sorted.end());
    if(sorted.front()<4+count*4||sorted.back()>size-4)return false;
    for(u32 i=0;i<sorted.size();++i){u32 p=sorted[i],end=i+1<sorted.size()?sorted[i+1]:size;
        while(p<end){if(end-p<4)return false;const u8 op=source[p+2],length=source[p+3];if(length>end-p-4)return false;
            constexpr u8 minimum[]={0,4,4,5,4,3,0,4,0,0,0,0,0,1,0,20,1,8,1,1,1,4,0};
            if(op<sizeof(minimum)&&length<minimum[op])return false;
            if(op==3||op==16||op==19||op==20){char text_buffer[256];const u32 prefix=op==3?4:0;if(!text(source+p+4+prefix,length-prefix,text_buffer,sizeof(text_buffer)))return false;}
            p+=4+length;
        }
    }
    data.assign(source,source+size);offsets=std::move(entries);return true;
}
const u8* MessageProgram::entry(i32 index)const{return index>=0&&u32(index)<offsets.size()?data.data()+offsets[index]:nullptr;}
bool MessageProgram::instruction(const u8* pointer,MessageInstruction& output)const{
    const auto base=reinterpret_cast<std::uintptr_t>(data.data()),p=reinterpret_cast<std::uintptr_t>(pointer);if(p<base||p-base>data.size()||data.size()-(p-base)<4)return false;
    std::memcpy(&output,pointer,4);return output.size<=data.size()-(p-base)-4;
}
bool MessageProgram::text(const u8* source,u32 available,char* output,u32 capacity){
    if(!source||!output||!capacity)return false;for(u32 i=0;i<available&&i<capacity;++i){output[i]=char(source[i]^0x77);if(!output[i])return true;}output[capacity-1]=0;return false;
}
}
