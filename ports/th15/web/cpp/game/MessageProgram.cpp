#include "MessageProgram.hpp"
namespace th15 {
std::string MessageInstruction::text()const{std::string out;u8 key=0x77,step=7;for(u8 byte:payload){byte^=key;key+=step;step+=0x10;if(!byte)break;out.push_back(char(byte));}return out;}
bool MessageProgram::open(const u8* data,u32 size){
    scripts.clear();error.clear();auto fail=[&](const char* s){scripts.clear();error=s;return false;};
    if(!data||size<4)return fail("Truncated MSG directory");u32 count;std::memcpy(&count,data,4);if(count>(size-4)/8)return fail("Invalid MSG directory size");
    for(u32 i=0;i<count;i++){u32 offset,flag;std::memcpy(&offset,data+4+i*8,4);std::memcpy(&flag,data+8+i*8,4);if(offset<4+count*8||offset>size)return fail("Invalid MSG script offset");
        MessageScript script;script.flag=flag;bool ended=false;while(offset+4<=size){MessageInstruction c;c.offset=offset;std::memcpy(&c.time,data+offset,2);c.opcode=data[offset+2];const u32 n=data[offset+3];if(n>size-offset-4)return fail("Truncated MSG instruction");c.payload.assign(data+offset+4,data+offset+4+n);script.instructions.push_back(std::move(c));offset+=4+n;if(script.instructions.back().opcode==0){ended=true;break;}}
        if(!ended)return fail("Missing MSG terminator");scripts.push_back(std::move(script));
    }return true;
}
}
