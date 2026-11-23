// th16 ANM 指令解释器——原版 0x45f980 及其辅助函数的行为镜像。
//
// 分发表：opcode -1..209 经字节表 0x462d8c → 跳转表 0x462cb8；301..613 经 0x462fa8 → 0x462e64。
// 变量：param_mask 第 n 位置位表示第 n 个参数是变量编号（int 经 0x45f610 / 0x45f890，
// float 经 0x45f2d0 / 0x45f780）。
// 每帧顺序（0x45f980）：慢放系数 → 存活计时 → 中断检查 → 执行到 time > 当前时刻 → 速度/角速度（0x4630f0）
// → 插值推进（0x463b30）→ 600 系列几何（0x4632f0）→ 脚本计时 +1。
#include <cmath>
#include <cstring>
#include "AnmGeometry.hpp"
#include "AnmManager.hpp"
#include "RenderDevice.hpp"

namespace th16 {

void AnmVm::clear() noexcept {
    std::memset(static_cast<void*>(this), 0, sizeof *this);
    sprite_source_slot = -1;
}

namespace {

constexpr float kPi = 3.1415927410125732f;
constexpr float kTwoPi = 6.2831854820251465f;

// 0x405340 / 0x402d30 等内联的角度归一化：最多各 0x22 次修正。
float normalize_angle(float a) noexcept {
    int i = 0;
    while (a > kPi) { a -= kTwoPi; if (++i > 0x21) break; }
    while (a < -kPi) { a += kTwoPi; if (++i > 0x21) break; }
    return a;
}

// 0x4033f0：缓动曲线 mode ∈ 1..31，t 为当前帧（浮点），end 为总帧数。
float ease(int mode, float t, float end) noexcept {
    if (end == 0.0f) return 1.0f;
    const float r = t / end;
    auto sinf64 = [](float x) { return float(std::sin(double(x))); };
    switch (mode) {
    case 1: return r * r;
    case 2: return r * r * r;
    case 3: return r * r * r * r;
    case 4: { const float u = 1.0f - r; return 1.0f - u * u; }
    case 5: { const float u = 1.0f - r; return 1.0f - u * u * u; }
    case 6: { const float u = 1.0f - r; return 1.0f - u * u * u * u; }
    case 9: { const float t2 = r + r; if (1.0f > t2) return t2 * t2 * 0.5f; const float u = 2.0f - t2; return (2.0f - u * u) * 0.5f; }
    case 10: { const float t2 = r + r; if (1.0f > t2) return t2 * t2 * t2 * 0.5f; const float u = 2.0f - t2; return (2.0f - u * u * u) * 0.5f; }
    case 11: { const float t2 = r + r; if (1.0f > t2) return t2 * t2 * t2 * t2 * 0.5f; const float u = 2.0f - t2; return (2.0f - u * u * u * u) * 0.5f; }
    case 12: { const float t2 = r + r; if (1.0f > t2) { const float u = 1.0f - t2; return 0.5f - u * u * 0.5f; } const float u = t2 - 1.0f; return u * u * 0.5f + 0.5f; }
    case 13: { const float t2 = r + r; if (1.0f > t2) { const float u = 1.0f - t2; return 0.5f - u * u * u * 0.5f; } const float u = t2 - 1.0f; return u * u * u * 0.5f + 0.5f; }
    case 14: { const float t2 = r + r; if (1.0f > t2) { const float u = 1.0f - t2; return 0.5f - u * u * u * u * 0.5f; } const float u = t2 - 1.0f; return u * u * u * 0.5f + 0.5f; }  // 后半为三次（原版如此）
    case 16: return 1.0f;
    case 18: return sinf64(r * kPi * 0.5f);
    case 19: return 1.0f - sinf64(r * kPi * 0.5f + 1.5707963705062866f);
    case 20: { const float t2 = r + r; const float x = t2 * kPi * 0.5f; if (1.0f > t2) return sinf64(x) * 0.5f; return (1.0f - sinf64(x)) * 0.5f + 0.5f; }
    case 21: { const float t2 = r + r; if (1.0f > t2) return (1.0f - sinf64(t2 * kPi * 0.5f + 1.5707963705062866f)) * 0.5f; const float u = t2 - 1.0f; return sinf64(u * kPi * 0.5f) * 0.5f + 0.5f; }
    case 22: case 23: case 24: case 25: case 26:
    case 27: case 28: case 29: case 30: case 31: {
        static const float c[5][4] = {
            {0.25f, 0.5625f, 0.1111111119389534f, 0.8888888955116272f},
            {0.30000001192092896f, 0.4899999797344208f, 0.18367348611354828f, 0.8163264989852905f},
            {0.3499999940395355f, 0.42249995470046997f, 0.28994083404541016f, 0.7100591659545898f},
            {0.3799999952316284f, 0.38440001010894775f, 0.3756503462791443f, 0.6243496537208557f},
            {0.4000000059604645f, 0.36000001430511475f, 0.4444444477558136f, 0.5555555820465088f},
        };
        const float* k = c[(mode - 22) % 5];
        if (mode <= 26) { float v = r - k[0]; v = v * v; v = v / k[1]; v = v - k[2]; return v / k[3]; }
        float v = (1.0f - r) - k[0]; v = v * v; v = v / k[1]; v = v - k[2]; v = v / k[3];
        return 1.0f - v;
    }
    default: return r;  // 0、7、8、15、17 及越界：线性
    }
}

// 0x406190 式计时器推进（表下标被强制为 0 → 游戏速度）。
void tick_timer(i32& previous, i32& current, float& current_f, i32& slot, float game_speed) noexcept {
    if (slot != 0) slot = 0;
    previous = current;
    if (game_speed <= 0.99f || 1.01f <= game_speed) {
        current_f = current_f + game_speed;
        current = i32(current_f);
    } else {
        current = current + 1;
        current_f = current_f + 1.0f;
    }
}

template <typename T> T add(const T& a, const T& b);
template <> [[maybe_unused]] float add(const float& a, const float& b) { return a + b; }
template <> [[maybe_unused]] i32 add(const i32& a, const i32& b) { return a + b; }
template <> [[maybe_unused]] Vec2 add(const Vec2& a, const Vec2& b) { return {a.x + b.x, a.y + b.y}; }
template <> [[maybe_unused]] Vec3 add(const Vec3& a, const Vec3& b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
template <> [[maybe_unused]] IVec3 add(const IVec3& a, const IVec3& b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }

}  // namespace

// Hermite：goal*h01 + initial*h00 + b1*h10 + b2*h11（乘加顺序与原版一致；0x4258b0 敌机位置插值同一公式）
float hermite(float initial, float goal, float b1, float b2, float r) noexcept {
    const float s = r - 1.0f;
    const float h11 = s * r * r;
    const float h00 = s * s * (r + r + 1.0f);
    const float h01 = r * r * (3.0f - (r + r));
    const float h10 = (1.0f - r) * (1.0f - r) * r;
    return goal * h01 + initial * h00 + b1 * h10 + b2 * h11;
}

namespace {
float lerp(float initial, float goal, float f) noexcept { return (goal - initial) * f + initial; }

template <typename T> T hermite_v(const T& i, const T& g, const T& b1, const T& b2, float r);
template <> float hermite_v(const float& i, const float& g, const float& b1, const float& b2, float r) { return hermite(i, g, b1, b2, r); }
template <> Vec2 hermite_v(const Vec2& i, const Vec2& g, const Vec2& b1, const Vec2& b2, float r) {
    return {hermite(i.x, g.x, b1.x, b2.x, r), hermite(i.y, g.y, b1.y, b2.y, r)};
}
template <> Vec3 hermite_v(const Vec3& i, const Vec3& g, const Vec3& b1, const Vec3& b2, float r) {
    return {hermite(i.x, g.x, b1.x, b2.x, r), hermite(i.y, g.y, b1.y, b2.y, r), hermite(i.z, g.z, b1.z, b2.z, r)};
}
// 0x464590：整数三元组按分量先各自截断再求和
template <> IVec3 hermite_v(const IVec3& i, const IVec3& g, const IVec3& b1, const IVec3& b2, float r) {
    const float s = r - 1.0f;
    const float h11 = s * r * r;
    const float h00 = s * s * (r + r + 1.0f);
    const float h10 = (1.0f - r) * (1.0f - r) * r;
    const float h01 = r * r * (3.0f - (r + r));
    auto one = [&](i32 a, i32 b, i32 c, i32 d) {
        return i32(float(a) * h00) + i32(float(b) * h01) + i32(float(c) * h10) + i32(float(d) * h11);
    };
    return {one(i.x, g.x, b1.x, b2.x), one(i.y, g.y, b1.y, b2.y), one(i.z, g.z, b1.z, b2.z)};
}
// 0x4643b0：整数单值 Hermite（先浮点求和再截断）
i32 hermite_int(i32 i, i32 g, i32 b1, i32 b2, float r) noexcept {
    const float s = r - 1.0f;
    return i32(r * r * (3.0f - (r + r)) * float(g) + s * s * (r + r + 1.0f) * float(i) +
               (1.0f - r) * (1.0f - r) * r * float(b1) + s * r * r * float(b2));
}

template <typename T> T lerp_v(const T& i, const T& g, float f);
template <> float lerp_v(const float& i, const float& g, float f) { return lerp(i, g, f); }
template <> Vec2 lerp_v(const Vec2& i, const Vec2& g, float f) { return {lerp(i.x, g.x, f), lerp(i.y, g.y, f)}; }
template <> Vec3 lerp_v(const Vec3& i, const Vec3& g, float f) { return {lerp(i.x, g.x, f), lerp(i.y, g.y, f), lerp(i.z, g.z, f)}; }
template <> IVec3 lerp_v(const IVec3& i, const IVec3& g, float f) {
    return {i32(float(g.x - i.x) * f) + i.x, i32(float(g.y - i.y) * f) + i.y, i32(float(g.z - i.z) * f) + i.z};
}
template <> i32 lerp_v(const i32& i, const i32& g, float f) { return i32(f * float(g - i) + i); }

// 0x406e10 / 0x463d40 / 0x464590 / 0x4643b0 / 0x4171c0：通用插值推进，返回当前值。
// 到达终点时 end_time 清零并输出 goal（mode 7/0x11 输出 initial）。
template <typename T>
T step_interp(Interp<T>& it, float game_speed) {
    const i32 end = it.end_time;
    bool finished = end == 0;
    if (end > 0) {
        tick_timer(it.timer.previous, it.timer.current, it.timer.current_f, it.timer.speed_slot, game_speed);
        if (end <= it.timer.current) {
            it.timer.set(end);
            it.end_time = 0;
            finished = true;
        }
    }
    if (finished) {
        if (it.mode != 7 && it.mode != 0x11) return it.goal;
        return it.initial;
    }
    if (it.mode == 7) {
        it.initial = add(it.initial, it.goal);
        it.current = it.initial;
    } else if (it.mode == 0x11) {
        it.initial = add(it.initial, it.bezier1);
        it.bezier1 = add(it.bezier1, it.goal);
        it.current = it.initial;
    } else if (it.mode == 8) {
        it.current = hermite_v(it.initial, it.goal, it.bezier1, it.bezier2, it.timer.current_f / float(end));
    } else {
        it.current = lerp_v(it.initial, it.goal, ease(it.mode, it.timer.current_f, float(end)));
    }
    return it.current;
}

// 0x4643b0 的整数版本与模板主体相同但 Hermite 求和顺序不同
i32 step_interp_int(Interp<i32>& it, float game_speed) {
    const i32 end = it.end_time;
    bool finished = end == 0;
    if (end > 0) {
        tick_timer(it.timer.previous, it.timer.current, it.timer.current_f, it.timer.speed_slot, game_speed);
        if (end <= it.timer.current) {
            if ((it.timer.flags & 1) == 0) { it.timer.current = 0; it.timer.previous = -999999; it.timer.current_f = 0; it.timer.speed_slot = 0; it.timer.flags |= 1; }
            it.timer.current = end; it.timer.previous = end - 1; it.timer.current_f = float(end);
            it.end_time = 0;
            finished = true;
        }
    }
    if (finished) return (it.mode != 7 && it.mode != 0x11) ? it.goal : it.initial;
    if (it.mode == 7) { it.initial += it.goal; it.current = it.initial; return it.current; }
    if (it.mode == 0x11) { it.initial += it.bezier1; it.bezier1 += it.goal; it.current = it.initial; return it.current; }
    if (it.mode == 8) { it.current = hermite_int(it.initial, it.goal, it.bezier1, it.bezier2, it.timer.current_f / float(end)); return it.current; }
    it.current = lerp_v(it.initial, it.goal, ease(it.mode, it.timer.current_f, float(end)));
    return it.current;
}

// 0x464080：角度插值（ins_411），线性模式取最短弧
float step_interp_angle(Interp<float>& it, float game_speed) {
    const i32 end = it.end_time;
    if (end > 0) {
        tick_timer(it.timer.previous, it.timer.current, it.timer.current_f, it.timer.speed_slot, game_speed);
        if (it.timer.current < end) goto running;
        it.timer.set(end);
        it.end_time = 0;
    } else if (end != 0) {
        goto running;
    }
    return (it.mode != 7 && it.mode != 0x11) ? it.goal : it.initial;
running:
    if (it.mode == 7) {
        it.initial = normalize_angle(it.goal + it.initial);
        it.current = it.initial;
        return it.current;
    }
    if (it.mode == 0x11) {
        it.initial = normalize_angle(it.bezier1 + it.initial);
        it.bezier1 = normalize_angle(it.goal + it.bezier1);
        it.current = it.initial;
        return it.current;
    }
    if (it.mode == 8) {
        const float r = it.timer.current_f / float(end);
        const float s = r - 1.0f;
        const float h11 = s * r * r, h00 = s * s * (r + r + 1.0f), h01 = r * r * (3.0f - (r + r)), h10 = (1.0f - r) * (1.0f - r) * r;
        // 0x4475f0 ×4 → 0x447650 ×3：各项归一化后依次相加归一化
        const float a = normalize_angle(it.bezier2 * h11);
        const float b = normalize_angle(it.bezier1 * h10);
        const float c = normalize_angle(it.goal * h01);
        float v = normalize_angle(it.initial * h00);
        v = normalize_angle(v + c);
        v = normalize_angle(v + b);
        v = normalize_angle(v + a);
        it.current = v;
        return v;
    }
    {
        const float f = ease(it.mode, it.timer.current_f, float(end));
        // 0x405390：goal 相对 initial 的最短差值，再乘系数（0x4475f0）并加回 initial（0x447650）
        const float i = it.initial, g = it.goal;
        float d = i - g;
        if (d <= kPi) { if (kPi < g - i) d = i - (g - kTwoPi); }
        else d = i - (g + kTwoPi);
        d = normalize_angle(d);
        // 注意 0x405390 计算的是 initial-goal，随后 0x4475f0 乘 f 再加 initial —— 原版如此
        const float scaled = normalize_angle(d * f);
        it.current = normalize_angle(scaled + i);
        return it.current;
    }
}

struct Ins {
    const u8* p;
    u16 type() const { return read_u16(p); }
    u16 length() const { return read_u16(p + 2); }
    i16 time() const { return i16(read_u16(p + 4)); }
    u16 mask() const { return read_u16(p + 6); }
    i32 i(int n) const { return read_i32(p + 8 + 4 * n); }
    float f(int n) const { return read_f32(p + 8 + 4 * n); }
    u16 h(int n) const { return read_u16(p + 8 + 2 * n); }
    bool var(int n) const { return (mask() >> n) & 1; }
};

}  // namespace

Vec3 step_interp_vec3(Interp<Vec3>& it, float game_speed) { return step_interp(it, game_speed); }
float step_interp_float(Interp<float>& it, float game_speed) { return step_interp(it, game_speed); }
Vec2 step_interp_vec2(Interp<Vec2>& it, float game_speed) { return step_interp(it, game_speed); }
float interp_ease(int mode, float t, float end) noexcept { return ease(mode, t, end); }

Vec3 anm_world_rotation(AnmVm& v) {
    v.world_rotation = v.rotation;
    if (v.root != nullptr && (v.flags_hi & AnmVm::kHiDetached) == 0) {
        const Vec3 p = anm_world_rotation(*v.root);
        v.world_rotation.x = p.x + v.world_rotation.x;
        v.world_rotation.y = p.y + v.world_rotation.y;
        v.world_rotation.z = p.z + v.world_rotation.z;
        v.rotation.x = normalize_angle(v.rotation.x);
        v.rotation.y = normalize_angle(v.rotation.y);
        v.rotation.z = normalize_angle(v.rotation.z);
    }
    return v.world_rotation;
}

// 0x45edb0
int anm_set_sprite(AnmFile& f, AnmVm& vm, i32 index) {
    if (f.resource == nullptr) return -1;
    vm.sprite_index = index;
    vm.sprite_source_slot = f.slot;
    const AnmRuntimeSprite& s = f.sprites[index];
    vm.uv[0] = {s.u0, s.v0};
    vm.uv[1] = {s.u1, s.v0};
    vm.uv[2] = {s.u0, s.v1};
    vm.uv[3] = {s.u1, s.v1};
    vm.sprite_size = {s.width, s.height};
    for (float* mtx : {vm.texture_matrix, vm.sprite_matrix}) {
        for (int k = 0; k < 16; ++k) mtx[k] = (k % 5 == 0) ? 1.0f : 0.0f;
    }
    vm.texture_matrix[0] = vm.sprite_size.x * 0.00390625f;
    vm.texture_matrix[5] = vm.sprite_size.y * 0.00390625f;
    vm.sprite_matrix[0] = (vm.sprite_size.x / s.tex_w) * s.ratio_x;
    vm.sprite_matrix[5] = (vm.sprite_size.y / s.tex_h) * s.ratio_y;
    std::memcpy(vm.texture_matrix_copy, vm.texture_matrix, sizeof vm.texture_matrix);
    return 0;
}

Vec3 anm_world_position(AnmVm& v) {
    // 三个分量的加法顺序照原版（y / z 先加 pos2），避免单精度舍入差异
    Vec3 p{v.pos.x + v.entity_offset.x + v.pos2.x,
           v.pos2.y + v.pos.y + v.entity_offset.y,
           v.pos2.z + v.pos.z + v.entity_offset.z};
    if (v.root == nullptr || (v.flags_hi & AnmVm::kHiDetached) != 0) return p;
    if ((v.flags_hi & AnmVm::kHi314) != 0) {
        const float s = float(std::sin(double(v.root->rotation.z)));
        const float c = float(std::cos(double(v.root->rotation.z)));
        const float x = p.x;
        p.x = x * c - p.y * s;
        p.y = p.y * c + x * s;
    }
    const Vec3 parent = anm_world_position(*v.root);
    p.x += parent.x;
    p.y += parent.y;
    p.z += parent.z;
    return p;
}

// 0x406d80
void anm_set_layer(AnmVm& vm, i32 layer) noexcept {
    vm.layer = layer;
    if (layer < 3 || 0x13 < layer) {
        if (layer < 0x14 || 0x17 < layer) vm.flags_hi &= 0xfff3ffffu;
        else vm.flags_hi = (vm.flags_hi & 0xfffbffffu) | 0x80000u;
    } else {
        vm.flags_hi = (vm.flags_hi & 0xfff7ffffu) | 0x40000u;
    }
    if ((0x13 < layer && layer < 0x20) || (0x23 < layer && layer < 0x2b)) vm.flags_hi = (vm.flags_hi & 0xff9fffffu) | 0x100000u;
}

void Timer::tick(float game_speed) noexcept { tick_timer(previous, current, current_f, speed_slot, game_speed); }

void Timer::untick(float game_speed) noexcept {
    if (speed_slot != 0) speed_slot = 0;
    previous = current;
    if (game_speed <= 0.99f || 1.01f <= game_speed) {
        current_f = current_f - game_speed * 1.0f;
        current = i32(current_f);
    } else {
        current_f = current_f - 1.0f;
        current = i32(current_f);
    }
}

void Timer::advance(float n, float game_speed) noexcept {
    if (speed_slot != 0) speed_slot = 0;
    previous = current;
    if (game_speed <= 0.99f || 1.01f <= game_speed) {
        current_f = game_speed * n + current_f;
        current = i32(current_f);
    } else {
        current_f = current_f + n;
        current = i32(current_f);
    }
}

void Timer::rewind(i32 n, float game_speed) noexcept {
    if (speed_slot != 0) speed_slot = 0;
    previous = current;
    if (game_speed <= 0.99f || 1.01f <= game_speed) {
        current_f = game_speed * float(-n) + current_f;
        current = i32(current_f);
    } else {
        current_f = current_f + float(-n);
        current = i32(current_f);
    }
}

namespace {

// 0x464d60：x87 cos / sin × 半径后截成 float（与 Motion.cpp 的 polar 同口径：先 double 乘再舍入）
void polar_into(float* xy, float angle, float radius) noexcept {
    xy[0] = float(std::cos(double(angle)) * double(radius));
    xy[1] = float(std::sin(double(angle)) * double(radius));
}

// 0x4632f0 的半径：外 / 内半径 = scale.y ± scale.x / 2，随父缩放（ins_507 分离除外；原版外半径乘父 scale.x、
// 内半径乘父 scale.y），再按 ins_313 的 scale mode 乘窗口缩放
void strip_radii(const AnmVm& vm, float window_scale, float& outer, float& inner) noexcept {
    const float half = vm.scale.x * 0.5f;
    outer = vm.scale.y + half;
    inner = vm.scale.y - half;
    if (vm.parent != nullptr && (vm.flags_hi & AnmVm::kHiDetached) == 0) {
        outer = outer * vm.parent->scale.x;
        inner = inner * vm.parent->scale.y;
    }
    const u32 mode = vm.flags_hi & AnmVm::kHiScaleModeMask;
    if (mode == 0x100000u) { outer = outer * window_scale; inner = inner * window_scale; }
    else if (mode == 0x200000u) { inner = inner * window_scale * 0.5f; outer = outer * window_scale * 0.5f; }
}

// 0x4632f0：600 系列条带几何——每帧 VM 更新末尾按当前位置 / 旋转 / 缩放重算宿主顶点缓冲（0x468490 mode 9 / 0xd / 0xe
// 经 0x4681f0 以 XYZRHW|DIFFUSE|TEX1 三角条带绘制，每采样点外 / 内两个顶点）：
//   mode 9（ins_600）  整圈环带：$I0 个采样点（首尾重合），起角 = rotation.z，V 沿环累加到 $I1 圈
//   mode 0xd（ins_601）圆弧带：张角 rotation.x，起角 = rotation.z − rotation.x / 2
//   mode 0xe（ins_602）圆弧带：起角 = rotation.z（Boss 血条 front.anm 244：38 点、张角 −2π）
//   mode 0x18 / 0x19（ins_609 / 610）世界空间圆柱带——本作 ANM 未使用，不生成
// 原版缓冲由 ins_600 族按参数 malloc（$I0 × 0x38 字节，未初始化）；这里在首次生成 / 点数增大时按 $I0 分配，
// 其余语义相同（点数 < 2 时原版除以 0 得 ±∞ / NaN 角步，此处照算）。
void generate_strip_geometry(AnmVm& vm, const AnmGeometry& geometry) {
    const u32 mode = vm.render_mode();
    if (mode != 9 && mode != 0xd && mode != 0xe) return;
    const i32 nodes = vm.int_vars[0];
    if (nodes <= 0) return;
    const u32 bytes = u32(nodes) * 2u * sizeof(ScreenColorUvVertex);
    if (vm.vertex_buffer == nullptr || vm.vertex_buffer_size < bytes) {
        if (vm.vertex_buffer != nullptr && vm.vertex_buffer_free != nullptr) vm.vertex_buffer_free(vm.vertex_buffer);
        vm.vertex_buffer = new ScreenColorUvVertex[size_t(nodes) * 2];
        vm.vertex_buffer_size = bytes;
        vm.vertex_buffer_free = [](void* p) { delete[] static_cast<ScreenColorUvVertex*>(p); };
    }
    auto* out = static_cast<ScreenColorUvVertex*>(vm.vertex_buffer);
    const Vec3 world = geometry.world_position(vm);   // 0x406c40
    float outer, inner;
    strip_radii(vm, geometry.window_scale, outer, inner);
    const float u_outer = vm.uv_scroll.x + vm.uv[0].x;
    const float u_inner = vm.uv[1].x + vm.uv_scroll.x;
    u32 color;
    std::memcpy(&color, vm.color, 4);
    auto emit = [&](ScreenColorUvVertex* pair, float angle, float v) {
        pair[0].rhw = 1.0f; pair[0].color = color; pair[0].u = u_outer; pair[0].v = v;
        polar_into(&pair[0].x, angle, outer);
        pair[0].z = 0.0f;
        pair[0].x = pair[0].x + world.x; pair[0].y = world.y + pair[0].y; pair[0].z = pair[0].z + world.z;
        pair[1].rhw = 1.0f; pair[1].color = color; pair[1].u = u_inner; pair[1].v = v;
        polar_into(&pair[1].x, angle, inner);
        pair[1].z = 0.0f;
        pair[1].x = pair[1].x + world.x; pair[1].y = world.y + pair[1].y; pair[1].z = pair[1].z + world.z;
    };

    if (mode == 9) {
        const i32 segments = nodes - 1;
        const float angle_step = kTwoPi / float(segments);
        const float v_step = float(vm.int_vars[1]) / float(segments);
        float angle = vm.rotation.z;
        float v = 0.0f;
        for (i32 i = 0; i < segments; ++i) {
            emit(out + i * 2, angle, vm.uv_scroll.y + v);
            v = v + v_step;
            angle = normalize_angle(angle + angle_step);
        }
        // 末对 = 首对坐标（闭合），V 取累加到整圈的值
        out[segments * 2] = out[0];
        out[segments * 2].v = vm.uv_scroll.y + v;
        out[segments * 2 + 1] = out[1];
        out[segments * 2 + 1].v = vm.uv_scroll.y + v;
        return;
    }
    // mode 0xd / 0xe：颜色按 color mode 取 color / color2
    if ((vm.flags_lo & AnmVm::kColorModeMask) != 0) std::memcpy(&color, vm.color2, 4);
    float angle = mode == 0xe ? normalize_angle(vm.rotation.z) : normalize_angle(vm.rotation.z - vm.rotation.x * 0.5f);
    const float angle_step = vm.rotation.x / float(nodes - 1);
    const float v_step = float(vm.int_vars[1]) / float(nodes - 1);
    float v = 0.0f;
    for (i32 i = 0; i < nodes; ++i) {
        emit(out + i * 2, angle, vm.uv_scroll.y + v);
        v = v_step + v;
        angle = normalize_angle(angle + angle_step);
    }
}

}  // namespace

struct AnmInterpreter {
    AnmManager& m;
    AnmVm& vm;
    AnmFile& file;

