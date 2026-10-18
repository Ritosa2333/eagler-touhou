#pragma once
#include "animation.hpp"

namespace th20::source::sprite {
// ANM v8 instruction storage; parameter slots may be modified by unmasked
// destinations. Scripts therefore remain writable, as in the original engine.
struct AnmInstruction {
    std::int16_t opcode;
    std::uint16_t size;
    std::int16_t time;
    std::uint16_t mask;
    std::uint32_t* arguments() {return reinterpret_cast<std::uint32_t*>(this+1);}
};
static_assert(sizeof(AnmInstruction)==8);
// External dependencies have no fallback implementation. VA comments identify
// the original storage or function that the engine integration must supply.
namespace anm_environment {
float& clock_scale();                              // storage 0x5aefe4
const float* timer_rate();                         // pointer 0x5aefe0
AnmInstruction* script(Animation&);                // 0x437470 + 0x437490
bool gameplay_frozen();                            // nullable 0x5ba828 -> 0x424060
std::uint32_t random_next();                       // 0x423ee0 on RNG 0x5ba4c4
std::uint32_t random_bounded(std::uint32_t);         // 0x423ea0
float random_unit();                              // 0x429830
float random_signed_unit();                       // 0x4298e0
float camera_component(std::int32_t variable);     // 10016..10021, 0x5c53fc etc.
void assign_sprite(Animation&,std::int32_t index); // 0x438620, negative uses fallback 0x120
void set_layer(Animation&,std::int32_t);            // 0x450350 (also relinks via 0x40e5e0)
void add_camera_offset(Vec3&);                     // 0x4296e0 with 0x5c5540
void calculate_corners(Animation&,Vec3 (&)[4]);    // 0x445770
float screen_scale();                             // layout 0x5b6758 +20c0
std::int32_t screen_offset(unsigned preset,unsigned axis); // layout +58/+5c or +60/+64
void* allocate_geometry(std::uint32_t bytes);       // 0x41f610
std::uint32_t spawn_child(Animation&,std::int32_t script,std::uint32_t flags); // 0x451050
std::uint32_t spawn_detached(Animation&,std::int32_t script,std::uint32_t flags); // 0x4512b0
Animation& lookup_animation(std::uint32_t handle);  // 0x44ced0
void spawn_effect(Animation&,std::int32_t script);  // 0x49db70 via 0x437520(0)
}
std::int32_t anm_integer_variable(Animation&,std::int32_t); // 0x437d00
float anm_float_variable(Animation&,float);                 // 0x437eb0
std::uint32_t& anm_integer_destination(Animation&,AnmInstruction&,unsigned); // 0x437220
float& anm_float_destination(Animation&,AnmInstruction&,unsigned);          // 0x437310
float animation_slowdown(Animation&) noexcept;              // 0x4374b0
void update_animation_motion(Animation&);                   // 0x435520
// Exact contiguous Interpolation<T> storage: components 1/2/3, integer color
// or float; wrapped_angle selects the original angle scalar 0x42b1f0.
void sample_animation_interpolation(void* storage,unsigned components,bool integer,bool wrapped_angle,void* output,const float* rate);
void update_animation_interpolations(Animation&);           // 0x4358d0
Vec3& inherited_animation_rotation(Animation&);              // 0x437770
Vec3 animation_position(Animation&);                        // 0x4376e0 + 0x4379e0
void transform_animation_offset(Animation&,Vec3&,bool rotate,bool scale); // 0x437840
void update_animation_geometry(Animation&);                 // 0x435c80
bool implemented_anm_opcode(std::int16_t) noexcept;
// 0x42b5d0: returns 1 on deletion / nonzero update callback; 0 otherwise.
std::int32_t execute_animation(Animation&);
}
