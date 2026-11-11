#include "TitleOptionsMenu.hpp"

#include <algorithm>

namespace th13 {

namespace {
// 行条/数字脚本号（title.anm；root1 spawn 列表 dump 实读）。
constexpr i32 kOptionRoot = 1;
constexpr i32 kOptionRows = 5;
// 数字脚本：BGM 小 33-35 / 大 37-39；SE 小 41-43 / 大 45-47（0x21.. 族）。
constexpr i32 kDigitBgmSmall = 33, kDigitBgmLarge = 37;
constexpr i32 kDigitSeSmall = 41, kDigitSeLarge = 45;
constexpr i32 kSpriteSmall = 50, kSpriteLarge = 61;  // +digit（FUN_00461a60 实参）
}  // namespace

bool TitleOptionsMenu::fail(const std::string& reason) {
    if (error.empty()) error = reason;
    return false;
}

bool TitleOptionsMenu::sound(i32 id) {
    if (play_sound && play_sound(id)) return true;
    return fail("Title option sound service unavailable");
}

// case0（0x44dd00）：光标归 0 + 音量刷新 + 光标栈 push（主菜单派发侧
// FUN_00473190 对合）；root1 由本屏自建（原版常驻，移植重入惯例：旧根
// pending label1 清槽后重建）。
bool TitleOptionsMenu::open() {
    row_ = 0;
    if (!pushed_) {
        state.menu.push();
        pushed_ = true;
    }
    // 从キー配置屏返回时 Option 光标栈条目未 pop，但本屏 case0 仍按原版
    // FUN_00415030(0) 归位；count 重新落 5（键配置屏曾改为 7）。
    state.menu.wrapping = true;
    state.menu.count = kOptionRows;
    state.menu.select(0);
    if (state.handles[kOptionRoot]) {
        if (!visuals.interrupt(kOptionRoot, 1, false)) return fail(visuals.error);
        state.handles[kOptionRoot] = 0;
    }
    if (!visuals.create(kOptionRoot)) return fail(visuals.error);
    if (!refresh_volumes()) return false;
    state.change_substate(1);
    substate_frames_ = 0;
    return true;
}

// 0044ed70 对合：数字 sprite 写值（小=值+50、大=值+61）+ 前导零可见位
//（百位 v≥100、十位 v≥10；个位恒显）+ host.apply（DAT_004e476c/70 写）。
bool TitleOptionsMenu::refresh_volumes() {
    host.apply_volumes(volumes.bgm, volumes.effect);
    auto* root = animations.registry.find(state.handles[kOptionRoot]);
    if (!root) return true;  // VM 未建（宿主开机预应用路径）：仅应用音量
    const i32 values[2] = {std::clamp(volumes.bgm, 0, 100),
                           std::clamp(volumes.effect, 0, 100)};
    const i32 small_bases[2] = {kDigitBgmSmall, kDigitSeSmall};
    const i32 large_bases[2] = {kDigitBgmLarge, kDigitSeLarge};
    for (i32 which = 0; which < 2; ++which) {
        const i32 value = values[which];
        const i32 digits[3] = {value / 100, (value / 10) % 10, value % 10};
        const bool visible[3] = {value >= 100, value >= 10, true};
        for (i32 place = 0; place < 3; ++place) {
            for (const i32 base : {small_bases[which], large_bases[which]}) {
                auto* vm = animations.registry.find_child_script(*root, base + place, 0);
                if (!vm) continue;
                if (!vm->select_sprite((base == kDigitBgmSmall || base == kDigitSeSmall
                                            ? kSpriteSmall
                                            : kSpriteLarge) +
                                       digits[place]))
                    return fail("Title option digit sprite select failed");
                // 原版 VM+0x18 bit1 ↔ port visual.flags bit0（tick 每帧按 flags
                // 重导出 visible，双写与 TitleSelectionMenu poke 惯例一致）。
                if (visible[place]) {
                    vm->visual.flags |= 1u;
                    vm->visible = true;
                } else {
                    vm->visual.flags &= ~1u;
                    vm->visible = false;
                }
            }
        }
    }
    return true;
}

// case1/case2 选中广播：immediate label3（熄灭全部行条）+ pending label
// 基+行（入场 17+行 / 移动 7+行；行条脚本 label 空间 7..11/17..21 实读）。
void TitleOptionsMenu::select_row(i32 label_base) {
    (void)visuals.interrupt(kOptionRoot, 3, true);
    (void)visuals.interrupt(kOptionRoot, label_base + row_, false);
}

// case4 出口（>9 帧）：行2 → 键配置屏（宿主接线，未接 = 真实失败记录）；
// 行4 → 持久化（原版离屏落盘）+ root1 label1 收面 + 回主菜单 + 光标栈 pop
//（0x44dd00 case4 + 004731d0）。
bool TitleOptionsMenu::leave(bool to_key_config) {
    if (to_key_config) {
        if (!host.open_key_config())
            return fail("Title option key config screen not wired");
        // 原版 case4 行2：0044bac0(...,4) + 00473190 push，root1 不显式收面；
        // 移植侧两屏字条同坐标（root1/root2 均 (70,312)），必须收 Option 面
        // 再切屏，避免与键配置 7 行重叠（视觉等价，生命周期差异登记）。
        (void)visuals.interrupt(kOptionRoot, 1, false);
        state.handles[kOptionRoot] = 0;
        // 光标栈 push 归键配置屏 case0（与 00473190 净效果相同）。
        state.change_screen(TitleScreen::KeyConfig);
        return true;
    }
    if (!host.persist_options(volumes)) fail("Title option persistence unavailable");
    (void)visuals.interrupt(kOptionRoot, 1, false);
    state.handles[kOptionRoot] = 0;
    if (pushed_) {
        if (state.menu.depth > 0) state.menu.pop();
        pushed_ = false;
    }
    state.change_screen(TitleScreen::Main);
    return true;
}

bool TitleOptionsMenu::update(u32 pressed, u32 repeated) {
    const u32 input = pressed | repeated;
    ++substate_frames_;
    // root1 子 VM 首帧尚未 spawn 完成（原版 VM 常驻、0044ed70 仅变化时调；
    // 移植自建树，故逐帧幂等刷新补偿创建时序，写值本身与原版逐字节同式）。
    if (state.substate != 0 && state.substate != 4) {
        if (!refresh_volumes()) return false;
    }
    switch (state.substate) {
    case 0:
        if (!open()) return false;
        break;
    case 1:  // >6 帧进可交互（原版 6 < +0x2b8）
        if (substate_frames_ > 6) {
            state.change_substate(2);
            substate_frames_ = 0;
            select_row(17);
        }
        break;
    case 2: {
        const i32 previous = row_;
        if (input & 0x10) row_ = (row_ + kOptionRows - 1) % kOptionRows;  // ↑
        else if (input & 0x20) row_ = (row_ + 1) % kOptionRows;           // ↓
        if (row_ != previous) {
            if (!sound(10)) return false;
            select_row(7);
        }
        // X/Esc：非行4 → 光标跳行4（FUN_00415030(4) + 音9 + redraw）；行4 →
        // 音9 + substate4 同帧返回（LAB_0044df5f → 0044bb20(4) + return）。
        if (input & 0x102) {
            if (row_ != 4) {
                if (!sound(9)) return false;
                row_ = 4;
                select_row(7);
            } else {
                if (!sound(9)) return false;
                key_config_ = false;
                state.change_substate(4);
                substate_frames_ = 0;
                return true;
            }
        }
        // ←→ 音量 ±5（行0=BGM 行1=SE；钳 0..100；原版无音效）
        if (row_ == 0 || row_ == 1) {
            i32& value = row_ == 0 ? volumes.bgm : volumes.effect;
            const i32 before = value;
            if (input & 0x40) value = value < 5 ? 0 : value - 5;
            if (input & 0x80) value = value > 95 ? 100 : value + 5;
            if (value != before) {
                if (!refresh_volumes()) return false;
            }
        }
        // Z：行2 键配置 / 行3 复位默认 / 行4 退出
        if (input & 0x80001) {
            if (row_ == 2) {
                if (!sound(7)) return false;
                key_config_ = true;
                state.change_substate(4);
                substate_frames_ = 0;
            } else if (row_ == 3) {
                volumes.bgm = 100;
                volumes.effect = 80;  // DAT_004dc894=0 同拍（v1.00c 无读者，不另存）
                if (!sound(7)) return false;
                if (!refresh_volumes()) return false;
            } else if (row_ == 4) {
                if (!sound(9)) return false;
                key_config_ = false;
                state.change_substate(4);
                substate_frames_ = 0;
            }
        }
        break;
    }
    case 3:  // 原版空 case（保留枚举位）
        break;
    case 4:  // >9 帧离屏（原版 9 < +0x2b8）
        if (substate_frames_ > 9) {
            if (!leave(key_config_)) return false;
        }
        break;
    default:
        break;
    }
    return true;
}

}  // namespace th13
