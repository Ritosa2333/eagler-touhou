#pragma once
#include "AnmManager.hpp"
#include "MessageResource.hpp"
#include "StageConfiguration.hpp"
namespace th17 {
struct DialogueResources {i32 text=6,front=5,logo=11,player=10;std::array<i32,6> enemy{{7,8,-1,-1,-1,-1}};i32 character=0;};
struct DialogueInput {float rate=1;u32 held=0,pressed=0;i32 shoot_frames=0,skip_frames=0;};
struct DialogueServices {
    virtual ~DialogueServices()=default;
    virtual bool clear_dialogue_field()=0;
    virtual bool dialogue_text(AnmVm&,u32 color,u32 shadow,i32 style,i32 x,i32 y,const std::string&)=0;
    virtual bool dialogue_sound(i32)=0;
    virtual bool dialogue_boss_music()=0;virtual bool dialogue_music_fade(i32)=0;
    virtual bool dialogue_next_stage()=0;virtual bool dialogue_boss_name()=0;
    virtual bool dialogue_notice(u32)=0;
};
class Dialogue {
    AnmManager& animations;DialogueServices& host;const StageConfiguration& stage;DialogueResources resources;
    const MessageScript* script=nullptr;u32 cursor=0;bool initialized=false;
    bool check(bool,const char*);AnmVm* find(u32&);bool interrupt(u32,i32,bool=false);bool text(u32,u32,u32,i32,i32,i32,const std::string&);
    u32 create(i32,i32,const Vec3& position={});i32 bank(i32)const noexcept;
    bool speaking(i32,i32=0);bool lines(const std::string&);bool balloon(i32,float);bool follow_balloon();
    void reset_text(i32);void position_text();bool child_width(float);
public:
    i32 id=0;Timer age,script_age,wait;
    u32 player_portrait=0;std::array<u32,4> boss_portraits{};u32 name=0;std::array<u32,4> text_handles{};u32 boss_label=0,bubble=0;
    i32 complete_pulse=0;u32 flags=0;i32 line=0,cleared=0,speaker=0;
    std::array<u32,3> colors{};Vec3 position{384,640,0};float width=320;i32 bubble_style=0,portrait_mode=0;
    std::string error;
    Dialogue(AnmManager& a,DialogueServices& h,const StageConfiguration& s,DialogueResources r={}):animations(a),host(h),stage(s),resources(r){}
    ~Dialogue();
    bool initialize(const MessageResource&,i32 entry);
    // 0: active, 1: original end opcode, -1: explicit failure.
    i32 update(const DialogueInput&);bool clear();
    u32 instruction_offset()const noexcept{return script&&cursor<script->instructions.size()?script->instructions[cursor].offset:0;}
};
}
