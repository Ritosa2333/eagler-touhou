#include "TitleKeyConfigMenu.hpp"

#include <algorithm>

namespace th13 {

namespace {
constexpr i32 kKeyConfigRoot = 2;       // title.anm script2：spawn 49-82
constexpr i32 kGreyRowFirst = 49;       // 7 行灰字条（层26）
constexpr i32 kWhiteRowFirst = 56;      // 7 行白字条（层25）
constexpr i32 kGreyDigitFirst = 63;     // 5 行×2 位灰数字
constexpr i32 kWhiteDigitFirst = 73;    // 同位白数字（00450020 同写灰 sprite）
constexpr i32 kDigitSpriteBase = 50;    // 00461a60(+0x32)：0..9
constexpr u32 kRawPadButtonMask = 0x7fffffffu;  // 0044fcc0 扫描 iVar3 < 0x1f
}  // namespace

bool TitleKeyConfigMenu::fail(const std::string& reason) {
    if (error.empty()) error = reason;
    return false;
}

bool TitleKeyConfigMenu::sound(i32 id) {
    if (play_sound && play_sound(id)) return true;
    return fail("Title key config sound service unavailable");
}

// case0（0044fcc0）：行数 7、光标 0、建根、抄当前绑定、刷新数字。
bool TitleKeyConfigMenu::open() {
    row_ = 0;
    substate_frames_ = 0;
    state.menu.push();
    state.menu.wrapping = true;
    state.menu.count = kTitleKeyConfigRows;
    state.menu.select(0);
    bindings_ = host.pad_bindings();
    for (i16& value : bindings_) value = i16(std::clamp<i32>(value, 0, 31));
    if (state.handles[kKeyConfigRoot]) {
        if (!visuals.retire(kKeyConfigRoot)) return fail(visuals.error);
        state.handles[kKeyConfigRoot] = 0;
    }
    if (!visuals.create(kKeyConfigRoot)) return fail(visuals.error);
    if (!refresh_digits()) return false;
    state.change_substate(1);
    substate_frames_ = 0;
    return true;
}

// 00450020：五键十进制两位数。灰组 63..72 与白组 73..82 都写 sprite
// 50+d（反编译逐块 461a60(+0x32)，白组被灰层覆盖仍照写）。
bool TitleKeyConfigMenu::refresh_digits() {
    auto* root = animations.registry.find(state.handles[kKeyConfigRoot]);
    if (!root) return true;  // 首帧 spawn 未完成：下一帧补偿（原版 VM 常驻）
    for (i32 row = 0; row < kTitleKeyConfigBindings; ++row) {
        const i32 value = std::clamp<i32>(bindings_[u32(row)], 0, 31);
        const i32 digits[2] = {value / 10, value % 10};
        for (i32 place = 0; place < 2; ++place) {
            for (const i32 first : {kGreyDigitFirst, kWhiteDigitFirst}) {
                auto* vm = animations.registry.find_child_script(
                    *root, first + row * 2 + place, 0);
                if (!vm) continue;
                if (!vm->select_sprite(kDigitSpriteBase + digits[place]))
                    return fail("Title key config digit sprite select failed");
            }
        }
    }
    return true;
}

void TitleKeyConfigMenu::revert_bindings() {
    bindings_ = host.pad_bindings();
    for (i16& value : bindings_) value = i16(std::clamp<i32>(value, 0, 31));
    (void)refresh_digits();
}

// 004509d0：新值与当前相同 → 无操作（按住幂等，原版无额外 release 状态）；
// 其它行已占用新按钮 → 改回本行旧值（重复冲突交换）；再写本行 + 音7。
bool TitleKeyConfigMenu::learn(i32 button) {
    if (button < 0 || button > 30) return true;
    const i16 previous = bindings_[u32(row_)];
    if (previous == i16(button)) return true;
    for (i32 other = 0; other < kTitleKeyConfigBindings; ++other) {
        if (other != row_ && bindings_[u32(other)] == i16(button))
            bindings_[u32(other)] = previous;
    }
    bindings_[u32(row_)] = i16(button);
    if (!refresh_digits()) return false;
    return sound(7);
}

// case1/case2 选中广播：immediate label3（全行熄灭）+ pending 基+行。
void TitleKeyConfigMenu::select_row(i32 label_base) {
    if (!state.handles[kKeyConfigRoot]) return;
    (void)visuals.interrupt(kKeyConfigRoot, 3, true);
    (void)visuals.interrupt(kKeyConfigRoot, label_base + row_, false);
}

// case4（>9 帧）：收面（label1）→ 回 Option 屏（0044bac0(...,3)）+
// 光标栈 pop（004731d0）。Option case0 会按当前全局值重建。
bool TitleKeyConfigMenu::leave() {
    if (state.handles[kKeyConfigRoot]) {
        (void)visuals.interrupt(kKeyConfigRoot, 1, false);
        state.handles[kKeyConfigRoot] = 0;
    }
    state.menu.pop();
    state.change_screen(TitleScreen::Option);
    return true;
}

bool TitleKeyConfigMenu::update(u32 pressed, u32 repeated) {
    const u32 input = pressed | repeated;
    ++substate_frames_;
    // 原版根 VM 常驻、00450020 只在变化时调；移植自建树，首帧子 VM 尚未
    // spawn，故 substate0/1 逐帧幂等刷新补偿创建时序（写值同式）。
    if (state.substate == 0 || state.substate == 1) {
        if (!refresh_digits()) return false;
    }
    switch (state.substate) {
    case 0:
        if (!open()) return false;
        break;
    case 1:
        if (substate_frames_ > 6) {  // 原版 6 < +0x2b8
            state.change_substate(2);
            substate_frames_ = 0;
            select_row(17);
        }
        break;
    case 2: {
        const i32 previous = row_;
        if (input & 0x10) row_ = (row_ + kTitleKeyConfigRows - 1) % kTitleKeyConfigRows;
        else if (input & 0x20) row_ = (row_ + 1) % kTitleKeyConfigRows;
        if (row_ != previous) {
            if (!sound(10)) return false;
            select_row(7);
        }
        // 手柄学习：原始位图首个按下位；行 0..4 才可写，缺手柄/无按键时
        // pad_present/pad_buttons 门控使本段完全跳过（不伪造绑定）。
        if (row_ < kTitleKeyConfigBindings && host.pad_present()) {
            const u32 buttons = host.pad_buttons() & kRawPadButtonMask;
            for (i32 button = 0; button <= 30; ++button) {
                if (buttons & (1u << u32(button))) {
                    if (!learn(button)) return false;
                    break;
                }
            }
        }
        // X/Esc 仅 Quit 行（6）：放弃编辑（回滚为宿主当前值）+ 音9 + 离屏。
        if ((pressed & 0x102) && row_ == 6) {
            revert_bindings();
            if (!sound(9)) return false;
            state.change_substate(4);
            substate_frames_ = 0;
            return true;
        }
        if (pressed & 0x80001) {
            if (row_ == 5) {  // Default 行：原版把全局当前值抄回 scratch（音7）
                revert_bindings();
                if (!sound(7)) return false;
            } else if (row_ == 6) {  // Quit 行：保存 scratch + 音9 + 离屏
                if (!host.persist_pad_bindings(bindings_))
                    fail("Title key config pad persistence unavailable");
                if (!sound(9)) return false;
                state.change_substate(4);
                substate_frames_ = 0;
            }
        }
        break;
    }
    case 3:  // 原版空 case（保留枚举位）
        break;
    case 4:
        if (substate_frames_ > 9) {  // 原版 9 < +0x2b8
            if (!leave()) return false;
        }
        break;
    default:
        break;
    }
    return true;
}

}  // namespace th13
