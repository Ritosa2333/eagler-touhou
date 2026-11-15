#pragma once
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "EnemyCommands.hpp"
#include "GameEconomy.hpp"
#include "StageCompletion.hpp"
#include "AsciiText.hpp"
#include "HudScalarTh14.hpp"

namespace th11 {
struct HudInput {
    Vec3 player_position{};
    bool player_present=true,dialogue=false,spell=false;
    i32 stage=1,section=0,seconds=-1,hundredths=0;
#if PORT_TITLE == 14
    i32 character=0;
    i32 subtype=0,spell_id=-1;u32 spell_flags=0;
#endif
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
    i32 displayed_value()const noexcept{return th14_hud_load(displayed);}
    i32 high_value()const noexcept{return th14_hud_load(high);}
    i32 continues_value()const noexcept{return th14_hud_load(continues);}
    void update(const i32& score) noexcept;
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
#if PORT_TITLE == 16
    u32 th16_season_root=0;i32 th16_season_active=0;
    bool update_season_th16();
    u32 th16_icon_handles[16]{};
    AnmVm* icon_th16(u32 n)const noexcept{return n<16?animations.find(th16_icon_handles[n]):nullptr;}
    bool bind_icons_th16();
    void display_bombs(i32 count,i32 fragments);
#endif
#if PORT_TITLE == 14
    u32 bonus_marks[2]{};
    u32 th14_overlay=0;
    u32 digit_handles[2]{};
    AnmVm* digit_th14(u32 n)const noexcept{return n<2?animations.find(digit_handles[n]):nullptr;}
    bool timer_th14(const HudInput&);
    bool timer_text_th14(AsciiText&,const HudInput&);
    // TH14 42edf6 allocates and registers these VMs with the ANM manager.
    // Their scripts select layer 24; they have no separate HUD draw callback.
    u32 icon_handles[16]{};
    AnmVm* icon_th14(u32 index)const noexcept {return index<16?animations.find(icon_handles[index]):nullptr;}
    bool bind_icons_th14();
    void display_bombs(i32 count,i32 fragments);
    bool boss_name_th14(const HudInput&);
    bool remaining_th14();
    struct BossTh14 {float displayed=0,target=0;i32 health=0;u32 handles[7]{};bool active=false,hidden=false;};
    BossTh14 th14_boss[2]{};
    bool bosses_th14(const HudInput&);
    bool indicator_th14(const HudInput&);
#endif
    bool show_spell_time=false;
    bool notice(i32 type,i32 value=0);
    void prepare_stage(i32* current_seconds=nullptr);
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
#if PORT_TITLE == 14
    bool notice_th14(i32,i32);
#endif
};
}
