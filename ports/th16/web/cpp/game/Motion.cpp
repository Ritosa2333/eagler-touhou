#include "Motion.hpp"

#include <cmath>

namespace th16 {

float wrap_angle(float a) noexcept {
    int n = 0;
    while (a > 3.1415927f) { a -= 6.2831855f; if (++n > 0x21) break; }
    while (a < -3.1415927f) { a += 6.2831855f; if (++n > 0x21) break; }
    return a;
}

// 必须经 double 再舍入：sinf/cosf 会偶发与原版 CRT 差 1 ulp
float sin_f(float a) noexcept { return float(std::sin(double(a))); }
float cos_f(float a) noexcept { return float(std::cos(double(a))); }
float floor_f(float a) noexcept { return float(std::floor(double(a))); }

// 不能先把 cos 舍成 float 再乘（会双重舍入差 1 ulp）
Vec3 polar(float angle, float length) noexcept {
    return {float(std::cos(double(angle)) * double(length)), float(std::sin(double(angle)) * double(length)), 0.0f};
}

float atan2_f(float y, float x) noexcept { return float(std::atan2(double(y), double(x))); }

Vec3 normalize3(const Vec3& v) noexcept {
    const float len = std::sqrt((v.x * v.x + v.y * v.y) + v.z * v.z);
    if (len == 0.0f) return v;
    return {v.x / len, v.y / len, v.z / len};
}

// 0x403d30：把圆心旋到矩形局部坐标后，先判两条"十字"带，再判四个角点到圆心的距离
bool rect_hits_circle(float rect_x, float rect_y, float width, float height, float angle,
                      float cx, float cy, float r) noexcept {
    const float dx = cx - rect_x;
    const float dy = cy - rect_y;
    const float neg_a = -angle;
    const float s = sin_f(neg_a);
    const float c = cos_f(neg_a);
    const float lx = dx * c - dy * s;
    const float ly = dx * s + dy * c;
    const float hw = width * 0.5f;
    const float hh = height * 0.5f;
    const float ax = std::fabs(lx);
    const float ay = std::fabs(ly);
    if (ax <= hw + r && ay <= hh) return true;
    if (ax <= hw && ay <= hh + r) return true;
    const float r2 = r * r;
    const float x0 = lx - hw, y0 = ly - hh;
    if (x0 * x0 + y0 * y0 < r2) return true;
    const float x1 = lx + hw;
    if (x1 * x1 + y0 * y0 < r2) return true;
    const float y1 = ly + hh;
    if (x0 * x0 + y1 * y1 < r2) return true;
    return x1 * x1 + y1 * y1 < r2;
}

namespace {
inline void polar_into(float angle, float length, float& x, float& y) noexcept {
    const Vec3 p = polar(angle, length);
    x = p.x;
    y = p.y;
}
}  // namespace

void MotionState::update_velocity(float game_speed) noexcept {
    switch (mode & 0xf) {
    case 0:
        polar_into(angle, speed * game_speed, velocity.x, velocity.y);
        break;
    case 2:
    case 3:
        radius = radius_speed * game_speed + radius;
        angle = wrap_angle(speed * game_speed + angle);
        break;
    case 4:
        // 0x405420：f30 += radius_speed × 速度后折回；velocity = polar(angle2, speed × 速度)
        f30 = wrap_angle(radius_speed * game_speed + f30);
        polar_into(angle2, speed * game_speed, velocity.x, velocity.y);
        velocity.z = 0.0f;
        break;
    default:
        break;
    }
}

// 0x405390：angle 相对 angle2 的差（跨 ±π 时取另一侧），再折回
static float angle_offset(float angle, float base) noexcept {
    float d = angle - base;
    if (d <= 3.1415927f) {
        if (3.1415927f < base - angle) d = angle - (base - 6.2831855f);
    } else {
        d = angle - (base + 6.2831855f);
    }
    return wrap_angle(d);
}

void MotionState::integrate(float game_speed) noexcept {
    switch (mode & 0xf) {
    case 0:
        pos.x += velocity.x;
        pos.y += velocity.y;
        pos.z += velocity.z;
        break;
    case 2: {
        float lx, ly;
        polar_into(angle, radius, lx, ly);
        pos.x = lx + velocity.x;
        pos.y = velocity.y + ly;
        pos.z = velocity.z + 0.0f;
        break;
    }
    case 3: {
        // 椭圆：以 angle2 为长轴方向、f2c 为长短轴比，围绕圆心（velocity）定位
        float lx, ly;
        polar_into(wrap_angle(angle_offset(angle, angle2)), radius, lx, ly);
        const float sx = f2c * lx;
        const float s = sin_f(angle2), c = cos_f(angle2);
        const float y = velocity.y + (ly * c + s * sx);
        const float x = velocity.x + (c * sx - ly * s);
        pos.x = x;
        pos.y = y;
        pos.z = velocity.z + 0.0f;
        break;
    }
    case 4: {
        // 轨道：pos2 积分速度，再沿 angle2 + π/2 方向叠加 sin(f30) × radius 的横向摆动；angle 取本帧位移方向
        const float old_x = pos.x, old_y = pos.y;
        pos2.x = velocity.x + pos2.x;
        pos2.y = velocity.y + pos2.y;
        pos2.z = velocity.z + pos2.z;
        const float a = wrap_angle(angle2 + 1.5707964f);
        const float len = sin_f(f30) * radius * game_speed;
        float dx, dy;
        polar_into(wrap_angle(a), len, dx, dy);
        pos.x = dx + pos2.x;
        pos.y = pos2.y + dy;
        pos.z = pos2.z + 0.0f;
        angle = wrap_angle(atan2_f(pos.y - old_y, pos.x - old_x));
        break;
    }
    default:
        break;
    }
    pos.x = floor_f(pos.x * 100.0f) / 100.0f;
    pos.y = floor_f(pos.y * 100.0f) / 100.0f;
}

}  // namespace th16
