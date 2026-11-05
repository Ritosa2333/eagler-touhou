#include "StageResource.hpp"
#include <algorithm>
namespace th11 {
const StageInstruction* StageResource::instruction(u32 offset)const noexcept {
    const auto it=std::lower_bound(instructions.begin(),instructions.end(),offset,[](const StageInstruction& a,u32 b){return a.offset<b;});
    return it!=instructions.end()&&it->offset==offset?&*it:nullptr;
}
bool StageResource::open(const u8* data,u32 size){
    return decode(data,size,nullptr);
}
bool StageResource::open_practice(const std::vector<u8>& original,const std::vector<u8>& patched){
    StageResource verified;if(original.size()!=patched.size()||!verified.open(original.data(),u32(original.size())))return false;
    u32 script;std::memcpy(&script,original.data()+8,4);
    if(std::memcmp(original.data(),patched.data(),script))return false;
    return decode(patched.data(),u32(patched.size()),&verified);
}
bool StageResource::decode(const u8* data,u32 size,const StageResource* retained){
    *this={};auto fail=[&](const char* message){error=message;objects.clear();instances.clear();instructions.clear();return false;};
    if(!data||size<0x90)return fail("truncated STD header");
    auto s16=[&](u32 p){i16 x;std::memcpy(&x,data+p,2);return x;};auto u32at=[&](u32 p){u32 x;std::memcpy(&x,data+p,4);return x;};
    const i32 object_count=s16(0),primitive_count=s16(2);const u32 instance_offset=u32at(4),script_offset=u32at(8);
    if(object_count<0||primitive_count<0||u32(object_count)>(size-0x90)/4||instance_offset>size||script_offset>size||instance_offset>script_offset)return fail("invalid STD section bounds");
    const u32 directory_end=0x90+u32(object_count)*4;
    const auto* name_end=static_cast<const u8*>(std::memchr(data+16,0,128));if(!name_end)return fail("unterminated STD animation name");
    animation_name.assign(reinterpret_cast<const char*>(data+16),name_end-(data+16));
    for(i32 i=0;i<object_count;++i){
        const u32 start=u32at(0x90+u32(i)*4);if(start<directory_end||start>instance_offset||instance_offset-start<28)return fail("invalid STD object");
        StageObject object;object.id=s16(start);object.layer=data[start+2];object.flags=data[start+3];std::memcpy(&object.position,data+start+4,12);std::memcpy(&object.size,data+start+16,12);
        u32 p=start+28;bool ended=false;
        while(p+4<=instance_offset){const i16 type=s16(p),length=s16(p+2);if(type<0){ended=true;break;}if(length<28||u32(length)>instance_offset-p)return fail("invalid STD primitive length");
            StagePrimitive primitive;primitive.type=type;primitive.script=s16(p+4);primitive.animation=animation_count++;
            std::memcpy(&primitive.position,data+p+8,12);std::memcpy(&primitive.size,data+p+20,8);object.primitives.push_back(primitive);p+=u32(length);
        }
        if(!ended)return fail("missing STD primitive terminator");objects.push_back(std::move(object));
    }
    if(animation_count!=u32(primitive_count))return fail("STD primitive count mismatch");
    bool ended=false;for(u32 p=instance_offset;p+2<=script_offset;){const i16 id=s16(p);if(id<0){ended=true;break;}if(script_offset-p<16||id>=object_count)return fail("invalid STD instance");StageInstance instance;instance.object=id;instance.flags=s16(p+2);std::memcpy(&instance.position,data+p+4,12);instances.push_back(instance);p+=16;}
    if(!ended)return fail("missing STD instance terminator");
    constexpr u32 argument_counts[]={0,2,3,5,3,5,3,1,3,5,11,11,1,1,2,0,1,1};
    for(u32 p=script_offset;p+8<=size;){StageInstruction command;command.time=signed_bits(u32at(p));command.opcode=s16(p+4);command.length=s16(p+6);command.offset=p-script_offset;
        if(command.time==-1&&command.opcode==-1&&command.length==-1){
            instructions.push_back(std::move(command));
            // THPrac ST6 installs a sentinel in front of the retained native
            // script. The 0x8000-frame jump still targets its original tail.
            // Retain only byte-identical, already-verified instructions.
            if(retained)for(const auto& tail:retained->instructions)if(tail.offset>=p-script_offset+20){
                const u32 absolute=script_offset+tail.offset;
                if(tail.length>0){
                    if(signed_bits(u32at(absolute))!=tail.time||s16(absolute+4)!=tail.opcode||s16(absolute+6)!=tail.length||(!tail.arguments.empty()&&std::memcmp(data+absolute+8,tail.arguments.data(),tail.arguments.size()*4)))return fail("modified retained STD tail");
                    instructions.push_back(tail);
                }
                else instructions.push_back(tail);
            }
            break;
        }
        if(command.length<8||u32(command.length)>size-p||(command.length&3))return fail("invalid STD instruction length");
        const u32 count=(u32(command.length)-8)/4;if(command.opcode>=0&&command.opcode<18&&count<argument_counts[command.opcode])return fail("truncated STD instruction arguments");
        command.arguments.resize(count);if(count)std::memcpy(command.arguments.data(),data+p+8,count*4);instructions.push_back(std::move(command));p+=u32(instructions.back().length);
    }
    if(instructions.empty())return fail("missing STD script");
    for(const auto& command:instructions)if(command.opcode==1&&!instruction(command.argument<u32>(0)))return fail("invalid STD jump target");
    return true;
}
}
