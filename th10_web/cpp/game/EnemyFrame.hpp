#pragma once
#include "EnemyVariables.hpp"
#include "EnemyPhase.hpp"
#include "AnmRegistry.hpp"
#include "AnmEnvironment.hpp"
#include "EnemyDrops.hpp"
namespace th10 {
struct EnemyHealthBar {float amount;i32 style;};
struct EnemyFrameEnvironment : EnemyEnvironment {
    EnemyPhaseState phase;
    EclServices* scripts;
    AnmEnvironment* animations;
    AnmRegistry* registry;
    u32* started_animations;
    const i32* alternate_active;
    const i32* player_state;
    Enemy** player_target;
    u8* player_target_seen;
    i32* score;
    i32* enemy_activity;
    u32* boss_hp_flags;
    Enemy** boss_slots;
    EnemyHealthBar* health_bars;
    i32* boss_lives;
    const u32* message_status;
    ItemDropEnvironment* items;
#ifdef TH_ENABLE_THPRAC
    // F4 time lock (thprac_th10.cpp:526, PATCH_HK 0x40E5B0). When true the
    // enemy lifetime tick holds `current` instead of incrementing it.
    virtual bool time_locked() const { return false; }
#endif
    virtual i32 player_damage(const Vec3& position,const Vec2& hitbox)=0;
    virtual void player_collision(const Vec3& position,const Vec2& hitbox)=0;
    virtual i32 destroy(EnemyState& enemy)=0;
    virtual void play_sound(i32 sound,float horizontal_position)=0;
};
}
