#pragma once
#include "AnmVm.hpp"
#if PORT_TITLE == 14
#include "TransitionType0Th14.hpp"
#endif
namespace th11 {
struct AnmVertex {Vec3 position;float reciprocal_w;u32 color;Vec2 uv;};
static_assert(sizeof(AnmVertex)==28);
// Original logical 640x480 coordinates, before backend viewport conversion.
bool anm_quad_positions(const AnmVm& vm,Vec3 (&out)[4],const AnmRenderSpace& space={}) noexcept;
#if PORT_TITLE == 14
bool anm_th14_draw_quad_positions(const AnmVm&,Vec3 (&out)[4],const AnmRenderSpace& space={}) noexcept;
#endif
Vec2 anm_vertex_uv(const AnmVm& vm,u32 vertex) noexcept;
Vec2 anm_screen_uv(Vec3 position) noexcept;
bool anm_ring_vertices(const AnmVm& vm,AnmVertex* out,u32 capacity,const AnmRenderSpace& space={}) noexcept;
struct AnmRipple {
    AnmVertex vertices[33];u32 reserved_39c;
    float radii[33],velocities[32],scroll_x,scroll_y;u32 reserved_4ac;
};
static_assert(sizeof(AnmRipple)==0x4b0);
void anm_ripple_initialize(AnmVm& vm,AnmRipple& ripple,Rng& script_rng) noexcept;
void anm_ripple_update(AnmVm& vm) noexcept;
#if PORT_TITLE == 16
bool anm_th16_install_effect(AnmVm&,i32,AnmEnvironment&);
#endif
#if PORT_TITLE == 13 || PORT_TITLE == 14
struct AnmWorldVertex{Vec3 position;u32 color;Vec2 uv;};
#endif
#if PORT_TITLE == 14
bool anm_world_ring_vertices(const AnmVm& vm,AnmWorldVertex* out,u32 capacity) noexcept;
struct AnmTh14Ribbon{AnmWorldVertex vertices[80];Vec3 steps[40];float angular_speed,angle;u8 reserved[160];float phase;Timer timer;};
struct AnmTh14Particles{u32 children[200];Vec3 positions[200],tangents[200];u32 states[200];Vec3 near_center,far_center,origin;u32 reserved;Timer timer;};
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(AnmTh14Ribbon)==0xa20);
static_assert(sizeof(AnmTh14Particles)==0x193c);
#endif
bool anm_th14_install_effect(AnmVm&,i32,AnmEnvironment&);
void anm_th14_ribbon_update(AnmVm&,AnmEnvironment&) noexcept;
int anm_th14_particles_update(AnmVm&,AnmEnvironment&);
void anm_th14_particles_destroy(AnmVm&,AnmEnvironment&) noexcept;
#endif
#if PORT_TITLE == 13
struct AnmRibbon{AnmWorldVertex vertices[32];Vec3 steps[16];u8 reserved[64];float phase;Timer timer;};
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(AnmRibbon)==0x418);
#endif
bool anm_th13_install_effect(AnmVm&,i32,AnmEnvironment&);
void anm_th13_ribbon_update(AnmVm&,AnmEnvironment&) noexcept;
#endif
}
