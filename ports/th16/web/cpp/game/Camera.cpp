// th16 相机实现——地址见 Camera.hpp。矩阵公式按 D3DX9 参考实现逐项展开，保持单精度运算顺序。
#include "Camera.hpp"

#include <cmath>

namespace th16 {

namespace matrix {

void identity(float m[16]) {
    for (int k = 0; k < 16; ++k) m[k] = (k % 5 == 0) ? 1.0f : 0.0f;
}

void multiply(float out[16], const float a[16], const float b[16]) {
    float r[16];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            r[i * 4 + j] = a[i * 4 + 0] * b[0 * 4 + j] + a[i * 4 + 1] * b[1 * 4 + j] +
                           a[i * 4 + 2] * b[2 * 4 + j] + a[i * 4 + 3] * b[3 * 4 + j];
        }
    }
    for (int k = 0; k < 16; ++k) out[k] = r[k];
}

namespace {

// 原版的 D3DXMatrixRotationX/Y/Z 经 CRT 的 x87 fsin / fcos 在扩展精度里算完再截成 float；这里用双精度 sin / cos
// 再舍入来逼近。单精度库函数 sinf / cosf 的结果与之经常差 1 ulp（AnmGeometry.cpp 的扇面 / 环带同理），
// 第 2 面整段对拍里几乎每一帧的世界矩阵都因此差 1 ulp
void sin_cos(float angle, float& c, float& s) {
    const double a = angle;
    c = float(std::cos(a));
    s = float(std::sin(a));
}

}  // namespace

void rotation_x(float m[16], float angle) {
    identity(m);
    float c, s;
    sin_cos(angle, c, s);
    m[5] = c; m[6] = s; m[9] = -s; m[10] = c;
}

void rotation_y(float m[16], float angle) {
    identity(m);
    float c, s;
    sin_cos(angle, c, s);
    m[0] = c; m[2] = -s; m[8] = s; m[10] = c;
}

void rotation_z(float m[16], float angle) {
    identity(m);
    float c, s;
    sin_cos(angle, c, s);
    m[0] = c; m[1] = s; m[4] = -s; m[5] = c;
}

void translation(float m[16], float x, float y, float z) {
    identity(m);
    m[12] = x; m[13] = y; m[14] = z;
}

namespace {
void normalize(float v[3]) {
    const float len = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (len == 0.0f) { v[0] = v[1] = v[2] = 0.0f; return; }
    v[0] /= len; v[1] /= len; v[2] /= len;
}
}  // namespace

void look_at_lh(float m[16], const Vec3& eye, const Vec3& at, const Vec3& up) {
    // zaxis = normalize(at - eye)，xaxis = normalize(cross(up, zaxis))，yaxis = cross(zaxis, xaxis)
    float z[3] = {at.x - eye.x, at.y - eye.y, at.z - eye.z};
    normalize(z);
    float x[3] = {up.y * z[2] - up.z * z[1], up.z * z[0] - up.x * z[2], up.x * z[1] - up.y * z[0]};
    normalize(x);
    const float y[3] = {z[1] * x[2] - z[2] * x[1], z[2] * x[0] - z[0] * x[2], z[0] * x[1] - z[1] * x[0]};
    identity(m);
    m[0] = x[0]; m[1] = y[0]; m[2] = z[0];
    m[4] = x[1]; m[5] = y[1]; m[6] = z[1];
    m[8] = x[2]; m[9] = y[2]; m[10] = z[2];
    m[12] = -(x[0] * eye.x + x[1] * eye.y + x[2] * eye.z);
    m[13] = -(y[0] * eye.x + y[1] * eye.y + y[2] * eye.z);
    m[14] = -(z[0] * eye.x + z[1] * eye.y + z[2] * eye.z);
}

void perspective_fov_lh(float m[16], float fov, float aspect, float zn, float zf) {
    // D3DXMatrixPerspectiveFovLH 的舍入顺序：yScale = 1 / tan(fov/2)（双精度后截单），xScale = yScale / aspect，
    // q = zf / (zf − zn)，m[14] = −zn × q。mode 4 投影差分对矩阵 ulp 敏感，顺序需与 oracle 一致
    for (int k = 0; k < 16; ++k) m[k] = 0.0f;
    const float y_scale = float(1.0 / std::tan(double(fov) * 0.5));
    const float q = zf / (zf - zn);
    m[0] = y_scale / aspect;
    m[5] = y_scale;
    m[10] = q;
    m[11] = 1.0f;
    m[14] = -zn * q;
}