    // 0x45f610
    i32 int_var(i32 id) {
        switch (id) {
        case 10000: case 10001: case 10002: case 10003: return vm.int_vars[id - 10000];
        case 10004: case 10005: case 10006: case 10007: return i32(vm.float_vars[id - 10004]);
        case 10008: return vm.int_vars2[0];
        case 10009: return vm.int_vars2[1];
        case 10022: return vm.rand_int_max != 0 ? i32(m.game_rng.next32() % u32(vm.rand_int_max)) : 0;
        case 10027: return i32(vm.rand_scale);
        case 10028: return i32(vm.rand_angle_scale);
        case 10029: return vm.rand_int_max;
        case 10033: case 10034: case 10035: return i32(vm.float_vars2[id - 10033]);
        default: return id;
        }
    }
    // 0x45f2d0（xmm1 = 原始 float，按 (int) 取变量号）
    float float_var(float raw) {
        switch (i32(raw)) {
        case 10000: case 10001: case 10002: case 10003: return float(vm.int_vars[i32(raw) - 10000]);
        case 10004: case 10005: case 10006: case 10007: return vm.float_vars[i32(raw) - 10004];
        case 10008: return float(vm.int_vars2[0]);
        case 10009: return float(vm.int_vars2[1]);
        case 10010: return m.game_rng.signed_unit() * vm.rand_angle_scale;
        case 10011: return m.game_rng.unit() * vm.rand_scale;
        case 10012: return m.game_rng.signed_unit() * vm.rand_scale;
        case 10013: return vm.pos.x;
        case 10014: return vm.pos.y;
        case 10015: return vm.pos.z;
        case 10016: case 10017: case 10018: {  // DAT_004c16c8 + DAT_004c168c：相机 pos_offset + pos
            if (m.camera_pos == nullptr || m.camera_pos_offset == nullptr) return 0.0f;
            const int k = i32(raw) - 10016;
            return (&m.camera_pos_offset->x)[k] + (&m.camera_pos->x)[k];
        }
        case 10019: case 10020: case 10021:  // DAT_004c16b0..b8：相机 facing_norm
            return m.camera_facing_norm != nullptr ? (&m.camera_facing_norm->x)[i32(raw) - 10019] : 0.0f;
        case 10022: return m.game_rng.next_unsigned_float();
        case 10023: return vm.rotation.x;
        case 10024: return vm.rotation.y;
        case 10025: return vm.rotation.z;
        case 10026: return world_rotation().z;
        case 10027: return vm.rand_scale;
        case 10028: return vm.rand_angle_scale;
        case 10029: return float(vm.rand_int_max);
        case 10030: return m.visual_rng.signed_unit() * vm.rand_angle_scale;
        case 10031: return m.visual_rng.unit() * vm.rand_scale;
        case 10032: return m.visual_rng.signed_unit() * vm.rand_scale;
        case 10033: case 10034: case 10035: return vm.float_vars2[i32(raw) - 10033];
        default: return raw;
        }
    }
    // 0x45f890 / 0x45f780：可写变量地址（非变量则写入指令自身参数——原版行为，这里写到临时槽）
    i32* int_ref(const Ins& ins, int n) {
        const i32 id = ins.i(n);
        switch (id) {
        case 10000: case 10001: case 10002: case 10003: return &vm.int_vars[id - 10000];
        case 10008: return &vm.int_vars2[0];
        case 10009: return &vm.int_vars2[1];
        case 10029: return &vm.rand_int_max;
        default: scratch_i = id; return &scratch_i;
        }
    }
    float* float_ref(const Ins& ins, int n) {
        const float raw = ins.f(n);
        switch (i32(raw)) {
        case 10004: case 10005: case 10006: case 10007: return &vm.float_vars[i32(raw) - 10004];
        case 10013: return &vm.pos.x; case 10014: return &vm.pos.y; case 10015: return &vm.pos.z;
        case 10023: return &vm.rotation.x; case 10024: return &vm.rotation.y; case 10025: return &vm.rotation.z;
        case 10027: return &vm.rand_scale;
        case 10028: return &vm.rand_angle_scale;
        case 10033: case 10034: case 10035: return &vm.float_vars2[i32(raw) - 10033];
        default: scratch_f = raw; return &scratch_f;
        }
    }
    i32 scratch_i = 0;
    float scratch_f = 0;

