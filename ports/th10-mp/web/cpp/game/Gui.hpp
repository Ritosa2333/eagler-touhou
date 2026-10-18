#pragma once
#include "AnmManager.hpp"
#include "UpdateChain.hpp"
#include "GameEconomy.hpp"
namespace th10 {
struct Dialogue;
struct GuiAnimationEnvironment;
struct GuiScoreEnvironment;
struct GuiFrameEnvironment;
struct GuiDrawEnvironment;
struct GuiHealthBar {float amount;i32 style;};
struct Gui {
    u32 flags,state;
    UpdateChainEntry* update_entry;
    UpdateChainEntry* draw_entry;
    AnmVm high_score_digits[10],score_digits[10],life_icons[9],power_digits[4],countdown_digits[2],faith_digits[7];
    AnmVm enemy_marker;
    u32 bonus_digits[8],notification,power_notification;
    u32 unresolved_9e1c[2],boss_name,boss_life_icons[10],difficulty_badge,difficulty_label,border,background;
    Timer elapsed;u32 elapsed_flags;
    i32 displayed_high_score,displayed_score,score_step;
    AnmFile* stage_animations;
    float displayed_boss_health,target_boss_health;
    i32 boss_health_points;
    i32 boss_lives;
    GuiHealthBar boss_health[4];
    u32 display_flags;
    Dialogue* dialogue;
    u8* message_file;
    i32 countdown;
    i32 previous_countdown;
    AnmFile* animations;
    i32 ending_frames;
    void initialize(Gui** current) noexcept;
    void update_lives(i32 lives) noexcept;
    void notify(i32 kind,i32 value,GuiAnimationEnvironment& environment);
    void update_score(GuiScoreEnvironment& environment);
    void update_power(i32 whole,i32 fraction,GuiScoreEnvironment& environment);
    i32 update(GuiFrameEnvironment& environment);
    i32 draw(GuiDrawEnvironment& environment);
};
static_assert(sizeof(Gui)==0x9ed0&&offsetof(Gui,enemy_marker)==0x9a48);
static_assert(offsetof(Gui,bonus_digits)==0x9df4&&offsetof(Gui,dialogue)==0x9eb8);
struct GuiAnimationEnvironment {
    AnmRegistry* registry;
    virtual u32 create(AnmFile& file,i32 script)=0;
    virtual void bind_sprite(AnmVm& vm,i32 index)=0;
};
struct GuiScoreEnvironment {
    GameEconomy* game;
    const i32* normal_extends;
    const i32* extra_extends;
    virtual void bind_digit(AnmFile& file,AnmVm& vm,i32 sprite)=0;
    virtual void update_animation(AnmVm& vm)=0;
    virtual void add_life()=0;
};
}
