#pragma once
#include "StageResource.hpp"
#include "AnmCamera.hpp"
namespace th17 {
class StageVisibility {
    const AnmCamera& camera;
    Matrix4 screen, fixed;
    Vec2 origin;
    float distance_squared;
public:
    StageVisibility(const AnmCamera&,const GraphicsViewport&,Vec2 playfield_origin,float distance_squared)noexcept;
    bool visible(const StageObject&,const Vec3& instance,std::array<Vec3,16>* projected=nullptr)const noexcept;
};
// Original background box corners and centre slices. The viewport describes
// projection; playfield_origin describes the independent logical clipping box.
bool stage_visible(const StageObject&,const Vec3& instance,const AnmCamera&,const GraphicsViewport&,Vec2 playfield_origin,float distance_squared,std::array<Vec3,16>* projected=nullptr)noexcept;
}
