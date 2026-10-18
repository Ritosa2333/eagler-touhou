#pragma once
#include "Player.hpp"
#include "EnemyManager.hpp"
#include "Gui.hpp"
#include "ScreenEffect.hpp"
namespace th10 {
struct PlayerDrawEnvironment {
    GameEconomy* game;
    EnemyManager** enemies;
    Gui** gui;
    const std::int8_t* controller_flags;
    const i32* results_state;
    virtual void draw_animation(AnmVm& vm)=0;
    virtual void draw_option(PlayerOption& option)=0;
    virtual void rectangle(const ScreenRect& rectangle,u32 color)=0;
};
}
