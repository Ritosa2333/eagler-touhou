#pragma once
#include "AnmVm.hpp"
namespace th11 {
struct GraphicsViewport {u32 x=0,y=0,width=640,height=480;float near_depth=0,far_depth=1;};
struct GraphicsMath {
    static Matrix4 multiply(const Matrix4&,const Matrix4&) noexcept;
    static Matrix4 rotation(u32 axis,float angle) noexcept;
    static Vec3 normalize(Vec3) noexcept;
    static Vec3 cross(Vec3,Vec3) noexcept;
    static Matrix4 look_at(Vec3 eye,Vec3 target,Vec3 up) noexcept;
    static Matrix4 perspective(float fov,float aspect,float near_plane,float far_plane) noexcept;
    static void transform(Vec3,const Matrix4&,float (&out)[4]) noexcept;
    static Vec3 project(Vec3,const GraphicsViewport&,const Matrix4& projection,const Matrix4& view,const Matrix4& world) noexcept;
};
}
