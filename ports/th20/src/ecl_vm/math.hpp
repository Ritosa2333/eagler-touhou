#pragma once
#include <cstdint>
#include "native_core.hpp"
namespace th20::source::ecl::math {
float sine(float value); // 0x439820 -> double CRT sin -> float
float cosine(float value); // 0x4397c0
float square_root(float value); // 0x446b30
float arctangent(float y,float x); // 0x459280
float wrap_angle(float value); // 0x438540; intentionally limited to 34 iterations
float angle_difference(float a,float b); // 0x4396c0
void polar(float& x,float& y,float angle,float length); // 0x439330
void rotate(float& x,float& y,float angle); // 0x458fa0
float easing(std::int32_t mode,float time,float duration); // 0x454ef0
struct Interpolator {
    float start=0,end=0,tangent_start=0,tangent_end=0,current=0;
    th20::recovered::Timer timer{};
    std::int32_t duration=0,mode=0;
    // ECL-specific suffix in the original 56-byte record.
    std::int32_t frame_base=0,subroutine=0,instruction_offset=0;
    float sample(const float* clock_rate); // 0x42a110
};
static_assert(sizeof(Interpolator)==56);
}
