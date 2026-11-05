#pragma once
#include "Types.hpp"
#include "Rng.hpp"
#include "Timer.hpp"
namespace th15 {
// Logical variables, independent of the original animation object's byte layout.
struct AnmVariables {
    i32 integers[4]{};float floats[4]{},vector[3]{};i32 extra_integers[2]{};
    float random_scale=1,random_angle=3.1415927410125732421875f;u32 random_bound=65536;
    Vec3 position{},rotation{};
    i32 integer(i32 value,Rng& rng)noexcept;
    float floating(float value,Rng& rng)noexcept;
    i32* integer_destination(i32* argument)noexcept;
    float* float_destination(float* argument)noexcept;
};
}
