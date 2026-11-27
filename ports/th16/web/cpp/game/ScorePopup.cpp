// th16 得分飘字（原版 DAT_004a6f10；各函数地址见 ScorePopup.hpp 的锚点表）。
#include "ScorePopup.hpp"

#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "Player.hpp"

namespace th16 {

namespace {

// ascii.anm 里三套数字字形的起点：普通 / 放大 1 / 放大 2（0..9 数字，10 为特殊字形）
constexpr i32 kDigitSpriteNormal = 0x103;
constexpr i32 kDigitSpriteLarge1 = 0x10e;
constexpr i32 kDigitSpriteLarge2 = 0x118;
constexpr float kGlyphWidth = 8.0f;

void set_vm_color(AnmVm& vm, u32 argb) {
    vm.color[0] = u8(argb);
    vm.color[1] = u8(argb >> 8);
    vm.color[2] = u8(argb >> 16);
    vm.color[3] = u8(argb >> 24);
}

}  // namespace

// 0x449cd0
void ScorePopupManager::create(AnmFile& ascii) {
    ascii_ = &ascii;
    AnmManager::reset_vm(glyph_vm_);
    glyph_vm_.file_slot = ascii.slot;
    anm_set_sprite(ascii, glyph_vm_, kDigitSpriteNormal);
    // 层组 0x80000：屏幕坐标按场地原点 +(320,16) 偏移；缩放模式 3
    glyph_vm_.flags_hi = (glyph_vm_.flags_hi & 0xffbbffffu) | 0x380000u;
    ring_cursor_ = 0;
    for (Popup& p : digit_popups_) p = Popup{};
}

// 0x44a460
void ScorePopupManager::push(const Vec3& pos, i32 value, u32 color) {
    if (ring_cursor_ > kRingSize - 1) ring_cursor_ = 0;
    Popup& popup = digit_popups_[ring_cursor_];
    popup.active = true;
    if (value < 0) {
        popup.digits[0] = 10;
        popup.digit_count = 1;
    } else {
        int count = 0;
        for (i32 rest = value; rest != 0; rest /= 10) popup.digits[count++] = u8(rest % 10);
        if (count == 0) {
            popup.digits[0] = 0;
            count = 1;
        }
        popup.digit_count = u8(count);
    }
    popup.color = color;
    popup.timer.reset();
    popup.pos = pos;
    popup.rise_speed = 1.0f;
    ++ring_cursor_;
}

// 0x449ea0
void ScorePopupManager::update(float game_speed) {
    for (Popup& popup : digit_popups_) {
        if (!popup.active) continue;
        const float rise = popup.rise_speed * game_speed;
        popup.rise_speed = popup.rise_speed * 0.95f;
        popup.pos.y = popup.pos.y - rise;
        popup.timer.tick(game_speed);
        if (popup.timer.current > kLifetime) popup.active = false;
    }
}

// 0x44a000 中段：自机 64 像素内变淡——距离平方（取整）≤ 0x1000 时 0x80，到 0x4000 线性回到 0xff
u8 ScorePopupManager::alpha_near_player(const Popup& popup) const {
    if (player == nullptr) return 0xff;
    const float dy = player->pos.y - popup.pos.y;
    const float dx = player->pos.x - popup.pos.x;
    const i32 distance_sq = i32(dy * dy + dx * dx);
    if (distance_sq > 0x4000) return 0xff;
    if (distance_sq <= 0x1000) return 0x80;
    return u8(((distance_sq - 0x1000) * 0x80) / 0x3000 + 0x80);
}

// 0x44a000 单条数字飘字
void ScorePopupManager::draw_digit_popup(AnmRenderer& renderer, const Popup& popup) {
    if (ascii_ == nullptr) return;
    // 头 8 帧字宽 = 8 / 计时（浮点）；入队当帧计时为 0，字宽即 ∞，原版也画出这组无效坐标
    float glyph_width = kGlyphWidth;
    if (popup.timer.current < 8) glyph_width = kGlyphWidth / popup.timer.current_f;
    AnmVm& vm = glyph_vm_;
    vm.entity_offset.x = popup.pos.x - float(popup.digit_count) * glyph_width * 0.5f;
    vm.entity_offset.y = popup.pos.y;
    set_vm_color(vm, popup.color);
    const u8 alpha = alpha_near_player(popup);

    // 从最高位画到最低位。第 k 位（k 从 0 起）在计时 < 0x34 - 2n + 2k 时用普通字形，之后 4 帧放大 1、
    // 再 4 帧放大 2，计时 ≥ 0x3c - 2n + 2k 后不再画——高位先消失。特殊字形（10）始终普通
    const i32 count = popup.digit_count;
    i32 vanish_at = 0x3c - 2 * count;
    i32 normal_until = 0x34 - 2 * count;
    for (i32 k = count - 1; k >= 0; --k) {
        const u8 digit = popup.digits[k];
        const i32 t = popup.timer.current;
        i32 sprite = -1;
        if (t < normal_until || digit == 10) sprite = kDigitSpriteNormal + digit;
        else if (t < vanish_at - 4) sprite = kDigitSpriteLarge1 + digit;
        else if (t < vanish_at) sprite = kDigitSpriteLarge2 + digit;
        if (sprite >= 0) {
            anm_set_sprite(*ascii_, vm, sprite);
            vm.color[3] = alpha;
            vm.flags_lo |= AnmVm::kScaleDirty;
            renderer.draw_screen_sprite(vm);
        }
        vanish_at += 2;
        normal_until += 2;
        vm.entity_offset.x = vm.entity_offset.x + glyph_width;
    }
}

// 0x44a000
void ScorePopupManager::draw(AnmRenderer& renderer) {
    renderer.set_fog(false);   // 0x43c4f0 内联
    for (const Popup& popup : digit_popups_) {
        if (popup.active) draw_digit_popup(renderer, popup);
    }
}

}  // namespace th16
