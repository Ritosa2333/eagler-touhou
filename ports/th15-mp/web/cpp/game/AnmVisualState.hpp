#pragma once
#include "Types.hpp"
#include <array>
namespace th15 {
// Game animation properties. The renderer consumes these values; this is not
// a graphics API object or a byte-layout clone of the original executable.
struct AnmVisualState {
    u32 flags=6,render_flags=0x8000;
    i32 layer=0,sprite=0;
    Vec3 child_anchor{},angular_velocity{};
    Vec2 scale{1,1},secondary_scale{1,1},scale_velocity{},uv_scale{1,1},uv_offset{},uv_velocity{},size{};
    u32 color=0xffffffff,secondary_color=0;
    Vec2 uv[4]{},sprite_size{};
    Matrix4 sprite_matrix,transform_matrix,uv_matrix;
    i32 sprite_frame=0;
    Vec3 translation{};
    std::array<Vec3,4> quad{};u32 inherited_color=0;
    void set_layer(i32 value)noexcept;
    void advance(Vec3& rotation,float rate)noexcept;
    u32 draw_mode()const noexcept{return flags>>25&31;}
    u32 blend_mode()const noexcept{return flags>>5&15;}
    bool visible()const noexcept{return flags&1;}
};
float normalize_angle(float angle)noexcept;
}
