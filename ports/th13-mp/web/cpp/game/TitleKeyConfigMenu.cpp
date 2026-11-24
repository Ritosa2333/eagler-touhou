#include "TitleKeyConfigMenu.hpp"

#include <algorithm>

namespace th13 {

namespace {
constexpr i32 kKeyConfigRoot = 2;
constexpr i32 kGreyDigitFirst = 63;
constexpr i32 kWhiteDigitFirst = 73;
constexpr i32 kDigitSpriteBase = 50;
constexpr u32 kRawPadButtonMask = 0x7fffffffu;
}

bool TitleKeyConfigMenu::fail(const std::string& reason) {
    if (error.empty()) error = reason;
    return false;
}

bool TitleKeyConfigMenu::sound(i32 id) {
    if (play_sound && play_sound(id)) return true;
    return fail("Title key config sound service unavailable");
}

bool TitleKeyConfigMenu::open() {
    row_ = 0;
    substate_frames_ = 0;
    state.menu.push();
    state.menu.wrapping = true;
    state.menu.count = kTitleKeyConfigRows;
    state.menu.select(0);
    bindings_ = host.pad_bindings();
    for (i16& value : bindings_)
        value = i16(std::clamp<i32>(value, 0, 31));
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

bool TitleKeyConfigMenu::refresh_digits() {
    auto* root = animations.registry.find(state.handles[kKeyConfigRoot]);
    if (!root) return true;
    for (i32 row = 0; row < kTitleKeyConfigBindings; ++row) {
        const i32 value = std::clamp<i32>(bindings_[u32(row)], 0, 31);
        const i32 digits[2] = {value / 10, value % 10};
        for (i32 place = 0; place < 2; ++place) {
            for (const i32 first : {kGreyDigitFirst, kWhiteDigitFirst}) {
                auto* vm = animations.registry.find_child_script(*root, first + row * 2 + place, 0);
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
    for (i16& value : bindings_)
        value = i16(std::clamp<i32>(value, 0, 31));
    (void)refresh_digits();
}

bool TitleKeyConfigMenu::learn(i32 button) {
    if (button < 0 || button > 30) return true;
    const i16 previous = bindings_[u32(row_)];
    if (previous == i16(button)) return true;
    for (i32 other = 0; other < kTitleKeyConfigBindings; ++other) {
        if (other != row_ && bindings_[u32(other)] == i16(button)) bindings_[u32(other)] = previous;
    }
    bindings_[u32(row_)] = i16(button);
    if (!refresh_digits()) return false;
    return sound(7);
}

void TitleKeyConfigMenu::select_row(i32 label_base) {
    if (!state.handles[kKeyConfigRoot]) return;
    (void)visuals.interrupt(kKeyConfigRoot, 3, true);
    (void)visuals.interrupt(kKeyConfigRoot, label_base + row_, false);
}

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
    if (state.substate == 0 || state.substate == 1) {
        if (!refresh_digits()) return false;
    }
    switch (state.substate) {
    case 0:
        if (!open()) return false;
        break;
    case 1:
        if (substate_frames_ > 6) {
            state.change_substate(2);
            substate_frames_ = 0;
            select_row(17);
        }
        break;
    case 2: {
        const i32 previous = row_;
        if (input & 0x10)
            row_ = (row_ + kTitleKeyConfigRows - 1) % kTitleKeyConfigRows;
        else if (input & 0x20)
            row_ = (row_ + 1) % kTitleKeyConfigRows;
        if (row_ != previous) {
            if (!sound(10)) return false;
            select_row(7);
        }
        if (row_ < kTitleKeyConfigBindings && host.pad_present()) {
            const u32 buttons = host.pad_buttons() & kRawPadButtonMask;
            for (i32 button = 0; button <= 30; ++button) {
                if (buttons & (1u << u32(button))) {
                    if (!learn(button)) return false;
                    break;
                }
            }
        }
        if ((pressed & 0x102) && row_ == 6) {
            revert_bindings();
            if (!sound(9)) return false;
            state.change_substate(4);
            substate_frames_ = 0;
            return true;
        }
        if (pressed & 0x80001) {
            if (row_ == 5) {
                revert_bindings();
                if (!sound(7)) return false;
            } else if (row_ == 6) {
                if (!host.persist_pad_bindings(bindings_)) fail("Title key config pad persistence unavailable");
                if (!sound(9)) return false;
                state.change_substate(4);
                substate_frames_ = 0;
            }
        }
        break;
    }
    case 3:
        break;
    case 4:
        if (substate_frames_ > 9) {
            if (!leave()) return false;
        }
        break;
    default:
        break;
    }
    return true;
}

}