    i32 argi(const Ins& ins, int n) { return ins.var(n) ? int_var(ins.i(n)) : ins.i(n); }
    float argf(const Ins& ins, int n) { return ins.var(n) ? float_var(ins.f(n)) : ins.f(n); }
    i32* refi(const Ins& ins, int n) { if (ins.var(n)) return int_ref(ins, n); scratch_i = ins.i(n); return &scratch_i; }
    float* reff(const Ins& ins, int n) { if (ins.var(n)) return float_ref(ins, n); scratch_f = ins.f(n); return &scratch_f; }

    Vec3 world_rotation() { return anm_world_rotation(vm); }

    const u8* script_base() { return file.scripts[vm.script_index]->bytes.data(); }

    int set_sprite(AnmFile& f, i32 index) { return anm_set_sprite(f, vm, index); }

    void set_layer(i32 layer) { anm_set_layer(vm, layer); }

    static void set_bit(u32& flags, u32 mask, int shift, u32 value) { flags ^= ((value << shift) ^ flags) & mask; }

    void begin_vec3_interp(Interp<Vec3>& it, i32 time, i32 mode, const Vec3& initial, const Vec3& goal) {
        anm_begin_vec3_interp(it, time, mode, initial, goal);
    }
    void begin_color_interp(Interp<IVec3>& it, i32 time, i32 mode, const u8* cur, u8 b, u8 g, u8 r) {
        anm_begin_color_interp(it, time, mode, cur, b, g, r);
    }
    void begin_scalar_interp(Interp<i32>& it, i32 time, i32 mode, i32 initial, i32 goal) {
        anm_begin_scalar_interp(it, time, mode, initial, goal);
    }
    void begin_vec2_interp(Interp<Vec2>& it, i32 time, i32 mode, const Vec2& initial, const Vec2& goal) {
        anm_begin_vec2_interp(it, time, mode, initial, goal);
    }

