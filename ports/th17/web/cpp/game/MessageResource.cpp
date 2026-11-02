#include "MessageResource.hpp"
namespace th17 {
namespace {template<class T>T read(const u8* source){T value;std::memcpy(&value,source,sizeof(T));return value;}}
bool MessageResource::decode(const u8* source,u32 size,std::string& result){
    if(!source)return false;std::string decoded;u8 key=0x77,step=7;bool ended=false;
    for(u32 i=0;i<size;i++){const u8 value=source[i]^key;key=u8(key+step);step=u8(step+16);if(!value){ended=true;break;}decoded.push_back(char(value));}
    if(!ended)return false;
    for(u32 i=0;i<decoded.size();i++){const u8 value=u8(decoded[i]);if((value>=0x81&&value<=0x9f)||u8(value+0x20)<=0x1c){if(i+1>=decoded.size())return false;i++;}else if(value=='_')decoded[i]=' ';}
    result=std::move(decoded);return true;
}
bool MessageResource::open(const u8* source,u32 size,bool ending){
    const auto fail=[this](const char* message){error=message;return false;};
    if(!source||size<4)return fail("Truncated message resource");const u32 count=read<u32>(source);
    if(count>(size-4)/8)return fail("Truncated message entry table");MessageResource next;
    for(u32 index=0;index<count;index++){
        MessageScript script;script.offset=read<u32>(source+4+index*8);script.flags=read<u32>(source+8+index*8);
        const u32 end=index+1<count?read<u32>(source+12+index*8):size;
        if(script.offset<4+count*8||end<script.offset||end>size)return fail("Invalid message entry span");
        for(u32 offset=script.offset;offset<end;){
            if(end-offset<4)return fail("Truncated message instruction");MessageInstruction instruction;instruction.offset=offset;instruction.time=read<u16>(source+offset);instruction.opcode=source[offset+2];const u32 length=source[offset+3];
            if(length>end-offset-4)return fail("Truncated message arguments");instruction.data.assign(source+offset+4,source+offset+4+length);
            if((ending?instruction.opcode==3:(instruction.opcode>=15&&instruction.opcode<=17))&&!decode(instruction.data.data(),length,instruction.text))return fail("Invalid message text");
            script.instructions.push_back(std::move(instruction));offset+=length+4;
        }
        next.scripts.push_back(std::move(script));
    }*this=std::move(next);return true;
}
}
