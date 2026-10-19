#pragma once
#include "PlayerMovement.hpp"
#include "PlayerShooting.hpp"
#include "PlayerLifecycle.hpp"
#include "Bomb.hpp"
namespace th10 {
struct PlayerFrameEnvironment {
    GameEconomy* economy;
    float* default_rate;
    const u32* input_keys;
    const u32* dialogue;
    const i32* enemy_count;
    const i32* replay_mode;
    Bomb* bomb;
    bool gui_present;
    AnmEnvironment* animations;
    PlayerLifecycleEnvironment* lifecycle;
    PlayerMovementEnvironment* movement;
    PlayerShootingEnvironment* shooting;
#ifdef TH_ENABLE_THPRAC
    PracticeState* practice=nullptr;
#endif
    virtual void clear_bullets(bool include_protected)=0;
    virtual void clear_lasers(bool include_protected)=0;
    virtual void cancel_bullet_circle(const Vec3& position,float radius,bool animated)=0;
    virtual void cancel_laser_circle(const Vec3& position,float radius)=0;
    virtual void start_bomb()=0;
    virtual void update_options(Player& player)=0;
    virtual void update_power(i32 level,i32 percent)=0;
    virtual void drop_power(const Vec3& position,i32 kind,float angle)=0;
    virtual void game_over(bool replay)=0;
};
}