    // 子 VM 创建（ins_500 系列）
    void create_child(const Ins& ins, u32 list_flags) {
        u32 id = 0;
        m.create_child(file, argi(ins, 0), vm, list_flags, &id);
    }

    // 主体：返回 1 表示应删除
    int run() {
        const float saved_speed = m.game_speed;
        if (vm.flags_hi & AnmVm::kHiIgnoreSpeed) m.game_speed = 1.0f;
        // 0x406340：沿 root 链取慢放系数
        const float slow = AnmManager::root_slowdown(vm);
        if (0.0f < slow) {
            float speed = saved_speed - slow * saved_speed;
            if (speed < 0.0f) speed = 0.0f;
            m.game_speed = speed;
        }
        int result = 0;
        if (vm.update_callback != 0 && m.hooks != nullptr && m.hooks->update(vm) != 0) {
            // 0x462be6：回调返回非零 → 本帧到此为止并删除 VM
            m.game_speed = saved_speed;
            return 1;
        }
        if (vm.ins_offset >= 0 && (vm.flags_lo & AnmVm::kPaused) == 0) {
            vm.age.tick(m.game_speed);
            bool execute = true;
            if (vm.pending_interrupt != 0) {
                execute = handle_interrupt();
            } else if ((vm.flags_hi & AnmVm::kHiPauseMask) == 0x4000 && m.paused) {
                m.game_speed = saved_speed;
                return 0;
            }
            if (execute) {
                const int r = execute_instructions();
                if (r >= 0) { m.game_speed = saved_speed; return r; }
            }
            end_of_frame();
        }
        m.game_speed = saved_speed;
        return result;
    }

