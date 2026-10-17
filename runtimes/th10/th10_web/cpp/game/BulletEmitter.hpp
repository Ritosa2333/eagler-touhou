#pragma once
#include "BulletFrame.hpp"
namespace th10 {
struct BulletEmitter {
    std::int16_t sprite_type,color;
    Vec3 position;
    float angle,spread,speed_start,speed_end;
    ProjectileCommand commands[18];
    u8 reserved_1d0[0x24];
    std::int16_t count,layers,pattern,reserved_1fa;
    u32 flags;
    i32 shoot_sound,turn_sound,command_start;
    u32 reserved_20c;
    void initialize() noexcept;
    i32 fire(EnemyBulletManager& manager,BulletBehaviorEnvironment& environment) const;
};
static_assert(sizeof(BulletEmitter)==0x210);
static_assert(offsetof(BulletEmitter,count)==0x1f4);
struct BulletBehaviorEnvironment : BulletFrameEnvironment {
    Rng* rng;
    const i32 *sprite_scripts,*cancel_types,*cancel_scripts,*draw_layers;
    const float* hitbox_sizes;
#ifdef TH_ENABLE_THPRAC
    // thprac_th10.cpp:2224 th10_real_bullet_sprite (PATCH_ST 0x406e03). When
    // set, the 0x4000 embedded-animation command is skipped so bullets keep
    // their base sprite. Points into PracticeState.
    const bool* real_bullet_sprite=nullptr;
#endif
    virtual void emit(const BulletEmitter& emitter)=0;
};
}
