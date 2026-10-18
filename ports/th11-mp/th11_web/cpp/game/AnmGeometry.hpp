#pragma once
#include "AnmVm.hpp"
namespace th11 {
struct AnmVertex {Vec3 position;float reciprocal_w;u32 color;Vec2 uv;};
static_assert(sizeof(AnmVertex)==28);
// Original logical 640x480 coordinates, before backend viewport conversion.
bool anm_quad_positions(const AnmVm& vm,Vec3 (&out)[4],bool preserve_subpixel=false) noexcept;
Vec2 anm_screen_uv(Vec3 position) noexcept;
bool anm_ring_vertices(const AnmVm& vm,AnmVertex* out,u32 capacity) noexcept;
struct AnmRipple {
    AnmVertex vertices[33];u32 reserved_39c;
    float radii[33],velocities[32],scroll_x,scroll_y;u32 reserved_4ac;
};
static_assert(sizeof(AnmRipple)==0x4b0);
void anm_ripple_initialize(AnmVm& vm,AnmRipple& ripple,Rng& script_rng) noexcept;
void anm_ripple_update(AnmVm& vm) noexcept;
}
