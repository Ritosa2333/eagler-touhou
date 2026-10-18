#pragma once
#include "ZunGraphics.hpp"
#include "AnmVm.hpp"
namespace th15 {
struct AnmCamera {
    Vec3 eye{},reference{};Matrix4 view,projection;float fog_near=1000,fog_far=5000;Vec3 fog_rgb{160,160,160};u32 fog_color=0xffa0a0a0;
    AnmCamera(){view.identity();projection.identity();}
};
Vec3 project_vertex(const Vec3&,const Matrix4& world,const AnmCamera&,const GraphicsViewport&)noexcept;
int anm_projected_quad(AnmVm&,const AnmCamera&,const GraphicsViewport&,Vec3 (&out)[4])noexcept;
}
