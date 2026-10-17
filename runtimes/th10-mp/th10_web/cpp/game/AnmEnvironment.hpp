#pragma once
#include "AnmVm.hpp"
#include "Rng.hpp"
namespace th10 {
struct AnmEnvironment {
    float* rate;
    Rng* visual_rng;
    Rng* script_rng;
    const Vec3* reference_positions[2];
    const Vec3* camera_delta;
    const Vec3* default_tangent;
    Rng& random(const AnmVm& vm) const noexcept {return *(vm.flags&0x40000000?visual_rng:script_rng);}
    virtual void bind_sprite(AnmVm& vm,i32 index)=0;
    virtual void change_draw_mode(AnmVm& vm)=0;
    virtual void* allocate_geometry(u32 bytes)=0;
    virtual AnmVm* spawn_child(AnmVm& parent,i32 script,u32 mode)=0;
};
}
