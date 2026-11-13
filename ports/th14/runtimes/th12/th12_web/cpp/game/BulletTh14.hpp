#pragma once
#include "EnemyTh14Emitter.hpp"
namespace th11 {
struct BulletState;struct BulletEnvironment;
struct Th14BulletAppearance {i32 script,sprites[64];float hitbox;i32 group,effect,secondary;};
static_assert(sizeof(Th14BulletAppearance)==0x114);
extern const Th14BulletAppearance th14_bullet_appearances[43];
extern const i32 th14_bullet_cancel_scripts[8];
i32 bullet_th14_start_transforms(BulletState&,BulletEnvironment&)noexcept;
i32 bullet_th14_acceleration_tick(BulletState&,BulletEnvironment&)noexcept;
i32 bullet_th14_turn_tick(BulletState&,BulletEnvironment&)noexcept;
i32 bullet_th14_speed_ramp_tick(BulletState&,BulletEnvironment&)noexcept;
i32 bullet_th14_bounce_tick(BulletState&,BulletEnvironment&)noexcept;
i32 bullet_th14_step_tick(BulletState&,BulletEnvironment&)noexcept;
i32 bullet_th14_wrap_tick(BulletState&,BulletEnvironment&)noexcept;
i32 bullet_th14_offscreen_hold_tick(BulletState&,BulletEnvironment&)noexcept;
i32 bullet_th14_polar_move_tick(BulletState&,BulletEnvironment&)noexcept;
i32 bullet_th14_position_tick(BulletState&,BulletEnvironment&)noexcept;
void bullet_th14_size_tick(BulletState&,BulletEnvironment&)noexcept;
}
