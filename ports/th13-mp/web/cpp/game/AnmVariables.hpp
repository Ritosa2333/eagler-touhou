#pragma once
#include "GameRandom.hpp"
#include "Timer.hpp"
#include "Types.hpp"
namespace th13 {
struct AnmVariables {
    i32 integers[4]{};
    float floats[4]{};
    i32 extra_integers[2]{};
    float random_scale = 1, random_angle = 3.1415927410125732421875f;
    u32 random_bound = 65536;
    Vec3 position{}, rotation{};
    i32 integer(i32 value, GameRandom& rng) noexcept;
    float floating(float value, GameRandom& rng) noexcept;
    i32* integer_destination(i32* argument) noexcept;
    float* float_destination(float* argument) noexcept;
};
}