    // LAB_0045fac6：查找中断标签；返回 true 继续执行指令，false 表示进入帧末处理
    bool handle_interrupt() {
        const u8* base = script_base();
        const u8* p = base;
        const u8* fallback = nullptr;
        u32 fallback_offset = 0, offset = 0;
        for (;;) {
            const u16 type = read_u16(p);
            if (type == 5 && read_i32(p + 8) == vm.pending_interrupt) break;
            if (type == 0xffff) break;
            if (type == 5 && read_i32(p + 8) == -1) { fallback = p; fallback_offset = offset; }
            offset += read_u16(p + 2);
            p += read_u16(p + 2);
        }
        vm.flags_lo &= ~AnmVm::kStopped;
        vm.pending_interrupt = 0;
        if (read_u16(p) != 5) {
            if (fallback == nullptr) {
                // 0x4629fa：找不到标签时只回退计时；kStopped 已在 0x45fb21 清掉且不再置位（0x4629f0 被跳过）
                vm.time.untick(m.game_speed);
                return false;
            }
            offset = fallback_offset;
            p = fallback;
        }
        vm.interrupt_return_time.set(vm.time.current);
        vm.interrupt_return_offset = vm.ins_offset;
        vm.time.set(i16(read_u16(p + 4)));
        vm.flags_lo |= AnmVm::kVisible;
        vm.ins_offset = i32(offset);
        return true;
    }

    void jump(const Ins& ins, i32 offset, i32 time) { vm.time.set(time); vm.ins_offset = offset; (void)ins; }
    void next(const Ins& ins) { vm.ins_offset += ins.length(); }

