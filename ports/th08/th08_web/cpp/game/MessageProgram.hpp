#pragma once
#include "Types.hpp"
#include <vector>
namespace th08 {
enum class MessageOpcode:u8 {Delete,PortraitScript,PortraitSprite,Text,Wait,PortraitInterrupt,ResumeEcl,Music,Intro,StageResults,Halt,StageEnd,FadeMusic,Skippable,FadeScreen,ConfigureAll,SpeakerText,ConfigurePortrait,TextboxVisible,TopLine,BottomLine,Selection,ReadSelected};
struct MessageInstruction {
    u16 time=0;MessageOpcode opcode=MessageOpcode::Delete;u8 size=0;
};
static_assert(sizeof(MessageInstruction)==4);
class MessageProgram {
public:
    bool load(const u8* bytes,u32 size);
    void clear(){data.clear();offsets.clear();}
    const u8* entry(i32 index)const;
    bool instruction(const u8* pointer,MessageInstruction& output)const;
    const u8* bytes()const{return data.data();}
    u32 size()const{return data.size();}
    u32 count()const{return offsets.size();}
    static bool text(const u8* source,u32 available,char* output,u32 capacity);
private:
    std::vector<u8> data;std::vector<u32> offsets;
};
}
