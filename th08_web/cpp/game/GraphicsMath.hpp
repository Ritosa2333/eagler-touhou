#pragma once
#include "AnmRenderer.hpp"
namespace th08 {
enum class GraphicsArithmetic { Simd, Scalar };
// D3DX 8 has two arithmetic paths with different rounding and normalization.
// Simd is the original runtime's default; Scalar is its DisablePSGP option.
struct GraphicsMath {
    static void arithmetic(GraphicsArithmetic path);
    static void multiply(Matrix4& output, const Matrix4& first, const Matrix4& second);
    static void rotation(Matrix4& output, u32 axis, float radians);
    static void quaternion(Matrix4& output,const float* xyzw);
    static void normalize(Vec3& output, const Vec3& input);
    static void look_at(Matrix4& output, const Vec3& eye, const Vec3& target, const Vec3& up);
    static void perspective(Matrix4& output, float fov, float aspect, float near_plane, float far_plane);
    static void transform_coordinate(Vec3& output, const Vec3& input, const Matrix4& matrix);
    static void compose_projection(Matrix4& output,const Matrix4* projection,const Matrix4* view,const Matrix4* world);
    static void project_composed(Vec3& output,const Vec3& input,const Viewport* viewport,const Matrix4& transform);
    static void project(Vec3& output, const Vec3& input, const Viewport* viewport,
                        const Matrix4* projection, const Matrix4* view, const Matrix4* world);
};
}
