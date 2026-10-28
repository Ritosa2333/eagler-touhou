// 妖梦 Bomb 斩击池实现。数值口径（x87 80 位中间量按原式用 double 复刻后窄化）：
//   y       = f32(lane + f32(signed_unit × 48))          （0x40cc40 fstp f32）
//   angle'  = f32(angle + double(π/2))                    （0x40cdcb fadd QWORD 0x4ae9c0）
//   offset  = f32(cos/sin(angle') × 256)                  （0x40cf70，80bit 三角）
//   center  = (0 ∓ offset.x, y ∓ offset.y)（偶减/奇加）   （0x40ce0b / 0x40cdf1）
//   h       = f32(rect_scale × double(128.0))             （0x40ce3b fmul QWORD 0x4ae8c8）
#include "YoumuBomb.hpp"

#include <array>
#include <cmath>

namespace th13 {

void YoumuBomb::reset() {
    for (auto& s : slots_) s = Slot{};
    cursor_ = 0;
}

std::optional<YoumuBomb::Spawn> YoumuBomb::step(int age, GameRandom& rng) {
    if (age >= kActiveFrames) return std::nullopt;
    const int c = cursor_++;
    const float jitter =
        static_cast<float>(static_cast<double>(rng.signed_unit()) * 48.0);
    const float y = static_cast<float>(static_cast<double>(kLanes[c % 11]) +
                                       static_cast<double>(jitter));
    const int script = (c & 1) ? 20 : 19;
    if (c < kSlots) {
        Slot& s = slots_[c];
        s.has_anm = true;
        s.odd = (c & 1) != 0;
        s.script = script;
        s.y = y;
        s.scale = 0.0f;     // 0x4016c0(0)：value=-1, max=0, rate=0.0f
        s.counter = 0;
        s.alive = false;    // VM 状态待本帧 set_vm 回填
        s.rect_scale = 0.0f;
        return Spawn{c, script, y};
    }
    return Spawn{-1, script, y};
}

void YoumuBomb::set_vm(int slot, bool alive, float rotation_z) {
    if (slot < 0 || slot >= kSlots) return;
    Slot& s = slots_[slot];
    if (!s.has_anm) return;  // 原版：anm id==0 的槽不进循环体（无 tick、无矩形）
    if (!alive) {
        s.has_anm = false;  // 0x46fb90 返回 0 → anm id 清零（scale 冻结）
        s.alive = false;
        return;
    }
    s.alive = true;
    s.angle = rotation_z;
    s.rect_scale = s.scale;  // 本帧矩形用 tick 前值
    s.scale = static_cast<float>(static_cast<double>(s.scale) + 1.0);
    s.counter += 1;          // 槽定时器 max += *cb(=1.0)
}

bool YoumuBomb::active(int slot) const { return slots_[slot].alive; }
bool YoumuBomb::has_anm(int slot) const { return slots_[slot].has_anm; }
float YoumuBomb::scale(int slot) const { return slots_[slot].scale; }
float YoumuBomb::rect_scale(int slot) const { return slots_[slot].rect_scale; }
float YoumuBomb::lane_y(int slot) const { return slots_[slot].y; }
float YoumuBomb::angle(int slot) const { return slots_[slot].angle; }
int YoumuBomb::script(int slot) const { return slots_[slot].script; }
int YoumuBomb::cursor() const { return cursor_; }

YoumuBomb::Rect YoumuBomb::rect(int slot) const {
    const Slot& s = slots_[slot];
    const float ap =
        static_cast<float>(static_cast<double>(s.angle) + 1.5707963705062866);
    const float dx = static_cast<float>(std::cos(static_cast<double>(ap)) * 256.0);
    const float dy = static_cast<float>(std::sin(static_cast<double>(ap)) * 256.0);
    const float cx = s.odd ? dx : static_cast<float>(0.0 - static_cast<double>(dx));
    const float cy = s.odd
                         ? static_cast<float>(static_cast<double>(s.y) + static_cast<double>(dy))
                         : static_cast<float>(static_cast<double>(s.y) - static_cast<double>(dy));
    const float h = static_cast<float>(static_cast<double>(s.rect_scale) * 128.0);
    return Rect{cx, cy, kRectLength, h, s.angle};
}

bool YoumuBomb::rect_covers(const Rect& rc, float px, float py, float r) {
    const double ca = std::cos(static_cast<double>(rc.angle));
    const double sa = std::sin(static_cast<double>(rc.angle));
    const double ddx = static_cast<double>(px) - static_cast<double>(rc.cx);
    const double ddy = static_cast<double>(py) - static_cast<double>(rc.cy);
    const double lx = ddx * ca - ddy * sa;  // fVar2
    const double ly = ddx * sa + ddy * ca;  // fVar1
    const double hw = static_cast<double>(rc.w) * 0.5;
    const double hh = static_cast<double>(rc.h) * 0.5;
    const double rr = static_cast<double>(r);
    if (!((rr + hw < std::fabs(lx)) || (hh < std::fabs(ly)))) return true;
    if ((std::fabs(lx) < hw) && (std::fabs(ly) <= hh + rr)) return true;
    const double ax = lx - hw, ay = ly - hh;
    if (ay * ay + ax * ax < rr * rr) return true;
    const double bx = lx + hw;
    if (ay * ay + bx * bx < rr * rr) return true;
    const double by = ly + hh;
    if (by * by + ax * ax < rr * rr) return true;
    return by * by + bx * bx < rr * rr;
}

int YoumuBomb::cover_count(float px, float py, float r) const {
    int n = 0;
    for (int i = 0; i < kSlots; ++i)
        if (slots_[i].alive && rect_covers(rect(i), px, py, r)) ++n;
    return n;
}

bool YoumuBomb::clears_bullet(float px, float py, float r) const {
    static constexpr Rect field{0.0f, 224.0f, 384.0f, 448.0f, 0.0f};
    if (!rect_covers(field, px, py, r)) return false;
    for (int i = 0; i < kSlots; ++i)
        if (slots_[i].alive && rect_covers(rect(i), px, py, r)) return true;
    return false;
}

}  // namespace th13