    // LAB_0045fba0 循环；返回 -1 表示继续帧末处理，0/1 表示直接返回该值
    int execute_instructions() {
        for (;;) {
            const Ins ins{script_base() + vm.ins_offset};
            if (i32(ins.time()) > vm.time.current) return -1;
            const int op = ins.type() == 0xffff ? -1 : int(ins.type());
            switch (op) {
            case -1: case 1:
                vm.flags_lo &= ~AnmVm::kVisible;
                vm.ins_offset = -1;
                return 1;
            case 2:
                vm.ins_offset = -1;
                return 0;
            case 4:
                vm.flags_lo &= ~AnmVm::kVisible;
                [[fallthrough]];
            case 3:
                if (vm.pending_interrupt == 0) {
                    vm.flags_lo |= AnmVm::kStopped;
                    vm.time.untick(m.game_speed);
                    return -1;
                }
                if (!handle_interrupt()) return -1;
                continue;
            case 6: {
                const i32 n = argi(ins, 0);
                vm.time.previous = vm.time.current;
                vm.time.current_f = vm.time.current_f - float(n);
                vm.time.current = i32(vm.time.current_f);
                next(ins);
                continue;
            }
            case 7:
                vm.time.set(vm.interrupt_return_time.current);
                vm.ins_offset = vm.interrupt_return_offset;
                continue;
            case 100: *refi(ins, 0) = argi(ins, 1); break;
            case 101: *reff(ins, 0) = argf(ins, 1); break;
            case 102: { const i32 b = argi(ins, 1); i32* a = refi(ins, 0); *a = *a + b; break; }
            case 103: { const float b = argf(ins, 1); float* a = reff(ins, 0); *a = *a + b; break; }
            case 104: { const i32 b = argi(ins, 1); i32* a = refi(ins, 0); *a = *a - b; break; }
            case 105: { const float b = argf(ins, 1); float* a = reff(ins, 0); *a = *a - b; break; }
            case 106: { const i32 b = argi(ins, 1); i32* a = refi(ins, 0); *a = *a * b; break; }
            case 107: { const float b = argf(ins, 1); float* a = reff(ins, 0); *a = *a * b; break; }
            case 108: { const i32 b = argi(ins, 1); i32* a = refi(ins, 0); *a = *a / b; break; }
            case 109: { const float b = argf(ins, 1); float* a = reff(ins, 0); *a = *a / b; break; }
            case 110: { const i32 b = argi(ins, 1); i32* a = refi(ins, 0); *a = *a % b; break; }
            case 111: { const float b = argf(ins, 1); const float a = argf(ins, 0); *reff(ins, 0) = float(std::fmod(double(a), double(b))); break; }
            case 112: { const i32 a = argi(ins, 1), b = argi(ins, 2); *refi(ins, 0) = a + b; break; }
            case 113: { const float a = argf(ins, 1), b = argf(ins, 2); *reff(ins, 0) = a + b; break; }
            case 114: { const i32 a = argi(ins, 1), b = argi(ins, 2); *refi(ins, 0) = a - b; break; }
            case 115: { const float a = argf(ins, 1), b = argf(ins, 2); *reff(ins, 0) = a - b; break; }
            case 116: { const i32 a = argi(ins, 1), b = argi(ins, 2); *refi(ins, 0) = a * b; break; }
            case 117: { const float a = argf(ins, 1), b = argf(ins, 2); *reff(ins, 0) = a * b; break; }
            case 118: { const i32 a = argi(ins, 1), b = argi(ins, 2); *refi(ins, 0) = a / b; break; }
            case 119: { const float a = argf(ins, 1), b = argf(ins, 2); *reff(ins, 0) = a / b; break; }
            case 120: { const i32 a = argi(ins, 1), b = argi(ins, 2); *refi(ins, 0) = a % b; break; }
            case 121: { const float b = argf(ins, 2), a = argf(ins, 1); *reff(ins, 0) = float(std::fmod(double(a), double(b))); break; }
            case 122: { const i32 n = argi(ins, 1); *refi(ins, 0) = n == 0 ? 0 : i32(m.game_rng.next32() % u32(n)); break; }
            case 123: { const float a = argf(ins, 1); *reff(ins, 0) = m.game_rng.unit() * a; break; }
            case 124: { const float a = argf(ins, 1); *reff(ins, 0) = float(std::sin(double(a))); break; }
            case 125: { const float a = argf(ins, 1); *reff(ins, 0) = float(std::cos(double(a))); break; }
            case 126: { const float a = argf(ins, 1); *reff(ins, 0) = float(std::tan(double(a))); break; }
            case 127: { const float a = argf(ins, 1); *reff(ins, 0) = float(std::acos(double(a))); break; }
            case 128: { const float a = argf(ins, 1); *reff(ins, 0) = float(std::atan(double(a))); break; }
            case 129: { float* a = reff(ins, 0); *a = normalize_angle(*a); break; }
            case 130: {
                const float radius = argf(ins, 3), angle = argf(ins, 2);
                float* x = reff(ins, 1);
                float* y = reff(ins, 0);
                // 0x464930：x87 fcos/fsin 后乘半径
                *y = float(std::cos(double(angle)) * double(radius));
                *x = float(std::sin(double(angle)) * double(radius));
                break;
            }
            case 131: {
                const float lo = argf(ins, 2), hi = argf(ins, 3);
                const float angle = m.game_rng.signed_unit() * kPi;
                const float r = m.game_rng.signed_unit();
                const float radius = r * (hi - lo) + lo;
                const float x = float(std::cos(double(angle)) * double(radius));
                const float y = float(std::sin(double(angle)) * double(radius));
                *reff(ins, 0) = x;
                *reff(ins, 1) = y;
                break;
            }
            case 200: jump(ins, ins.i(0), ins.i(1)); continue;
            case 201: {
                i32* counter = refi(ins, 0);
                *counter = *counter - 1;
                const i32 v = ins.var(0) ? int_var(ins.i(0)) : ins.i(0);
                if (v < 1) break;
                jump(ins, ins.i(1), ins.i(2));
                continue;
            }
            case 202: case 204: case 206: case 208: case 210: case 212: {
                const i32 a = argi(ins, 0), b = argi(ins, 1);
                bool take = false;
                switch (op) {
                case 202: take = a == b; break; case 204: take = a != b; break;
                case 206: take = a < b; break; case 208: take = a <= b; break;
                case 210: take = a > b; break; case 212: take = a >= b; break;
                }
                if (!take) break;
                jump(ins, ins.i(2), ins.i(3));
                continue;
            }
            case 203: case 205: case 207: case 209: case 211: case 213: {
                const float a = argf(ins, 0), b = argf(ins, 1);
                bool take = false;
                switch (op) {
                case 203: take = a == b; break; case 205: take = a != b; break;
                case 207: take = a < b; break; case 209: take = a <= b; break;
                case 211: take = a > b; break; case 213: take = a >= b; break;
                }
                if (!take) break;
                jump(ins, ins.i(2), ins.i(3));
                continue;
            }
            case 300: case 301: {
                vm.flags_lo |= AnmVm::kVisible;
                i32 index;
                if (op == 300) {
                    index = argi(ins, 0);
                } else {
                    const i32 base = argi(ins, 0), count = argi(ins, 1);
                    index = i32(m.game_rng.next32() % u32(count)) + base;
                }
                // DAT_00491b1c[sprite_mapper]（0x5dc）：敌弹 / 激光 / 道具按宿主的弹种与颜色重映射精灵号
                if (vm.sprite_mapper != 0) {
                    const u32 slot = u32(vm.sprite_mapper);
                    if (slot < AnmManager::kSpriteMapperCount && m.sprite_mappers[slot] != nullptr)
                        index = m.sprite_mappers[slot](vm, index);
                }
                AnmFile* f = m.file(vm.file_slot);
                if (index < 0) { index = 0x102; f = m.file(AnmManager::kAsciiSlot); }  // DAT_004a6d98->0x19240：ascii.anm 的 0x102 号精灵
                if (f != nullptr) set_sprite(*f, index);
                vm.sprite_set_time = vm.time.current;
                break;
            }
            case 302:
                set_bit(vm.flags_lo, AnmVm::kRenderModeMask, AnmVm::kRenderModeShift, u32(ins.i(0)));
                // 渲染模式 10（0x469e20 环形粒子缓冲 + 绘制钩子 6）：全部 .anm 无 302(10)，不移植
                break;
            case 303: set_bit(vm.flags_lo, AnmVm::kBlendMask, AnmVm::kBlendShift, u32(ins.i(0))); break;
            case 304: set_layer(ins.p[8]); break;
            case 305: set_bit(vm.flags_lo, 0x2000u, 13, u32(ins.i(0))); break;
            case 306: set_bit(vm.flags_lo, 0x8000u, 15, u32(ins.i(0))); break;
            case 307: set_bit(vm.flags_hi, 0x400u, 10, ins.p[8]); break;
            case 308: vm.scale.x = vm.scale.x * -1.0f; vm.flags_lo = (vm.flags_lo ^ AnmVm::kFlipX) | AnmVm::kScaleDirty; break;
            case 309: vm.scale.y = vm.scale.y * -1.0f; vm.flags_lo = (vm.flags_lo ^ AnmVm::kFlipY) | AnmVm::kScaleDirty; break;
            case 310: set_bit(vm.flags_lo, 1u, 0, u32(ins.i(0))); break;
            case 311: set_bit(vm.flags_hi, 0x800u, 11, u32(ins.i(0))); break;
            case 312:
                set_bit(vm.flags_hi, 3u, 0, u32(argi(ins, 0)));
                vm.flags_lo = (vm.flags_lo & 0x3fffffffu) | (u32(argi(ins, 1)) << 30);
                break;
            case 313: set_bit(vm.flags_hi, 0x700000u, 20, u32(argi(ins, 0))); break;
            case 314: set_bit(vm.flags_hi, 0x800000u, 23, u32(argi(ins, 0))); break;
            case 315: set_bit(vm.flags_hi, 0x2000000u, 25, ins.p[8]); break;
            case 316: vm.flags_lo |= AnmVm::kFlag2; break;
            case 317: vm.flags_lo &= ~AnmVm::kFlag2; break;
            case 400: {
                const float z = argf(ins, 2), y = argf(ins, 1), x = argf(ins, 0);
                Vec3& target = (vm.flags_lo & AnmVm::kUsePos2) ? vm.pos2 : vm.pos;
                target = {x, y, z};
                break;
            }
            case 401:
                vm.rotation.x = argf(ins, 0);
                vm.rotation.y = argf(ins, 1);
                vm.rotation.z = argf(ins, 2);
                vm.flags_lo |= AnmVm::kRotationDirty;
                break;
            case 402:
                vm.scale.x = argf(ins, 0);
                vm.scale.y = argf(ins, 1);
                vm.flags_lo |= AnmVm::kScaleDirty;
                break;
            case 403: vm.color[3] = u8(argi(ins, 0)); break;
            case 404: vm.color[2] = u8(argi(ins, 0)); vm.color[1] = u8(argi(ins, 1)); vm.color[0] = u8(argi(ins, 2)); break;
            case 405: vm.color2[3] = u8(argi(ins, 0)); break;
            case 406: vm.color2[2] = u8(argi(ins, 0)); vm.color2[1] = u8(argi(ins, 1)); vm.color2[0] = u8(argi(ins, 2)); break;
            case 407: {
                const i32 time = argi(ins, 0);
                const Vec3 initial = (vm.flags_lo & AnmVm::kUsePos2) ? vm.pos2 : vm.pos;
                const float z = argf(ins, 4), y = argf(ins, 3), x = argf(ins, 2);
                begin_vec3_interp(vm.pos_interp, time, ins.i(1), initial, {x, y, z});
                break;
            }
            case 408: {
                const u8 b = u8(argi(ins, 4)), g = u8(argi(ins, 3)), r = u8(argi(ins, 2));
                begin_color_interp(vm.color_interp, argi(ins, 0), ins.p[8 + 4], vm.color, b, g, r);
                break;
            }
            case 409: {
                const u8 goal = u8(argi(ins, 2));
                begin_scalar_interp(vm.alpha_interp, argi(ins, 0), ins.p[8 + 4], vm.color[3], goal);
                break;
            }
            case 410: {
                const float z = argf(ins, 4), y = argf(ins, 3), x = argf(ins, 2);
                begin_vec3_interp(vm.rotation_interp, argi(ins, 0), ins.i(1), vm.rotation, {x, y, z});
                vm.flags_lo |= AnmVm::kRotationDirty;
                break;
            }
            case 411: {
                // 角度插值到 rotation.z：参数 (time, mode, angle)，各量先归一化
                const float goal = normalize_angle(argf(ins, 2));
                const float initial = normalize_angle(vm.rotation.z);
                Interp<float>& it = vm.angle_interp;
                it.end_time = argi(ins, 0);
                it.initial = initial;
                it.goal = goal;
                it.bezier1 = normalize_angle(0.0f);
                it.bezier2 = normalize_angle(0.0f);
                it.mode = ins.i(1);
                it.timer.reset();
                vm.flags_lo |= AnmVm::kRotationDirty;
                break;
            }
            case 412: {
                const float y = argf(ins, 3), x = argf(ins, 2);
                begin_vec2_interp(vm.scale_interp, argi(ins, 0), ins.p[8 + 4], vm.scale, {x, y});
                vm.flags_lo |= AnmVm::kScaleDirty;
                break;
            }
            case 413: {
                const u8 b = u8(argi(ins, 4)), g = u8(argi(ins, 3)), r = u8(argi(ins, 2));
                begin_color_interp(vm.color2_interp, argi(ins, 0), ins.p[8 + 4], vm.color2, b, g, r);
                vm.flags_lo = (vm.flags_lo & 0xfffbffffu) | 0x20000u;
                break;
            }
            case 414: {
                const u8 goal = u8(argi(ins, 2));
                begin_scalar_interp(vm.alpha2_interp, argi(ins, 0), ins.p[8 + 4], vm.color2[3], goal);
                vm.flags_lo = (vm.flags_lo & 0xfffbffffu) | 0x20000u;
                break;
            }
            case 415: {
                const float z = argf(ins, 2), y = argf(ins, 1), x = argf(ins, 0);
                vm.flags_hi |= AnmVm::kHiHasVelocity;
                vm.angular_velocity = {x, y, z};
                break;
            }
            case 416: {
                const float y = argf(ins, 1), x = argf(ins, 0);
                vm.flags_hi |= AnmVm::kHiHasVelocity;
                vm.scale_growth = {x, y};
                break;
            }
            case 417: {
                const i32 time = argi(ins, 1);
                begin_scalar_interp(vm.alpha_interp, time, 0, vm.color[3], ins.p[8]);
                break;
            }
            case 418:
                // 屏幕坐标 → UV 映射（0x468c00 + 0x45f940），依赖投影子系统
                break;
            case 419: set_bit(vm.flags_hi, 0x2000u, 13, u32(argi(ins, 0))); break;
            case 420: {
                Interp<Vec3>& it = vm.pos_interp;
                const float b1x = argf(ins, 1), b1y = argf(ins, 2), b1z = argf(ins, 3);
                const float b2x = argf(ins, 7), b2y = argf(ins, 8), b2z = argf(ins, 9);
                it.end_time = argi(ins, 0);
                it.bezier1 = {b1x, b1y, b1z};
                it.bezier2 = {b2x, b2y, b2z};
                it.mode = 8;
                it.initial = (vm.flags_lo & AnmVm::kUsePos2) ? vm.pos2 : vm.pos;
                const float gz = argf(ins, 6), gy = argf(ins, 5), gx = argf(ins, 4);
                it.goal = {gx, gy, gz};
                it.timer.reset();
                break;
            }
            case 421:
                set_bit(vm.flags_lo, AnmVm::kAnchorXMask, 21, ins.h(0));
                set_bit(vm.flags_lo, AnmVm::kAnchorYMask, 23, ins.h(1));
                break;
            case 422:
                vm.pos = vm.entity_offset;
                vm.entity_offset = {};
                break;
            case 423: set_bit(vm.flags_lo, AnmVm::kColorModeMask, 17, ins.p[8]); break;
            case 424: set_bit(vm.flags_hi, 0x80u, 7, ins.p[8]); break;
            case 425: vm.flags_hi |= AnmVm::kHiHasVelocity; vm.uv_scroll_velocity.x = argf(ins, 0); break;
            case 426: vm.flags_hi |= AnmVm::kHiHasVelocity; vm.uv_scroll_velocity.y = argf(ins, 0); break;
            case 427: case 428: {
                Interp<float>& it = op == 427 ? vm.u_scroll_interp : vm.v_scroll_interp;
                const float goal = argf(ins, 2);
                it.end_time = argi(ins, 0);
                it.bezier1 = 0; it.bezier2 = 0;
                it.mode = ins.i(1);
                it.initial = op == 427 ? vm.uv_scroll_velocity.x : vm.uv_scroll_velocity.y;
                it.goal = goal;
                it.timer.reset();
                break;
            }
            case 429: vm.uv_scale.x = argf(ins, 0); vm.uv_scale.y = argf(ins, 1); vm.flags_lo |= AnmVm::kUvScaleDirty; break;
            case 430: {
                const float y = argf(ins, 3), x = argf(ins, 2);
                begin_vec2_interp(vm.uv_scale_interp, argi(ins, 0), ins.p[8 + 4], vm.uv_scale, {x, y});
                vm.flags_lo |= AnmVm::kUvScaleDirty;
                break;
            }
            case 431: set_bit(vm.flags_hi, 0x100u, 8, ins.p[8]); break;
            case 432: set_bit(vm.flags_hi, 0x200u, 9, u32(argi(ins, 0))); break;
            case 433: {
                Interp<Vec3>& it = vm.pos_interp;
                it.end_time = argi(ins, 0);
                it.bezier1 = {}; it.bezier2 = {};
                it.mode = ins.i(1);
                it.initial = (vm.flags_lo & AnmVm::kUsePos2) ? vm.pos2 : vm.pos;
                const float radius = argf(ins, 3), angle = argf(ins, 2);
                it.goal = {float(std::cos(double(angle)) * double(radius)), float(std::sin(double(angle)) * double(radius)), 0.0f};
                it.timer.reset();
                break;
            }
            case 434: vm.scale2.x = argf(ins, 0); vm.scale2.y = argf(ins, 1); vm.flags_lo |= AnmVm::kScaleDirty; break;
            case 435: {
                const float y = argf(ins, 3), x = argf(ins, 2);
                begin_vec2_interp(vm.scale2_interp, argi(ins, 0), ins.p[8 + 4], vm.scale, {x, y});
                vm.flags_lo |= AnmVm::kScaleDirty;
                break;
            }
            case 436: vm.anchor_offset.x = argf(ins, 0); vm.anchor_offset.y = argf(ins, 1); break;
            case 437: set_bit(vm.flags_hi, 0x1cu, 2, u32(argi(ins, 0))); break;
            case 438: set_bit(vm.flags_hi, 0xc0000u, 18, ins.p[8]); break;
            case 500: create_child(ins, 0); break;
            case 501: create_child(ins, 4); break;
            case 502: create_child(ins, 2); break;
            case 503: create_child(ins, 6); break;
            case 504: { u32 id = 0; m.create_detached(file, argi(ins, 0), vm, &id); break; }
            case 505: case 506: {
                u32 id = 0;
                AnmVm* child = op == 505 ? m.create_child(file, argi(ins, 0), vm, 0, &id) : m.create_detached(file, argi(ins, 0), vm, &id);
                if (child != nullptr) { child->pos2.x = argf(ins, 1); child->pos2.y = argf(ins, 2); }
                break;
            }
            case 507: set_bit(vm.flags_hi, 0x10000u, 16, u32(argi(ins, 0))); break;
            case 508:
                // 0x418af0：把效果种类（充能环等）的 update/draw/delete/interrupt 回调挂到本 VM
                if (m.hooks != nullptr) m.hooks->attach_effect(argi(ins, 0), vm);
                break;
            case 509:
                if (vm.parent != nullptr) {
                    std::memcpy(vm.int_vars, vm.parent->int_vars, 0x40);
                }
                break;
            case 600: case 601: case 602: {
                static const u32 modes[] = {0x12000000u, 0x1a000000u, 0x1c000000u};
                static const u32 keep[] = {0xd3ffffffu, 0xdbffffffu, 0xddffffffu};
                vm.flags_lo = (vm.flags_lo & keep[op - 600]) | modes[op - 600];
                vm.vertex_buffer_size = u32(argi(ins, 0)) * 0x38;
                break;
            }
            case 603: vm.flags_lo = (vm.flags_lo & 0xe1ffffffu) | 0x20000000u; vm.sprite_size = {argf(ins, 0), argf(ins, 1)}; break;
            case 604: case 605:
                vm.flags_lo = op == 604 ? (vm.flags_lo & 0xe3ffffffu) | 0x22000000u : (vm.flags_lo & 0xe5ffffffu) | 0x24000000u;
                vm.sprite_size.x = argf(ins, 0);
                vm.int_vars[0] = argi(ins, 1);
                break;
            case 606: vm.flags_lo = (vm.flags_lo & 0xe9ffffffu) | 0x28000000u; vm.sprite_size = {argf(ins, 0), argf(ins, 1)}; break;
            case 607: vm.flags_lo = (vm.flags_lo & 0xebffffffu) | 0x2a000000u; vm.sprite_size = {argf(ins, 0), argf(ins, 1)}; break;
            case 608: vm.flags_lo = (vm.flags_lo & 0xedffffffu) | 0x2c000000u; vm.sprite_size = {argf(ins, 0), argf(ins, 1)}; break;
            case 609: vm.flags_lo = (vm.flags_lo & 0xf1ffffffu) | 0x30000000u; vm.vertex_buffer_size = u32(argi(ins, 0)) * 0x30; break;
            case 610: vm.flags_lo = (vm.flags_lo & 0xf3ffffffu) | 0x32000000u; vm.vertex_buffer_size = u32(argi(ins, 0)) * 0x30; break;
            case 611:
                vm.flags_lo = (vm.flags_lo & 0xe7ffffffu) | 0x26000000u;
                vm.sprite_size = {argf(ins, 0), argf(ins, 1)};
                vm.int_vars[0] = argi(ins, 2);
                break;
            case 612: vm.flags_lo = (vm.flags_lo & 0xf7ffffffu) | 0x36000000u; vm.sprite_size = {argf(ins, 0), argf(ins, 1)}; break;
            case 613: vm.flags_lo = (vm.flags_lo & 0xf5ffffffu) | 0x34000000u; vm.sprite_size = {argf(ins, 0), argf(ins, 1)}; break;
            default:
                break;  // 0、5 及未知：跳过
            }
            next(ins);
        }
    }