void transform_point(const float v[3], const float m[16], float out[4]) {
    // 按 k 逐项累加（D3DXVec3Transform 的单精度顺序）
    for (int j = 0; j < 4; ++j) {
        float acc = v[0] * m[0 * 4 + j];
        acc = acc + v[1] * m[1 * 4 + j];
        acc = acc + v[2] * m[2 * 4 + j];
        out[j] = acc + m[3 * 4 + j];
    }
}

}  // namespace matrix

void Camera::reset_to_viewport_center() {
    // 0x43c780：视口中心 (x + w/2, y + h/2)，相机沿 -Z 退到 (h>>1)/tan(fov/2) 处看向 z=0 平面，up=(0,-1,0)
    const float cx = float(viewport.x) + float(viewport.width) * 0.5f;
    const float cy = float(viewport.height) * 0.5f + float(viewport.y);
    // 0x43dc90：tan 先升双精度再截回单精度（与 tanf 可差 1 ulp，而 mode 4 投影差分对此敏感）
    const float half_tan = float(std::tan(double(fov * 0.5f)));
    const float distance = (float(u32(viewport.height) >> 1) + 0.0f) / half_tan;
    matrix::look_at_lh(view, Vec3{cx, cy, distance}, Vec3{cx, cy, 0.0f}, Vec3{0.0f, -1.0f, 0.0f});
    matrix::perspective_fov_lh(projection, fov, float(viewport.width) / float(viewport.height), 1.0f, 10000.0f);
}

void Camera::look_along_facing() {
    // 0x43c940：eye = pos + pos_offset，at = eye + facing_norm；近 30、远 8000
    matrix::look_at_lh(view, eye(), at(), up);
    matrix::perspective_fov_lh(projection, fov, float(viewport.width) / float(viewport.height), 30.0f, 8000.0f);
    // right = normalize(cross(facing_norm, up))：原版 x = up.z·fn.y − up.y·fn.z 等，即 fn × up
    //（符号决定 mode 4 的第二次投影点落在屏幕哪一侧，但像素长度取模后不受影响）
    float r[3] = {up.z * facing_norm.y - up.y * facing_norm.z,
                  up.x * facing_norm.z - up.z * facing_norm.x,
                  up.y * facing_norm.x - up.x * facing_norm.y};
    matrix::normalize(r);
    right = {r[0], r[1], r[2]};
}

void Camera::project(const float world[3], float& sx, float& sy, float& sz, float& rhw) const {
    const float identity[3] = {0.0f, 0.0f, 0.0f};
    project_translated(identity, world, sx, sy, sz, rhw);
}

void Camera::project_translated(const float translation[3], const float v[3], float& sx, float& sy, float& sz,
                                float& rhw) const {
    // D3DXVec3Project（Wine d3dx9 参考实现的单精度顺序，oracle 的 project_one 与此一致）：
    // m = world · view · projection 先乘成一张矩阵，再 D3DXVec3TransformCoord，最后 X + (1 + x) × W / 2。
    // mode 4 用两次投影之差定尺寸，相减放大 ulp 误差，所以顺序必须逐步照搬
    float world[16];
    matrix::translation(world, translation[0], translation[1], translation[2]);
    float m[16];
    matrix::multiply(m, world, view);
    matrix::multiply(m, m, projection);
    float r[4];
    for (int c = 0; c < 4; ++c) {
        float acc = m[c] * v[0];
        acc = acc + m[4 + c] * v[1];
        acc = acc + m[8 + c] * v[2];
        r[c] = acc + m[12 + c];
    }
    const float w = r[3] != 0.0f ? r[3] : 1.0f;
    const float x = r[0] / w, y = r[1] / w;
    sx = float(viewport.x) + (1.0f + x) * float(viewport.width) / 2.0f;
    sy = float(viewport.y) + (1.0f - y) * float(viewport.height) / 2.0f;
    sz = r[2] / w;
    rhw = 1.0f / w;
}

}  // namespace th16
