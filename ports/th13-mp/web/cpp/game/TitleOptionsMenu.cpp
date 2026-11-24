#include "TitleOptionsMenu.hpp"

#include <algorithm>

namespace th13 {

namespace {
constexpr i32 kOptionRoot = 1;
constexpr i32 kOptionRows = 5;
constexpr i32 kDigitBgmSmall = 33, kDigitBgmLarge = 37;
constexpr i32 kDigitSeSmall = 41, kDigitSeLarge = 45;
constexpr i32 kSpriteSmall = 50, kSpriteLarge = 61;
}

bool TitleOptionsMenu::fail(const std::string& reason) {
    if (error.empty()) error = reason;
    return false;
}

bool TitleOptionsMenu::sound(i32 id) {
    if (play_sound && play_sound(id)) return true;
    return fail("Title option sound service unavailable");
}

bool TitleOptionsMenu::open() {
    row_ = 0;
    if (!pushed_) {
        state.menu.push();
        pushed_ = true;
    }
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

bool TitleOptionsMenu::refresh_volumes() {
    host.apply_volumes(volumes.bgm, volumes.effect);
    auto* root = animations.registry.find(state.handles[kOptionRoot]);
    if (!root) return true;
    const i32 values[2] = {std::clamp(volumes.bgm, 0, 100), std::clamp(volumes.effect, 0, 100)};
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
                if (!vm->select_sprite((base == kDigitBgmSmall || base == kDigitSeSmall ? kSpriteSmall : kSpriteLarge) +
                                       digits[place]))
                    return fail("Title option digit sprite select failed");
                if (visible[place]) {
                    vm->visual.flags |= 2u;
                } else {
                    vm->visual.flags &= ~2u;
                }
            }
        }
    }
    return true;
}

void TitleOptionsMenu::select_row(i32 label_base) {
    (void)visuals.interrupt(kOptionRoot, 3, true);
    (void)visuals.interrupt(kOptionRoot, label_base + row_, false);
}

bool TitleOptionsMenu::leave(bool to_key_config) {
    if (to_key_config) {
        if (!host.open_key_config()) return fail("Title option key config screen not wired");
        (void)visuals.interrupt(kOptionRoot, 1, false);
        state.handles[kOptionRoot] = 0;
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
    if (state.substate != 0 && state.substate != 4) {
        if (!refresh_volumes()) return false;
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
            row_ = (row_ + kOptionRows - 1) % kOptionRows;
        else if (input & 0x20)
            row_ = (row_ + 1) % kOptionRows;
        if (row_ != previous) {
            if (!sound(10)) return false;
            select_row(7);
        }
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
        if (row_ == 0 || row_ == 1) {
            i32& value = row_ == 0 ? volumes.bgm : volumes.effect;
            const i32 before = value;
            if (input & 0x40) value = value < 5 ? 0 : value - 5;
            if (input & 0x80) value = value > 95 ? 100 : value + 5;
            if (value != before) {
                if (!refresh_volumes()) return false;
            }
        }
        if (input & 0x80001) {
            if (row_ == 2) {
                if (!sound(7)) return false;
                key_config_ = true;
                state.change_substate(4);
                substate_frames_ = 0;
            } else if (row_ == 3) {
                volumes.bgm = 100;
                volumes.effect = 80;
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
    case 3:
        break;
    case 4:
        if (substate_frames_ > 9) {
            if (!leave(key_config_)) return false;
        }
        break;
    default:
        break;
    }
    return true;
}

}
