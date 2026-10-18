#pragma once
#include "Types.hpp"
#include "Rng.hpp"
#include "Timer.hpp"
namespace th09 {
struct EclLocals {
    i32 integers[8]{};float floats[8]{};i32 counters[4]{};float extra[2]{};
    i32 integer_arguments[4]{};float float_arguments[4]{};
    i32* integer(i32 id) noexcept;
    float* real(i32 id) noexcept;
};
static_assert(sizeof(EclLocals)==120);
struct EclWorldState {Rng random;i32 difficulty=0,rank=0;};
struct EclPlayfieldState {
    Vec3 player; i32 character=0,attack_levels[2]{};
    i32 integer_arguments[4]{};float float_arguments[4]{};
    i32 side=0;u32 flags=0;
};
struct EclVariables {
    EclWorldState* world=nullptr;EclPlayfieldState* field=nullptr;EclPlayfieldState* opponent=nullptr;EclLocals* locals=nullptr;
    i32 shared_integer[8]{};float shared_real[8]{};
    Vec3 position,resolved_position,origin,target,last_delta;
    float direction=0,angular_velocity=0,speed=0,acceleration=0,orbit_radius=0,orbit_angle=0,orbit_velocity=0;
    Timer lifetime{0,0,0};i32 life=0,last_damage=0,life_thresholds[4]{},item_reward=0,score_reward=0;
    i32 drop_count=0,drop_item=0;
    u32 flags=0;u8 boss_id=0;
    i32* integer_field(i32 id) noexcept;
    float* float_field(i32 id) noexcept;
    i32 read_int(i32 id) noexcept;
    double read_value(float value) noexcept;
    float read_float(float value) noexcept;
};
}
