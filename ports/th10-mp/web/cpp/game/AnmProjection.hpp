#pragma once
#include "AnmRenderer.hpp"
namespace th10 {
struct AnmFog {float near_distance,far_distance;float color[4];u32 packed_color;};
struct AnmProjectionEnvironment {
    const Vec3* camera_unit;
    const Vec3* camera_position;
    const AnmFog* fog;
    virtual void rotation(Matrix4& matrix,u32 axis,float radians)=0;
    virtual void multiply(Matrix4& output,const Matrix4& first,const Matrix4& second)=0;
    virtual void project(Vec3& output,const Vec3& input,const Matrix4& world)=0;
    virtual void transform(float* output,const Vec3& input,const Matrix4& world)=0;
};
struct AnmProjection {
    AnmRenderer& renderer;AnmProjectionEnvironment& environment;
    void update_transform(AnmVm& vm);
    i32 project_quad(AnmVm& vm);
    i32 billboard_geometry(const AnmVm& vm);
    i32 draw_billboard(AnmVm& vm,bool fog);
    i32 draw_projected(AnmVm& vm,bool fog);
    i32 draw_strip(AnmVm& vm,const void* vertices,u32 count);
    i32 draw_model(AnmVm& vm);
};
}
