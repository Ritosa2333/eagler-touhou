#pragma once
#ifdef TH11_MULTIPLAYER
#include "Multiplayer.hpp"
#include "PlayerFrame.hpp"
#include "BombController.hpp"
#include "ItemRewards.hpp"
#include "Communication.hpp"
#include "GameInput.hpp"
#include <memory>
namespace th11 {
class GameBattle;
// A permanent callback owner for one seat. No swaps of world/player globals.
struct MultiplayerPilot final:PlayerFrameWorld,BombWorld,ItemRewardEffects {
    GameBattle& world;unsigned seat;
    GameEconomy owned_economy{};Communication owned_communication{};
    GameEconomy& economy;Communication& communication;
    std::unique_ptr<PlayerFrame> owned_player;
    std::unique_ptr<BombController> owned_bomb;
    PlayerFrame* player=nullptr;BombController* bomb=nullptr;
    PlayerFrameInput input{};GameInput keys{};
    ItemRewards rewards;
    bool ghost=false,life_latched=false,force_attract=false;
    unsigned life_hold=0,power_taps=0,power_gap=0,ghost_clock=0;
    i32 ghost_dx=0,ghost_dy=0;
    u32 ghost_random=1,challenge_misses=0;
    MultiplayerPilot(GameBattle&,unsigned);
    bool alive()const noexcept;
    bool initialize(int);
    bool tick();bool tick_bomb();bool revive(bool rescued);
    bool sound(i32,float)override;
    bool sound(i32,float,bool)override;
    bool special_damage(const Vec3&,const Vec2&,i32&)override;
    bool player_sound(i32)override;
    bool attract_items()override;
    bool cancel_bullets(const Vec3*,float,bool)override;
    bool cancel_lasers(const Vec3*,float,bool,bool)override;
    bool start_bomb()override;
    bool spawn_item(i32,Vec3,u32,float,float)override;
    bool display_lives(i32,i32)override;
    bool record_death()override;
    bool enemy_death()override;
    bool game_over(bool)override;
    bool bomb_sound(i32,float,bool)override;
    bool bomb_stop_sound(i32)override;
    bool bomb_cancel(Vec3,float,u32,bool)override;
    i32& bomb_count()override;
    bool bomb_background_color(u32)override;
    bool bomb_refund_power()override;
    bool bomb_cancel_beam(Vec3,bool)override;
    bool popup(Vec3,i32,u32)override;
    bool notify(i32)override;
    bool power_changed()override;
    bool lives_changed(i32,i32)override;
};
}
#endif
