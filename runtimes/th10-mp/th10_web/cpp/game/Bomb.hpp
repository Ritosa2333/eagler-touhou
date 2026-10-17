#pragma once
#include "Timer.hpp"
namespace th10 {
struct BombEnvironment;
struct UpdateChainEntry;
struct BombDamageContext {u32 spell_flags;i32 character;bool boss_active;};
struct Bomb {
    u32 flags,manager_state;
    UpdateChainEntry *update_entry,*draw_entry;
    u32 reserved_010;
    Timer timer;
    u32 timer_flags;
    i32 active;
    u32 animation;
    Vec3 position;
    float radius;
    float radial_speed;
    i32 mode;
    i32 damage(const Vec3& target,const BombDamageContext& context) const noexcept;
    i32 start(BombEnvironment& environment);
    i32 update(BombEnvironment& environment);
    i32 cancel_projectiles(BombEnvironment& environment);
};
static_assert(offsetof(Bomb,position)==0x30);
}
