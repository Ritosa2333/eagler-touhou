#include "SceneMatrices.hpp"
#include "AnmTransforms.hpp"
#include "StageScript.hpp"

#include <cmath>

namespace th13 {

// D3DXVec3Normalize 语义（本地 d3dx9_43.dll = Wine builtin，导出 0x10025F10；
// 反汇编 + Unicorn 实跑，见 artifacts/round-render-semantics-20261031/
// scene-view-ulp/disasm/d3dx9_43.dll-10025f10.txt）：
//   len2 = x*x + y*y + z*z        — x87 扩展精度（0x10025F2B..0x10025F3F）
//   len  = sqrt(len2)             — fsqrt 扩展精度（0x10025F4B）
//   len == 0  → 输出 (0,0,0)      — 0x10025F55 je 0x10025F80 零分支
//   len 为 NaN → 走除法按 IEEE 传播 — 0x10025F55 jp 0x10025F59（不置零）
//   其余：逐分量 x/len 除法（0x10025F59..0x10025F6C），每分量一次 f32 存储。
// 旧实现（f32 平方和 + f32 倒数 + Newton 精化）与 D3DX 在 1 ULP 上分歧：
// scene-view-ulp 报告归因 4/27 个元素（frame1/15 的 view[2]/view[8]）。
// 证据边界：这是本机唯一可执行的 D3DX 二进制（Wine builtin）的语义；零售
// EXE 实际加载的 Microsoft d3dx9_43.dll 不在本机，无法逐位复核（residualRisks）。
Vec3 normalize_scene_vector(Vec3 v) noexcept {
    const double x = v.x;
    const double y = v.y;
    const double z = v.z;
    const double length = std::sqrt(x * x + y * y + z * z);
    if (length == 0.0) return {};
    return {static_cast<float>(x / length), static_cast<float>(y / length),
            static_cast<float>(z / length)};
}

Matrix4 scene_look_at(Vec3 eye, Vec3 target, Vec3 up) noexcept {
    // 原版 0x43BA20 以 stdcall 调 IAT 0x4A22D4 = d3dx9_43.dll!
    // D3DXMatrixLookAtLH(out=camera+0x58, eye, at, up)（capstone:
    // disasm/th13-0043ba20.txt 0x43BA98；decomp 0043ba20.c:27-37）。
    //
    // 【对齐对象与证据边界——必须如实理解】
    // 零售 EXE 实际链接的 Microsoft d3dx9_43.dll 不在本环境（只有 Wine
    // builtin），零售 D3DXMatrixLookAtLH 的逐位实现**无法用地址级证据裁决**。
    // 本函数逐位对齐的是 oracle 比较器桩（oracle/render_oracle.py 的
    // D3DXStubs.mat_LookAtLH）——独立原版 draw log 本身就是该桩产出，所以
    // 桩是当前唯一可用的比较基准。桩语义（double 全程，只在写矩阵时 f32 化）：
    //   z = (at-eye); z /= sqrt(z·z)          — 不回存 f32
    //   x = cross(up,z); x /= sqrt(x·x)       — 不回存 f32
    //   y = cross(z,x)                        — 不归一化
    //   m30..32 = -(axis·eye)                 — double 点积
    // 中间任何一步回存 f32 都会引入 1 ULP 偏差（scene-view-ulp 报告
    // stubArtifactElements 的成因）。已验证：frame 1/3/4 与 native draw log
    // **逐位一致**（C++ 直连产品 TU + Python 复刻双重验证）。
    // 已知未决项：Wine builtin（0x1001AEA0）在该函数里多一次
    // D3DXVec3Normalize(y,y)（0x1001af7e），Microsoft 发行版是否同样归一化 y
    // 无法验证；frame 0 的 y 轴长度 1.000000015 不足以区分两种实现
    // （1.47e-8 缩放小于 f32 半 ULP），不作论据。若未来取得 Microsoft DLL
    // 并证明其归一化 y，本函数需同步修改并重采 native。
    const double ex = eye.x, ey = eye.y, ez = eye.z;
    const double ax = target.x, ay = target.y, az = target.z;
    const double ux = up.x, uy = up.y, uz = up.z;
    double zx = ax - ex, zy = ay - ey, zz = az - ez;
    double zl = std::sqrt(zx * zx + zy * zy + zz * zz);
    // 退化 |z|=0（eye==at）：沿用 oracle 桩语义（除 0 → IEEE NaN 传播）。
    // 注意 Wine builtin 的 D3DXVec3Normalize 在 length==0 时输出零向量
    // （0x10025F80-8D），与桩不同；该差异属上述未决项，端口与桩保持一致。
    if (zl == 0.0) zl = std::nan("");
    zx /= zl; zy /= zl; zz /= zl;
    double xx = uy * zz - uz * zy, xy = uz * zx - ux * zz, xz = ux * zy - uy * zx;
    double xl = std::sqrt(xx * xx + xy * xy + xz * xz);
    if (xl == 0.0) xl = std::nan("");
    xx /= xl; xy /= xl; xz /= xl;
    const double yx = zy * xz - zz * xy, yy = zz * xx - zx * xz, yz = zx * xy - zy * xx;
    Matrix4 out;
    out.m[0] = static_cast<float>(xx); out.m[4] = static_cast<float>(xy);
    out.m[8] = static_cast<float>(xz);
    out.m[1] = static_cast<float>(yx); out.m[5] = static_cast<float>(yy);
    out.m[9] = static_cast<float>(yz);
    out.m[2] = static_cast<float>(zx); out.m[6] = static_cast<float>(zy);
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
    // 原版 0x43BB2C..0x43BB7C：x87 算 cross(direction, up) 存 f32，再调
    // IAT 0x4A22A0 = d3dx9_43.dll!D3DXVec3Normalize(edi+0x30, edi+0x30)
    // （capstone: 0x43BB7C ff15a0224a00；decomp 0043ba20.c 尾部）；
    // normalize 语义与 D3DX 对齐后此处 reference 才与原版同一位模式。
    out.reference = normalize_scene_vector(cross(s.direction, s.up));
    out.fog_near = s.fog.near_distance;
    out.fog_far = s.fog.far_distance;
    out.fog_rgb = {s.fog.channels[2], s.fog.channels[1], s.fog.channels[0]};
    out.fog_color = s.fog.color;
    return out;
}

}  // namespace th13
