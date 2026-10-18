#pragma once
#include "Types.hpp"
namespace th09 {
// Shared CPU geometry lets trails, effects and the renderer use the same span.
struct AttackColorVertex {Vec3 position;float reciprocal_w=0;u32 color=0;};
struct AttackTextureVertex {Vec3 position;union {float reciprocal_w=0;float rhw;};u32 color=0;Vec2 uv;};
static_assert(sizeof(AttackColorVertex)==20&&sizeof(AttackTextureVertex)==28);
}
