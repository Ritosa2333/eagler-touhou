#pragma once
#include "PlayerGauge.hpp"
#include "ShotFiring.hpp"
namespace th08 {
struct PlayerMotionState {
    AnmVm animation;PlayerMovementState movement;PlayerFormState form;PlayerOption options[4];PlayerGaugeState gauge;
};
struct PlayerMotionInput {
    Vec2 minimum,extent{384,448};Vec3 enemy;i32 bomb=0,bomb_type=0;
    u16 buttons=0;u8 character=0,gui_blocked=0,tampered=0,enemy_present=0,always_hitbox=0,padding[1]{};
};
struct PlayerMotionActions:PlayerFormActions,PlayerMovementActions,PlayerOptionActions,PlayerGaugeActions {
    virtual void step_animation(AnmVm&)=0;
    virtual void draw_player(AnmVm&)=0;
};
// Entire 0044aec0 sequence. ANM loading/execution and effect allocation are
// supplied by the owning game; history changes after options consume it.
void update_player_motion(PlayerMotionState&,PlayerMotionInput&,Timer& shooting,GameGauge&,const ShotProfile& human,const ShotProfile& focused,const FrameTiming&,PlayerMotionActions&);
// Self/option portion of 0044d530; the owner draws active shots and the bomb first.
void draw_player_motion(PlayerMotionState&,const Vec2& offset,bool game_over,PlayerMotionActions&);
}
