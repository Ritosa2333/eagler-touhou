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
    virtual bool dialogue_boss_name(){return false;}
    virtual bool dialogue_fade(float){return false;}
    virtual bool dialogue_stage_complete(){return false;}
};
struct DialogueControl {
    virtual ~DialogueControl()=default;
    virtual bool start_dialogue(i32)=0;
    virtual bool dialogue_waiting()const=0;
    // TH12 0x1a2 STARTS a dialogue; th11 never started one from the ECL. FUN_0041fbe0, which 0x1a2
    // calls, frees any live dialogue, allocates a fresh 0xac-byte state and initialises it through
    // FUN_0041f900. Reading it as a close is what left the real starter unreachable.
    virtual bool dialogue_close(){return false;}
};
// 41cfe0/41d380. HUD priority 24 runs this before normal ANM priority 26.
class Dialogue {
public:
    Dialogue(AnmManager& a,AnmResource& t,AnmResource& p,AnmResource& f,
             AnmResource& e,AnmResource& l,DialogueEffects& w)
        :manager(a),families(a),text(t),player(p),front(f),enemy(e),logo(l),effects(w){}
    // TH13's dialogue class is larger than the 0xac bytes above: its interpreter addresses
    // +0x134, +0x14c, +0x178, +0x17c, +0x180, +0x184, +0x194, +0x198 and +0x1a0, and
    // MSG opcode 28 (th13.exe 0x428c8e) writes the payload's two floats into a pair at
    // +0x194/+0x198. TH12 has no such pair, so the port keeps it here, named after the
    // address, until TH13's own state layout is transcribed.
    Vec2 text_pair_194{};
    // Retail TH14 +6c/+1ac/+1b4; separate from the TH12 comparison layout.
    u32 th14_text_background=0;
    float th14_text_width=0;
    i32 th14_text_layout=0;
    u32 th14_enemy_portraits[4]{};
    DialogueState state;bool active=false;i32 character=0,stage=1,last_opcode=-1;
    std::string error;
    EnemyAnimations* th14_animations=nullptr;
    static bool th14_asset(i32 stage,i32 selection,u32 opcode,i32& file,i32& script)noexcept;
    // Resolves message entry `id` to the byte offset of its instruction stream.
    // Split out of begin() so the container can be verified without animations.
    static bool locate(const std::vector<u8>&,i32 id,u32& offset,u32& flags);
    bool begin(const std::vector<u8>&,i32);
    void clear();
    // 0 remains active, 1 ended, -2 failed. Elapsed is ticked by the HUD owner.
    i32 update(u32 held,u32 pressed,u32 skip_frames=0,u32 fire_frames=0);
private:
    AnmManager& manager;EnemyAnimations families;
    AnmResource& text;AnmResource& player;AnmResource& front;AnmResource& enemy;AnmResource& logo;
    DialogueEffects& effects;const u8* end=nullptr;
    AnmVm* find(u32& id){auto* vm=manager.find(id);if(!vm)id=0;return vm;}
    bool create(u32 slot,AnmResource&,i32 script,u16 file);
    bool create_th14(u32 slot,i32 selection,u32 opcode);
    bool th14_background(i32 layout);
    static void th14_text_position(AnmVm&);
    bool expression(bool enemy_side,i32 value);
    bool write(u32 slot,const std::string&,i32 offset=0,i32 font=0,i32 style=-1);
    bool speaker(i32,i32 selection=0);
    bool command(u8,const u8*,u32);
    bool fail(const char* s){error=s;return false;}
};
}
