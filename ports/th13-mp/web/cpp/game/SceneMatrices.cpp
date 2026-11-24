#include "SceneMatrices.hpp"
#include "AnmTransforms.hpp"
#include "StageScript.hpp"

#include <cmath>

namespace th13 {

Vec3 normalize_scene_vector(Vec3 v) noexcept {
    const double x = v.x;
    const double y = v.y;
    const double z = v.z;
    const double length = std::sqrt(x * x + y * y + z * z);
    if (length == 0.0) return {};
    return {static_cast<float>(x / length), static_cast<float>(y / length), static_cast<float>(z / length)};
}

Matrix4 scene_look_at(Vec3 eye, Vec3 target, Vec3 up) noexcept {
    const double ex = eye.x, ey = eye.y, ez = eye.z;
    const double ax = target.x, ay = target.y, az = target.z;
    const double ux = up.x, uy = up.y, uz = up.z;
    double zx = ax - ex, zy = ay - ey, zz = az - ez;
    double zl = std::sqrt(zx * zx + zy * zy + zz * zz);
    if (zl == 0.0) zl = std::nan("");
    zx /= zl;
    zy /= zl;
    zz /= zl;
    double xx = uy * zz - uz * zy, xy = uz * zx - ux * zz, xz = ux * zy - uy * zx;
    double xl = std::sqrt(xx * xx + xy * xy + xz * xz);
    if (xl == 0.0) xl = std::nan("");
    xx /= xl;
    xy /= xl;
    xz /= xl;
    const double yx = zy * xz - zz * xy, yy = zz * xx - zx * xz, yz = zx * xy - zy * xx;
    Matrix4 out;
    out.m[0] = static_cast<float>(xx);
    out.m[4] = static_cast<float>(xy);
    out.m[8] = static_cast<float>(xz);
    out.m[1] = static_cast<float>(yx);
    out.m[5] = static_cast<float>(yy);
    out.m[9] = static_cast<float>(yz);
    out.m[2] = static_cast<float>(zx);
    out.m[6] = static_cast<float>(zy);
    out.m[10] = static_cast<float>(zz);
    out.m[3] = out.m[7] = out.m[11] = 0;
    out.m[12] = static_cast<float>(-(xx * ex + xy * ey + xz * ez));
    out.m[13] = static_cast<float>(-(yx * ex + yy * ey + yz * ez));
    out.m[14] = static_cast<float>(-(zx * ex + zy * ey + zz * ez));
    out.m[15] = 1;
    return out;
}

Matrix4 scene_perspective(float fov, float aspect, float near_plane, float far_plane) noexcept {
    const float half = static_cast<float>(fov * .5f);
    const float c = static_cast<float>(std::cos(double(half)));
    const float s = static_cast<float>(std::sin(double(half)));
    const float cot = static_cast<float>(c / s);
    Matrix4 out;
    out.m[0] = static_cast<float>(cot / aspect);
    out.m[5] = cot;
    out.m[10] = static_cast<float>(far_plane / static_cast<float>(far_plane - near_plane));
    out.m[11] = 1;
    out.m[14] = -static_cast<float>(near_plane * far_plane / static_cast<float>(far_plane - near_plane));
    return out;
}

namespace {
Vec3 add(Vec3 a, Vec3 b) { return {float(a.x + b.x), float(a.y + b.y), float(a.z + b.z)}; }
Vec3 cross(Vec3 a, Vec3 b) {
    return {float(float(a.y * b.z) - float(a.z * b.y)), float(float(a.z * b.x) - float(a.x * b.z)),
            float(float(a.x * b.y) - float(a.y * b.x))};
}
}

AnmCamera stage_camera(const StageCamera& s, const GraphicsViewport& viewport) noexcept {
    AnmCamera out;
    out.eye = add(s.position, s.eye_offset);
    out.view = scene_look_at(out.eye, add(out.eye, s.direction), s.up);
    out.projection = scene_perspective(s.fov, float(float(viewport.width) / float(viewport.height)), 30, 1800);
    out.reference = normalize_scene_vector(cross(s.direction, s.up));
    out.fog_near = s.fog.near_distance;
    out.fog_far = s.fog.far_distance;
    out.fog_rgb = {s.fog.channels[2], s.fog.channels[1], s.fog.channels[0]};
    out.fog_color = s.fog.color;
    return out;
}

}
