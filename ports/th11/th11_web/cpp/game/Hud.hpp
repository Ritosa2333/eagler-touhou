#pragma once
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "EnemyCommands.hpp"
#include "GameEconomy.hpp"
#include "StageCompletion.hpp"
#include "AsciiText.hpp"

namespace th11 {
struct HudInput {
    Vec3 player_position{};
    bool player_present=true,dialogue=false,spell=false;
    i32 stage=1,section=0,seconds=-1,hundredths=0;
    i32 practice_high=0;
    bool practice=false;
    u32 result_animation=0;
    i32 spell_frames=0,spell_time=0;
};
struct HudEffects {
    virtual ~HudEffects()=default;
    virtual bool hud_sound(i32){return false;}
};
struct HudScore {
    i32 displayed=0,speed=0,high=0,continues=0,high_continues=0;
    u32 flags=0;
    void update(i32 score) noexcept;
};
// Native HUD callbacks 41b380, 41c5f0 and 41c660. Owned VMs update at
// priority 24; manager-owned labels update later at priority 26.
class Hud {
public:
    Hud(AnmManager&,AnmResource&,AnmResource&,GameEconomy&,EnemyCommandEnvironment&,StageCompletionState&,HudEffects&);
    ~Hud();
    AnmVm lives[9]{},digits[2]{},communication[4]{},indicator{};
    Timer elapsed;
    i32 previous_seconds=-1;
    HudScore score;
    float displayed_health=0,target_health=0;
    i32 health=0;
    u32 frame_animation=0,boss_name=0,stars[10]{},difficulty_intro=0,difficulty_label=0;
    u32 bonus_digits[8]{},spell_notice=0,item_notice=0,spell_time_animation=0;
    bool show_spell_time=false;
    bool practice_skip_logo=false;
    bool notice(i32 type,i32 value=0);
    void prepare_stage();
    bool start_stage(AnmResource& logo,const HudInput&,bool demo=false,bool initial=true,i32 control_mode=0,i32 continues=0);
    void display_lives(i32 count,i32 fragments);
    bool update(const HudInput&);
    bool finish_update(const HudInput&);
    bool draw_outer(AnmRenderer&,const HudInput&);
    bool draw_inner(AnmRenderer&,const HudInput&);
    bool queue_text(AsciiText&,const HudInput&);
private:
    AnmManager& animations;AnmResource& front;AnmResource& text;
    GameEconomy& economy;EnemyCommandEnvironment& enemies;StageCompletionState& completion;HudEffects& effects;
    bool bind(AnmVm&,AnmResource&,i32,u16);
    void interrupt(u32,i16);
    u32 create(AnmResource&,i32,u16);
    bool boss_visible(const HudInput&)const;
};
}
