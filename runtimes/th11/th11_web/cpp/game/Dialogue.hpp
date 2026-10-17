#pragma once
#include "EnemyAnimations.hpp"
#include <string>
#include <vector>
namespace th11 {
struct DialogueState {
    i32 id=0;Timer elapsed,time,wait;u32 animations[8]{};u32 reserved60=0;
    const u8* instruction=nullptr;Vec3 text_positions[3]{};
    i32 release_signal=0;u32 flags=0;i32 next_line=0,lines_initialized=0,speaker=0;
    u32 colors[3]{};
};
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(DialogueState)==0xac);
static_assert(offsetof(DialogueState,instruction)==0x64);
#endif
struct DialogueText {
    u32 animation=0,color=0;i32 offset=0,font=0,style=0;
    std::string bytes; // Original Shift-JIS bytes; no lossy UTF-8 conversion.
    bool right_aligned=false;
    bool centered=false;
};
struct DialogueEffects {
    virtual ~DialogueEffects()=default;
    virtual bool dialogue_text(const DialogueText&){return false;}
    virtual bool dialogue_sound(i32){return false;}
    virtual bool dialogue_music(){return false;}
    virtual bool dialogue_fade(float){return false;}
    virtual bool dialogue_stage_complete(){return false;}
};
struct DialogueControl {
    virtual ~DialogueControl()=default;
    virtual bool start_dialogue(i32)=0;
    virtual bool dialogue_waiting()const=0;
};
// 41cfe0/41d380. HUD priority 24 runs this before normal ANM priority 26.
class Dialogue {
public:
    Dialogue(AnmManager& a,AnmResource& t,AnmResource& p,AnmResource& f,
             AnmResource& e,AnmResource& l,DialogueEffects& w)
        :manager(a),families(a),text(t),player(p),front(f),enemy(e),logo(l),effects(w){}
    DialogueState state;bool active=false;i32 character=0,stage=1,last_opcode=-1;
    std::string error;
    bool begin(const std::vector<u8>&,i32);
    void clear();
    // 0 remains active, 1 ended, -2 failed. Elapsed is ticked by the HUD owner.
    i32 update(u32 held,u32 pressed);
private:
    AnmManager& manager;EnemyAnimations families;
    AnmResource& text;AnmResource& player;AnmResource& front;AnmResource& enemy;AnmResource& logo;
    DialogueEffects& effects;const u8* end=nullptr;
    AnmVm* find(u32& id){auto* vm=manager.find(id);if(!vm)id=0;return vm;}
    bool create(u32 slot,AnmResource&,i32 script,u16 file);
    bool expression(bool enemy_side,i32 value);
    bool write(u32 slot,const std::string&,i32 offset=0,i32 font=0,i32 style=-1);
    bool speaker(i32);
    bool command(u8,const u8*,u32);
    bool fail(const char* s){error=s;return false;}
};
}
