#pragma once
#include "Camera.hpp"
namespace th10 {
// Recovered D3DX 9 math used by the game. No library image or CPU state.
// SIMD operations round to float independently of the extended game arithmetic.
struct GraphicsMath {
    static void multiply(Matrix4& output,const Matrix4& first,const Matrix4& second) noexcept;
    static void normalize(Vec3& output,const Vec3& input) noexcept;
    static void rotation(Matrix4& output,u32 axis,float radians) noexcept;
    static void translation(Matrix4& output,const Vec3& offset) noexcept;
    static void look_at(Matrix4& output,const Vec3& eye,const Vec3& target,const Vec3& up) noexcept;
    static void perspective(Matrix4& output,float field_of_view,float aspect,float near_plane,float far_plane) noexcept;
    static void transform(float* output,const Vec3& input,const Matrix4& matrix) noexcept;
    static void transform_normal(Vec3& output,const Vec3& input,const Matrix4& matrix) noexcept;
    static void transform_coordinate(Vec3& output,const Vec3& input,const Matrix4& matrix) noexcept;
    static void project(Vec3& output,const Vec3& input,const CameraViewport* viewport,const Matrix4* projection,const Matrix4* view,const Matrix4* world) noexcept;
    static void project_array(void* output,u32 output_stride,const void* input,u32 input_stride,u32 count,const CameraViewport* viewport,const Matrix4* projection,const Matrix4* view,const Matrix4* world) noexcept;
};
}
