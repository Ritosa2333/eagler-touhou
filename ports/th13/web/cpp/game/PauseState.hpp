// 暂停状态（th15 PauseState 裁剪：仅 Inactive/Pause/GameOver，
// Results/成绩登录/录像保存延后，ui-base.md §三）。
#pragma once

#include "MenuCursor.hpp"
#include "Timer.hpp"
#include <string>

namespace th13 {
inline constexpr char kResultNameCharacters[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=.,!?@:;[]()_/{}|~^#$%&*   ";

// BGameOver = 0x43f570 mode≠0 入口（0x440960 态 0x18）：与 ExtraEnd 同用
// 147 屏根/三项菜单，但默认光标落在重试（0x440960 态 0x18 的 0x15030(2)）。
enum class PauseScreen : i32 {
    Inactive = 0, Pause = 1, GameOver = 2, PracticeEnd = 3, ExtraEnd = 4, BGameOver = 5
};

struct PauseState {
    Timer age, selection_age;
    MenuCursor menu;
    PauseScreen screen = PauseScreen::Inactive, previous = PauseScreen::Inactive;
    i32 phase = 0;
    u32 flags = 0;
    u32 menu_animation = 0, snapshot_animation = 0;
    i32 front_bank = -1;
    bool practice = false;
    bool can_continue = false, can_save_replay = false, can_open_options = false;
    i32 selected_action = 0;
    i32 replay_slot = 1, name_cursor = 0, option_row = 0;
    std::string replay_name;
    bool name_is_result = false, result_needs_name = false;
    float saved_rate = 1;

    PauseState() {
        age.set(0);
        selection_age.set(0);
        menu.wrapping = false;
    }

    void select(PauseScreen next) noexcept {
        previous = screen;
        screen = next;
        phase = 0;
        menu.disabled_count = 0;
        age.set(0);
        selection_age.set(0);
    }

    void select_phase(i32 next) noexcept {
        phase = next;
        age.set(0);
    }
};

}  // namespace th13
