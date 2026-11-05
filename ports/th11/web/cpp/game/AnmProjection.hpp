#pragma once
#include "AnmGeometry.hpp"
#include "GraphicsMath.hpp"
namespace th11 {
struct AnmCamera {
    Vec3 position{},right{1,0,0};
    Matrix4 view{},projection{};
    GraphicsViewport viewport;
    float fog_near=1000,fog_far=5000;u32 fog_color=0xffa0a0a0;
    AnmCamera(){view.identity();projection.identity();}
};
Matrix4 anm_world_matrix(AnmVm& vm,bool anchor_translation) noexcept;
bool anm_projected_quad(AnmVm& vm,const AnmCamera&,Vec3 (&out)[4],Matrix4* world=nullptr) noexcept;
bool anm_billboard_quad(const AnmVm& vm,const AnmCamera&,Vec3 (&out)[4]) noexcept;
}