    // LAB_00462a06
    void end_of_frame() {
        if (vm.flags_hi & AnmVm::kHiHasVelocity) apply_velocities();
        if (vm.flags_lo & AnmVm::kEntityOffset) {
            vm.entity_offset.x = vm.entity_offset.x + m.entity_offset_delta.x;
            vm.entity_offset.y = vm.entity_offset.y + m.entity_offset_delta.y;
            vm.entity_offset.z = m.entity_offset_delta.z + vm.entity_offset.z;
        }
        // flags_hi & 0x2000：每帧屏幕 UV 映射（0x468c00）——依赖投影子系统
        step_interps();
        // 0x4632f0：600 系列条带几何（宿主顶点缓冲）
        {
            static const AnmGeometry kDefaultGeometry;
            generate_strip_geometry(vm, m.geometry != nullptr ? *m.geometry : kDefaultGeometry);
        }
        if (vm.time_callback != 0) {
            // DAT_004c0f44 表：非零槽位可阻止计时
        }
        vm.time.tick(m.game_speed);
    }

    // 0x4630f0
    void apply_velocities() {
        const float s = m.game_speed;
        if (vm.angular_velocity.x != 0.0f) { vm.rotation.x = normalize_angle(vm.angular_velocity.x * s + vm.rotation.x); vm.flags_lo |= AnmVm::kRotationDirty; }
        if (vm.angular_velocity.y != 0.0f) { vm.rotation.y = normalize_angle(vm.angular_velocity.y * s + vm.rotation.y); vm.flags_lo |= AnmVm::kRotationDirty; }
        if (vm.angular_velocity.z != 0.0f) { vm.rotation.z = normalize_angle(vm.angular_velocity.z * s + vm.rotation.z); vm.flags_lo |= AnmVm::kRotationDirty; }
        if (vm.scale_growth.y != 0.0f) { vm.flags_lo |= AnmVm::kScaleDirty; vm.scale.y = vm.scale_growth.y * s + vm.scale.y; }
        if (vm.scale_growth.x != 0.0f) { vm.flags_lo |= AnmVm::kScaleDirty; vm.scale.x = vm.scale_growth.x * s + vm.scale.x; }
        if (vm.uv_scroll_velocity.x != 0.0f) {
            float u = vm.uv_scroll_velocity.x * s + vm.uv_scroll.x;
            vm.uv_scroll.x = u;
            if (u < 2.0f) { if (u < 0.0f) vm.uv_scroll.x = u + 2.0f; }
            else vm.uv_scroll.x = u - 2.0f;
        }
        if (vm.uv_scroll_velocity.y != 0.0f) {
            float v = vm.uv_scroll_velocity.y * s + vm.uv_scroll.y;
            vm.uv_scroll.y = v;
            if (2.0f <= v) vm.uv_scroll.y = v - 2.0f;
            else if (v < 0.0f) vm.uv_scroll.y = v + 2.0f;
        }
    }

