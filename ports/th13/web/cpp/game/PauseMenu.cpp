#include "PauseMenu.hpp"

namespace th13 {

bool PauseMenu::check(bool ok, const char* reason) {
    if (!ok && error.empty()) error = reason;
    return ok;
}

bool PauseMenu::interrupt(u32 handle, i32 label, bool immediate) {
    return check(animations.interrupt(handle, label, immediate), "Pause animation interrupt failed");
}

bool PauseMenu::child(i32 script, i32 label) {
    auto* root = animations.registry.find(state.menu_animation);
    if (!root) {
        state.menu_animation = 0;
        return true;
    }
    auto* vm = animations.registry.find_child_script(*root, script, 0);
    return !vm || interrupt(animations.registry.handle(*vm), label);
}

bool PauseMenu::navigate(u32 pressed, u32 repeated, i32 label) {
    auto& menu = state.menu;
    menu.previous = menu.cursor;
    if ((pressed | repeated) & 0x10) menu.move(-1);
    if ((pressed | repeated) & 0x20) menu.move(1);
    if (!menu.error.empty()) return check(false, "Pause cursor movement failed");
    return menu.previous == menu.cursor ||
           (interrupt(state.menu_animation, i32(i16(menu.cursor)) + label) && host.sound(10));
}

// front132: 120/121/122/154/123；front133: 124/125/126（槽 0/1/4）；
// front134: 127/128（槽 1/4）；
// front142: 137/138/139/140/141。槽号同时决定 interrupt 7+slot。
i32 PauseMenu::entry_script(i32 entry) const {
    if (state.screen == PauseScreen::GameOver) return 137 + entry;
    if (state.screen == PauseScreen::PracticeEnd) return entry == 4 ? 128 : 127;
    if (state.practice) return entry == 4 ? 126 : 124 + entry;
    constexpr i32 scripts[] = {120, 121, 122, 154, 123};
    return scripts[entry];
}

bool PauseMenu::configure_entries() {
    auto& menu = state.menu;
    if (state.screen == PauseScreen::ExtraEnd) {
        menu.count=3; menu.wrapping=true; menu.disabled_count=0;
        if (!state.can_save_replay && !menu.disable(1)) return false;
        menu.select(0);
        return true;
    }
    menu.count = 5;
    menu.wrapping = true;
    menu.disabled_count = 0;
    // 43e780 disables save/options in practice; 43fa90 disables save after
    // a continue. PracticeEnd cannot resume.
    for (i32 entry : {2, 3}) {
        if ((entry == 2 && state.can_save_replay && !state.practice) ||
            (entry == 3 && state.can_open_options && !state.practice && state.screen != PauseScreen::PracticeEnd)) continue;
        if (!menu.disable(entry) ||
            (((!state.practice && state.screen != PauseScreen::PracticeEnd) ||
              state.screen == PauseScreen::GameOver) &&
             !child(entry_script(entry), 14))) return false;
    }
    if ((state.screen == PauseScreen::PracticeEnd ||
         (state.screen == PauseScreen::GameOver && !state.can_continue)) &&
        (!menu.disable(0) || !child(entry_script(0), 14))) return false;
    menu.select((state.screen == PauseScreen::PracticeEnd ||
                 (state.screen == PauseScreen::GameOver && !state.can_continue)) ? 1 : 0);
    return true;
}

bool PauseMenu::close() {
    return interrupt(state.menu_animation, 1) &&
           interrupt(state.snapshot_animation, 1);
}

bool PauseMenu::update(u32 pressed, u32 repeated) {
    if (!error.empty()) return false;
    auto& menu = state.menu;
    const i32 age = state.age.current;
    auto phase = [&](i32 next) { state.select_phase(next); };
    switch (state.phase) {
    case 0:  // 入场延迟后展开菜单
        if (age <= 9) break;
        if ((state.screen == PauseScreen::ExtraEnd || state.screen == PauseScreen::GameOver) && state.result_needs_name) {
            state.name_is_result=true; state.replay_name.clear(); state.name_cursor=90;
            state.result_needs_name=false; phase(18); break;
        }
        phase(6);
        if (!interrupt(state.menu_animation, 3, true) || !configure_entries()) return false;
        return
               interrupt(state.menu_animation, i32(i16(menu.cursor)) + 7);
    case 6:
        if (!navigate(pressed, repeated, 7)) return false;
        if (pressed & 0x80001) {
            if (!host.sound(7)) return false;
            if (state.screen == PauseScreen::ExtraEnd) {
                // 440960 state19: title, replay, retry.
                state.selected_action = menu.cursor == 0 ? 1 : menu.cursor == 1 ? 2 : 4;
                if (menu.cursor != 1 && !close()) return false;
                phase(16); return true;
            }
            state.selected_action = menu.cursor;
            if (menu.cursor == 0 || state.screen == PauseScreen::GameOver) {
                if (menu.cursor != 2 && menu.cursor != 3 && !close()) return false;
                phase(16);
            } else {
                if (!child(entry_script(menu.cursor), 6)) return false;
                // 43e97e/43ea10: flash the selected child with label6 even
                // for GameInf+74 != 0; that mode skips the Yes/No phase.
                phase(state.practice ? 16 : 8);
            }
            return true;
        }
        // 43e780 state 3: R selects retry and Q selects title directly.
        if (state.screen == PauseScreen::Pause &&
            (pressed & (0x10000u | 0x200000u))) {
            state.selected_action = (pressed & 0x10000u) ? 1 : 4;
            menu.select(state.selected_action);
            if (!host.sound(7) || !close()) return false;
            phase(16);
            return true;
        }
        if ((pressed & 0x100) && state.screen == PauseScreen::Pause) {
            if (!close()) return false;
            state.selected_action = 0;
            phase(16);
        } else if ((pressed & 0x102) && (state.screen == PauseScreen::GameOver || state.screen == PauseScreen::PracticeEnd)) {
            // GameOver 与 PracticeEnd 不能用 Esc 恢复一个已经结束的对局。
            menu.select(1);
            return interrupt(state.menu_animation, 8) && host.sound(9);
        }
        break;
    case 8:
        if (age < 20) break;
        if (!interrupt(state.menu_animation, 14, true)) return false;
        menu.count = 2;
        menu.disabled_count = 0;
        menu.select(1);  // 原版默认 No，不能误触重开/回标题。
        if (!interrupt(state.menu_animation, 16)) return false;
        phase(9);
        break;
    case 9:
        if (!navigate(pressed, repeated, 15)) return false;
        if (pressed & 0x80001) {
            if (!child(menu.cursor == 0 ? 130 : 131, 6) ||
                !host.sound(menu.cursor == 0 ? 7 : 9)) return false;
            phase(10);
        } else if (pressed & 0x102) {
            if (!host.sound(9)) return false;
            if (menu.cursor == 0) {
                menu.select(1);
                return interrupt(state.menu_animation, 16);
            }
            if (!child(131, 6)) return false;
            phase(10);
        }
        break;
    case 10:
        if (age < 20) break;
        if (menu.cursor == 0) {
            if (state.selected_action != 2 && state.selected_action != 3 && !close()) return false;
            phase(16);
        } else {
            const i32 selected = state.selected_action;
            if (!interrupt(state.menu_animation, 3, true) || !configure_entries()) return false;
            menu.select(selected);
            if (!interrupt(state.menu_animation, 7 + selected)) return false;
            phase(6);
        }
        break;
    case 16:  // 动画收场后执行所选动作
        if (age < 12) break;
        if (state.selected_action == 2) {
            state.replay_slot = 1; state.replay_name.clear(); state.name_cursor = 90; state.name_is_result=false;
            phase(17); break;
        }
        if (state.selected_action == 3) {
            state.option_row = 0; phase(19); break;
        }
        if (!check(host.finish_selection(state.selected_action), "Pause selected action failed"))
            return false;
        if (state.selected_action == 2 || state.selected_action == 3) {
            const i32 selected = state.selected_action;
            if (!interrupt(state.menu_animation, 3, true) || !configure_entries()) return false;
            menu.select(selected);
            if (!interrupt(state.menu_animation, 7 + selected)) return false;
            phase(6);
            break;
        }
        state.select(PauseScreen::Inactive);
        break;
    case 17: // 43e780 state9: select one of 25 replay slots.
        if (age <= 9) break;
        if ((pressed | repeated) & 0x10) state.replay_slot = (state.replay_slot + 23) % 25 + 1;
        if ((pressed | repeated) & 0x20) state.replay_slot = state.replay_slot % 25 + 1;
        if (pressed & 0x80001) { state.replay_name.clear(); state.name_cursor=90; phase(18); }
        else if (pressed & 0x102) {
            if (state.screen == PauseScreen::ExtraEnd || state.screen == PauseScreen::GameOver) {
                if (!interrupt(state.menu_animation,3,true) || !configure_entries()) return false;
                phase(6); break;
            }
            if (!close()) return false;
            state.selected_action=1; phase(16);
        }
        break;
    case 18: { // Name entry, up to eight characters.
        if (age <= 9) break;
        constexpr i32 count = 91;
        if ((pressed | repeated) & 0x10) state.name_cursor=(state.name_cursor+count-13)%count;
        if ((pressed | repeated) & 0x20) state.name_cursor=(state.name_cursor+13)%count;
        if ((pressed | repeated) & 0x40) state.name_cursor=(state.name_cursor+(state.name_cursor%13 == 0 ? 12 : count-1))%count;
        if ((pressed | repeated) & 0x80) state.name_cursor=(state.name_cursor+(state.name_cursor%13 == 12 ? count-12 : 1))%count;
        if (pressed & 0x80001) {
            if (state.name_cursor == 89) {
                if (!state.replay_name.empty()) state.replay_name.pop_back();
            } else if (state.name_cursor == 90) {
                if (state.name_is_result) {
                    if (!host.record_result_name(state.replay_name) ||
                        !interrupt(state.menu_animation,3,true) || !configure_entries()) return false;
                    phase(6); break;
                }
                if (!host.save_replay_slot(state.replay_slot,state.replay_name)) return false;
                phase(17);
            } else {
                const char ch=state.name_cursor == 88 ? ' ' : kResultNameCharacters[state.name_cursor];
                if (state.replay_name.size() < 8) state.replay_name += ch;
                else state.replay_name[7]=ch;
                if (state.replay_name.size() >= 8) state.name_cursor=90;
            }
        } else if (pressed & 0x102) {
            if (!state.replay_name.empty()) state.replay_name.pop_back();
            else if (state.name_is_result) { if (!configure_entries()) return false; phase(6); }
            else phase(17);
        }
        break;
    }
    case 19:
        if ((pressed | repeated) & 0x30) state.option_row ^= 1;
        if ((pressed | repeated) & 0x40) { if (!host.change_option(state.option_row,-5)) return false; }
        if ((pressed | repeated) & 0x80) { if (!host.change_option(state.option_row,5)) return false; }
        if (pressed & (0x80001|0x102)) { if (!configure_entries()) return false; phase(6); }
        break;
    }
    return true;
}

}  // namespace th13
