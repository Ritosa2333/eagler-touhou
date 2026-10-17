#pragma once
#include "Enemy.hpp"
#include "Rng.hpp"
#include "AnmVm.hpp"
namespace th10 {
struct EnemyEnvironment {
    Rng* script_rng;
    const Vec3* player_position;
    Enemy* boss;
    const i32* rank;
    const i32* difficulty;
    float* default_rate;
    const Vec3* default_tangent;
    const Vec2* default_tangent2;
    const Vec3* camera_delta;
    virtual AnmVm* animation(u32& id)=0;
    Extended aim_at_player(const Vec3& position) const noexcept;
};
struct EnemyVariables final : EclGlobals {
    EnemyState& enemy;
    EnemyEnvironment& environment;
    EnemyVariables(EnemyState& enemy,EnemyEnvironment& environment):enemy(enemy),environment(environment){}
    i32 integer(i32 variable) override;
    Extended floating(i32 variable) override;
    i32* integer_reference(i32 variable) override;
    float* float_reference(i32 variable) override;
};
}