    // 0x463b30
    void step_interps() {
        const float s = m.game_speed;
        if (vm.pos_interp.end_time != 0) {
            const Vec3 p = step_interp(vm.pos_interp, s);
            if (vm.flags_lo & AnmVm::kUsePos2) vm.pos2 = p; else vm.pos = p;
        }
        if (vm.color_interp.end_time != 0) {
            const IVec3 c = step_interp(vm.color_interp, s);
            vm.color[2] = u8(c.z); vm.color[1] = u8(c.y); vm.color[0] = u8(c.x);
        }
        if (vm.alpha_interp.end_time != 0) vm.color[3] = u8(step_interp_int(vm.alpha_interp, s));
        if (vm.scale_interp.end_time != 0) { vm.scale = step_interp(vm.scale_interp, s); vm.flags_lo |= AnmVm::kScaleDirty; }
        if (vm.scale2_interp.end_time != 0) { vm.scale2 = step_interp(vm.scale2_interp, s); vm.flags_lo |= AnmVm::kScaleDirty; }
        if (vm.uv_scale_interp.end_time != 0) { vm.uv_scale = step_interp(vm.uv_scale_interp, s); vm.flags_lo |= AnmVm::kUvScaleDirty; }
        if (vm.rotation_interp.end_time != 0) { vm.rotation = step_interp(vm.rotation_interp, s); vm.flags_lo |= AnmVm::kRotationDirty; }
        if (vm.angle_interp.end_time != 0) { vm.rotation.z = step_interp_angle(vm.angle_interp, s); vm.flags_lo |= AnmVm::kRotationDirty; }
        if (vm.color2_interp.end_time != 0) {
            const IVec3 c = step_interp(vm.color2_interp, s);
            vm.color2[2] = u8(c.z); vm.color2[1] = u8(c.y); vm.color2[0] = u8(c.x);
        }
        if (vm.alpha2_interp.end_time != 0) vm.color2[3] = u8(step_interp_int(vm.alpha2_interp, s));
        if (vm.u_scroll_interp.end_time != 0) vm.uv_scroll_velocity.x = step_interp(vm.u_scroll_interp, s);
        if (vm.v_scroll_interp.end_time != 0) vm.uv_scroll_velocity.y = step_interp(vm.v_scroll_interp, s);
    }
};

int AnmManager::run(AnmVm& vm) {
    AnmFile* f = file(vm.file_slot);
    if (f == nullptr || vm.script_index < 0 || u32(vm.script_index) >= f->scripts.size()) return 0;
    AnmInterpreter it{*this, vm, *f};
    const AnmVm* outer = running_vm;
    running_vm = &vm;
    const int rc = it.run();
    running_vm = outer;
    return rc;
}

}  // namespace th16
