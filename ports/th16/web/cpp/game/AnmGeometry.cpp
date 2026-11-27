// th16 ANM 几何实现——地址见 AnmGeometry.hpp。
#include "AnmGeometry.hpp"

#include <cmath>

namespace th16 {

namespace {

// DAT_004a3048 / DAT_004a3078：锚点 × 精灵宽高 → 四角本地坐标（TL, TR, BL, BR）
constexpr float kAnchorX[4][4] = {
    {-0.5f, 0.5f, -0.5f, 0.5f},
    {0.0f, 1.0f, 0.0f, 1.0f},
    {-1.0f, 0.0f, -1.0f, 0.0f},
    {-0.5f, -0.5f, 0.5f, 0.5f},
};
constexpr float kAnchorY[4][4] = {
    {-0.5f, -0.5f, 0.5f, 0.5f},
    {0.0f, 0.0f, 1.0f, 1.0f},
    {-1.0f, -1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
};

}  // namespace

float AnmGeometry::scale_factor(const AnmVm& vm, float window_scale) noexcept {
    const u32 mode = (vm.flags_hi >> 20) & 7u;
    if (mode == 1 || mode == 3) return window_scale;
    if (mode == 2 || mode == 4) return window_scale * 0.5f;
    return 1.0f;
}

Vec3 AnmGeometry::transform_position(const AnmVm& vm, Vec3 p) const {
    const float s = scale_factor(vm, window_scale);
    if (s != 1.0f) { p.x *= s; p.y *= s; p.z *= s; }
    if (vm.root != nullptr && (vm.flags_hi & AnmVm::kHiDetached) == 0) {
        if ((vm.flags_hi & AnmVm::kHi314) != 0) {
            // 随父 Z 旋转（0x405510 / 0x4054f0 = sin / cos(root.rotation.z)）
            const float sn = float(std::sin(double(vm.root->rotation.z))), cs = float(std::cos(double(vm.root->rotation.z)));
            const float x = p.x;
            p.x = x * cs - p.y * sn;
            p.y = p.y * cs + x * sn;
        }
        const Vec3 parent = world_position(*vm.root);
        p.x += parent.x;
        p.y += parent.y;
        p.z += parent.z;
        return p;
    }
    const u32 group = vm.flags_hi & 0xc0000u;
    if (group == 0x40000u) { p.x += field_offset_x; p.y += field_offset_y; }
    else if (group != 0) { p.x += screen_offset_x; p.y += screen_offset_y; }
    return p;
}

Vec3 AnmGeometry::world_position(const AnmVm& vm) const {
    return transform_position(vm, Vec3{
        vm.pos.x + vm.entity_offset.x + vm.pos2.x,
        vm.pos.y + vm.entity_offset.y + vm.pos2.y,
        vm.pos.z + vm.entity_offset.z + vm.pos2.z,
    });
}

void AnmGeometry::axis_corners(const AnmVm& vm, float out[4][2]) const {
    // 0x465c40 顺序：锚点 × 精灵尺寸 → 减 anchor_offset → × 窗口缩放（scale mode）→ × scale·scale2（含父）
    const int ax = int((vm.flags_lo >> 21) & 3u);
    const int ay = int((vm.flags_lo >> 23) & 3u);
    for (int i = 0; i < 4; ++i) {
        out[i][0] = kAnchorX[ax][i] * vm.sprite_size.x - vm.anchor_offset.x;
        out[i][1] = kAnchorY[ay][i] * vm.sprite_size.y - vm.anchor_offset.y;
    }
    const float window = scale_factor(vm, window_scale);
    if (window != 1.0f) for (int i = 0; i < 4; ++i) { out[i][0] *= window; out[i][1] *= window; }
    float sx = vm.scale2.x * vm.scale.x;
    float sy = vm.scale2.y * vm.scale.y;
    if (vm.parent != nullptr && (vm.flags_hi & AnmVm::kHiDetached) == 0) {
        sx = sx * vm.parent->scale2.x * vm.parent->scale.x;
        sy = sy * vm.parent->scale2.y * vm.parent->scale.y;
    }
    for (int i = 0; i < 4; ++i) { out[i][0] *= sx; out[i][1] *= sy; }
}

void AnmGeometry::rotated_corners(const AnmVm& vm, float out[4][2]) const {
    // 0x4660b0：x 先乘 scale，y 的缩放并入旋转式；保持 (lx·c − (sy·ly)·s) 的单精度顺序
    const float angle = anm_world_rotation(const_cast<AnmVm&>(vm)).z;
    const float c = float(std::cos(double(angle))), s = float(std::sin(double(angle)));
    const int ax = int((vm.flags_lo >> 21) & 3u);
    const int ay = int((vm.flags_lo >> 23) & 3u);
    float lx[4], ly[4];
    for (int i = 0; i < 4; ++i) {
        lx[i] = vm.sprite_size.x * kAnchorX[ax][i] - vm.anchor_offset.x;
        ly[i] = vm.sprite_size.y * kAnchorY[ay][i] - vm.anchor_offset.y;
    }
    const float window = scale_factor(vm, window_scale);
    if (window != 1.0f) for (int i = 0; i < 4; ++i) { lx[i] *= window; ly[i] *= window; }
    float sx = vm.scale2.x * vm.scale.x;
    float sy = vm.scale2.y * vm.scale.y;
    if (vm.parent != nullptr && (vm.flags_hi & AnmVm::kHiDetached) == 0) {
        sx = sx * vm.parent->scale2.x * vm.parent->scale.x;
        sy = sy * vm.parent->scale2.y * vm.parent->scale.y;
    }
    for (int i = 0; i < 4; ++i) {
        const float x = sx * lx[i];
        out[i][0] = x * c - sy * ly[i] * s;
        out[i][1] = sy * ly[i] * c + x * s;
    }
}

bool AnmGeometry::projected_corners(const AnmVm& vm, const Camera& camera, float out[4][2], float& depth) const {
    // 位置只取 pos + entity_offset + pos2（不经 0x406a70），用当前相机 D3DXVec3Project 到屏幕
    const float world[3] = {
        vm.entity_offset.x + vm.pos.x + vm.pos2.x,
        vm.entity_offset.y + vm.pos.y + vm.pos2.y,
        vm.entity_offset.z + vm.pos.z + vm.pos2.z,
    };
    // 两次 D3DXVec3Project 都以 world = 平移(位置)：先投原点，再投相机 right 单位向量
    const float origin[3] = {0.0f, 0.0f, 0.0f};
    float sx, sy, sz, rhw;
    camera.project_translated(world, origin, sx, sy, sz, rhw);
    if (sz < 0.0f || sz > 1.0f) return false;
    // 两点屏幕距离 = 一世界单位的像素长，取其一半作尺寸系数
    const float right_axis[3] = {camera.right.x, camera.right.y, camera.right.z};
    float rx, ry, rz, rrhw;
    camera.project_translated(world, right_axis, rx, ry, rz, rrhw);
    const float pixels_per_unit = std::sqrt((rx - sx) * (rx - sx) + (ry - sy) * (ry - sy) + (rz - sz) * (rz - sz));
    const float k = pixels_per_unit * 0.5f;
    const float width = vm.sprite_size.x * k * vm.scale.x * vm.scale2.x;
    const float height = vm.sprite_size.y * k * vm.scale.y * vm.scale2.y;

    const float angle = anm_world_rotation(const_cast<AnmVm&>(vm)).z;
    const float c = float(std::cos(double(angle))), s = float(std::sin(double(angle)));
    // 锚点：0 居中、1 左/上对齐、2 右/下对齐；3 在原版里读到的是未初始化的 sin 值，照搬
    float left, right, top, bottom;
    switch ((vm.flags_lo >> 21) & 3u) {
    case 0: left = width * -0.5f; right = width * 0.5f; break;
    case 1: left = 0.0f; right = width; break;
    case 2: left = -width; right = 0.0f; break;
    default: left = right = s; break;
    }
    switch ((vm.flags_lo >> 23) & 3u) {
    case 0: top = height * -0.5f; bottom = height * 0.5f; break;
    case 1: top = 0.0f; bottom = height; break;
    case 2: top = -height; bottom = 0.0f; break;
    default: top = bottom = s; break;
    }
    out[0][0] = left * c - top * s + sx;      out[0][1] = top * c + left * s + sy;
    out[1][0] = right * c - top * s + sx;     out[1][1] = top * c + right * s + sy;
    out[2][0] = left * c - bottom * s + sx;   out[2][1] = bottom * c + left * s + sy;
    out[3][0] = right * c - bottom * s + sx;  out[3][1] = bottom * c + right * s + sy;
    depth = sz;
    return true;
}

bool AnmGeometry::vm_corners(const AnmVm& vm, float out[4][3]) const {
    float corners[4][2];
    switch (vm.render_mode()) {
    case 0: case 2: case 3: axis_corners(vm, corners); break;
    case 1: rotated_corners(vm, corners); break;
    default: return false;
    }
    const Vec3 origin = world_position(vm);
    const float z = vm.entity_offset.z + vm.pos.z + vm.pos2.z;
    for (int i = 0; i < 4; ++i) {
        out[i][0] = corners[i][0] + origin.x;
        out[i][1] = corners[i][1] + origin.y;
        out[i][2] = z;
    }
    return true;
}

}  // namespace th16
