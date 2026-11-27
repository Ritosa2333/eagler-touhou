// th16 相机——原版 0x12c 字节相机结构（Supervisor 内四台 + StageManager 自有一台）的镜像。
//
// 锚点：
//   0x43cb10  Supervisor 初始化四台相机：场地 0x4c1308 (32,16,384,448)、宽场地 0x4c1434 (128,16,384,448)、
//             UI 0x4c1560 (0,0,640,480)、渲染目标 0x4c168c ((w-408)/2,(h-472)/2,408,472)；kind 依次 0/1/2/3
//   0x43c780  "正对视口中心"：eye=(视口中心, (h/2)/tan(fov/2))、at=(视口中心, 0)、up=(0,-1,0)，近远 1 / 10000；
//             UI 相机与层绘制回调（0x46ddd0）每帧都用它复位
//   0x43c940  关卡相机：eye = pos + pos_offset，at = eye + facing_norm，up 为脚本给定；近远 30 / 8000；
//             顺便算出 right = normalize(cross(facing_norm, up))，并把抖动偏移写进 AnmManager 绘制偏移
//   0x43c710  把相机的 view / projection 下发给设备（AnmRenderer::use_camera）
//
// 原版字段按 dword 下标：0 pos  3 facing  6 up  9 facing_norm  0xc right  0xf pos_offset  0x12 facing_offset
//   0x15 fov  0x18 view[16]  0x28 projection[16]  0x38 viewport{x,y,w,h,minz,maxz}  0x3e kind  0x3f shift{x,y}
//   0x41 pos_delta  0x44 fog_near  0x45 fog_far  0x46 fog_rgba_f[4]（B,G,R,A）  0x4a fog_color
#pragma once
#include "RenderDevice.hpp"
#include "Types.hpp"

namespace th16 {

struct CameraFog {
    float near_ = 0, far_ = 0;
    float c[4] = {0, 0, 0, 0};   // B,G,R,A 浮点（0x409490 插值用）
    u32 color = 0;               // 0xAARRGGBB（D3DRS_FOGCOLOR）
};
using StageFog = CameraFog;

struct Camera {
    enum Kind : i32 { kField = 0, kWideField = 1, kUi = 2, kRenderTarget = 3 };

    Vec3 pos;                    // 0
    Vec3 facing;                 // 3
    Vec3 up{0, 1, 0};            // 6
    Vec3 facing_norm;            // 9
    Vec3 right;                  // 0xc
    Vec3 pos_offset;             // 0xf
    Vec3 facing_offset;          // 0x12
    float fov = 0.5235988f;      // 0x15（0x3f060a92 = 30°）
    float view[16] = {};         // 0x18
    float projection[16] = {};   // 0x28
    Viewport viewport{116, 4, 408, 472};  // 0x38（深度范围恒 0..1）
    i32 kind = kRenderTarget;    // 0x3e（DAT_004c17bc 记录"当前相机种类"）
    float shift_x = 0, shift_y = 0;   // 0x3f（画面抖动 DAT_004c1788/8c 抄入；0x43c940 转给 AnmManager 绘制偏移）
    Vec3 pos_delta;              // 0x41
    CameraFog fog;               // 0x44..0x4a

    // 0x43c780：正对视口中心的默认视图 / 投影（近 1、远 10000）
    void reset_to_viewport_center();
    // 0x43c940 前半：按 pos / facing_norm / up 建视图与投影（近 30、远 8000），并更新 right
    void look_along_facing();
    // 0x43c940 的 eye / at
    Vec3 eye() const { return {pos.x + pos_offset.x, pos.y + pos_offset.y, pos.z + pos_offset.z}; }
    Vec3 at() const { const Vec3 e = eye(); return {e.x + facing_norm.x, e.y + facing_norm.y, e.z + facing_norm.z}; }
    // D3DXVec3Project(v, viewport, projection, view, world = 单位阵)：世界坐标 →（屏幕 x, y, 视口深度）；rhw 另给
    void project(const float world[3], float& sx, float& sy, float& sz, float& rhw) const;
    // D3DXVec3Project(v, viewport, projection, view, world = 平移 translation)：0x466390 的两次投影用法
    void project_translated(const float translation[3], const float v[3], float& sx, float& sy, float& sz,
                            float& rhw) const;
};
using StageCamera = Camera;

// D3DX 行向量矩阵工具（v' = v · M）
namespace matrix {
void identity(float m[16]);
void multiply(float out[16], const float a[16], const float b[16]);   // D3DXMatrixMultiply(out, a, b)
void rotation_x(float m[16], float angle);
void rotation_y(float m[16], float angle);
void rotation_z(float m[16], float angle);
void translation(float m[16], float x, float y, float z);
void look_at_lh(float m[16], const Vec3& eye, const Vec3& at, const Vec3& up);
void perspective_fov_lh(float m[16], float fov, float aspect, float zn, float zf);
void transform_point(const float v[3], const float m[16], float out[4]);
}  // namespace matrix

}  // namespace th16
