#include "SceneMatrices.hpp"
#include "AnmTransforms.hpp"
#include "StageScript.hpp"

#include <cmath>

namespace th13 {

Vec3 normalize_scene_vector(Vec3 v) noexcept {
    const float squared =
        static_cast<float>(static_cast<float>(v.x * v.x + v.y * v.y) + v.z * v.z);
    if (!(squared >= 0x1p-46f)) return {};
    const float inverse = static_cast<float>(1.f / static_cast<float>(std::sqrt(double(squared))));
    const float refined = static_cast<float>(
        static_cast<float>(.5f * inverse) *
        static_cast<float>(3.f - static_cast<float>(squared * inverse * inverse)));
    return {static_cast<float>(v.x * refined), static_cast<float>(v.y * refined),
            static_cast<float>(v.z * refined)};
}

namespace {
Vec3 view_cross(Vec3 a, Vec3 b) {
    return {static_cast<float>(double(a.y) * b.z - double(a.z) * b.y),
            static_cast<float>(double(a.z) * b.x - double(a.x) * b.z),
            static_cast<float>(double(a.x) * b.y - double(a.y) * b.x)};
}
}  // namespace

Matrix4 scene_look_at(Vec3 eye, Vec3 target, Vec3 up) noexcept {
    const Vec3 z = normalize_scene_vector({target.x - eye.x, target.y - eye.y, target.z - eye.z});
    const Vec3 x = normalize_scene_vector(view_cross(up, z));
    const Vec3 y = view_cross(z, x);
    const Vec3 axes[] = {x, y, z};
    Matrix4 out;
    for (u32 c = 0; c < 3; ++c) {
        const auto a = axes[c];
        out.m[c] = a.x;
        out.m[4 + c] = a.y;
        out.m[8 + c] = a.z;
        out.m[12 + c] =
            -static_cast<float>((double(a.x) * eye.x + double(a.y) * eye.y) + double(a.z) * eye.z);
    }
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
    // D3DXMatrixPerspectiveFovLH 原式 m34 = -zn*zf/(zf-zn)（oracle 桩同式）；
    // 经 -m22*zn 换算会差 1 ULP（frame-0 proj[14] 对拍实证）。
    out.m[14] = -static_cast<float>(near_plane * far_plane /
                                    static_cast<float>(far_plane - near_plane));
    return out;
}

namespace {
Vec3 add(Vec3 a, Vec3 b) { return {float(a.x + b.x), float(a.y + b.y), float(a.z + b.z)}; }
Vec3 cross(Vec3 a, Vec3 b) {
    return {float(float(a.y * b.z) - float(a.z * b.y)), float(float(a.z * b.x) - float(a.x * b.z)),
            float(float(a.x * b.y) - float(a.y * b.x))};
}
}  // namespace

AnmCamera stage_camera(const StageCamera& s, const GraphicsViewport& viewport) noexcept {
    AnmCamera out;
    out.eye = add(s.position, s.eye_offset);
    out.view = scene_look_at(out.eye, add(out.eye, s.direction), s.up);
    // [y 镜像移除·2026-10-25] 此处曾对取景矩阵 y 列（{1,5,9,13}）整体取负，
    // 声称补偿后端世界路径的 clip-space y 翻转（Shaders.hpp gl_Position.y=-y）。
    // 两项证据判定其为非原版的本作局部分叉，按授权移除：
    //  1) 原版真值：battle_oracle --replay（r4/oracle-replay/replay121.jsonl）
    //     f2 L2 世界 quad 的 view[5]=+1、view[13]=-1——与 D3DXMatrixLookAtLH
    //     直出一致，无任何列镜像；x/z 分量与 port 未镜像值六位重合
    //     （-79.7740936/897.2701）。
    //  2) 共享核心一致性：th15 同位 StageCamera.cpp 的 stage_camera 不做
    //     镜像，且两作 portable/sdl/Shaders.hpp 逐字节一致（同一
    //     gl_Position.y 取负）。th13 不经游戏矩阵做二次补偿。
    // 生产 draw 与 draws trace 消费同一（未镜像）矩阵；旧依据 golden f1242
    // 属 RT 合成链修复前的旧渲染对拍，已过期不再引用。若 STD 实拍出现
    // 上下翻转，属 STD 数据/顶点约定在真后端边界的裁决项，不得回添镜像。
    // [M0.3 实证修正] 远平面 8000 → 1800：原版 0x43ba20（场景相机构建）
    // D3DXMatrixPerspectiveFovLH(fov, aspect, 0x41f00000=30.0, 0x44e10000=1800.0)
    // 字节级实证（近平面 30 原已一致）。aspect = 该相机视口宽高（ctx+0xd8
    // D3DVIEWPORT9 的 W/H；0x43bbc0 初始化两组视口 {0,0,640,480}/{32,16,384,448}，
    // 0x404140 绘制遍历按相机组切换——组路由语义待 battle_oracle 捕获后裁决）。
    out.projection =
        scene_perspective(s.fov, float(float(viewport.width) / float(viewport.height)), 30, 1800);
    out.reference = normalize_scene_vector(cross(s.direction, s.up));
    out.fog_near = s.fog.near_distance;
    out.fog_far = s.fog.far_distance;
    out.fog_rgb = {s.fog.channels[2], s.fog.channels[1], s.fog.channels[0]};
    out.fog_color = s.fog.color;
    return out;
}

}  // namespace th13
