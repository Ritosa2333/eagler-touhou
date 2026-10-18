#pragma once
#include "Gui.hpp"
#include "EnemyManager.hpp"
#include "Player.hpp"
#include "ScreenEffect.hpp"
namespace th10 {
struct GuiFrameEnvironment {
    GameEconomy* game;
    Player** player;
    EnemyManager** enemies;
    const u32* spell_flags;
    const u32* engine_flags;
    i32* pending_screen;
    AnmRegistry* registry;
    virtual void update_animation(AnmVm& vm)=0;
    virtual void bind_digit(AnmFile& file,AnmVm& vm,i32 sprite)=0;
    virtual u32 create_animation(AnmFile& file,i32 script)=0;
    virtual i32 update_dialogue(Dialogue& dialogue)=0;
    virtual void release_dialogue(Dialogue* dialogue)=0;
    virtual void play_sound(i32 sound)=0;
};
struct GuiDrawEnvironment {
    GameEconomy* game;
    EnemyManager** enemies;
    virtual void draw_animation(AnmVm& vm)=0;
    virtual void rectangle(const ScreenRect& rectangle,u32 color)=0;
    virtual float presentation_boss_health(float current)=0;
};
}
