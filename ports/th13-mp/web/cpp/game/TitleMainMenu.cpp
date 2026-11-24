#include "TitleMainMenu.hpp"
#include "AnmRenderer.hpp"

namespace th13 {

bool TitleMainMenu::fail(const std::string& reason) {
    if (error.empty()) error = reason;
    return false;
}

bool TitleMainMenu::present(u32& handle) {
    if (animations.registry.find(handle)) return true;
    handle = 0;
    return false;
}

bool TitleMainMenu::create(i32 script) {
    state.handles[script] = animations.create(resources.title, script, -1, 0);
    return state.handles[script] || fail(animations.error);
}

bool TitleMainMenu::create_at(i32 script, const Vec3& translation) {
    state.handles[script] = animations.create(resources.title, script, -1, 2, translation);
    return state.handles[script] || fail(animations.error);
}

bool TitleMainMenu::interrupt(i32 script, i32 label, bool immediate) {
    return animations.interrupt(state.handles[script], label, immediate) || fail(animations.error);
}

bool TitleMainMenu::child_interrupt(i32 script, i32 label, bool immediate) {
    auto* root = animations.registry.find(state.handles[resources.main_root]);
    if (!root) {
        state.handles[resources.main_root] = 0;
        return fail("Title menu root unavailable");
    }
    auto* child = animations.registry.find_child_script(*root, script, 0);
    if (!child) return fail("Title menu child unavailable");
    child->pending_interrupt = label;
    return !immediate || animations.tick_instance(*child) >= 0 || fail(animations.error);
}

bool TitleMainMenu::sound(i32 id) {
    return play_sound ? play_sound(id) || fail("Title sound request failed") : fail("Title sound service unavailable");
}

bool TitleMainMenu::show_menu() {
    for (i32 item = 0; item < kTitleMainEntries; ++item) {
        if (item == state.menu.cursor) continue;
        const i32 label = item < state.menu.cursor ? 30 : 31;
        if (!child_interrupt(item + 3, label, true) || !child_interrupt(item + 13, label, true)) return false;
    }
    if (!extra_gate()) {
        if (!child_interrupt(4, 29, false) || !child_interrupt(14, 29, false)) return false;
    }
    return true;
}

bool TitleMainMenu::extra_gate() {
    for (i32 character = 0; character < 4; ++character)
        if (extra_unlocked && extra_unlocked(character)) return true;
    return false;
}

void TitleMainMenu::enter(i32 entry) {
    switch (TitleMainEntry(entry)) {
    case TitleMainEntry::StartGame:
    case TitleMainEntry::PracticeStart:
        progress.run_mode_flags =
            (progress.run_mode_flags & ~0x30u) | (entry == i32(TitleMainEntry::PracticeStart) ? 0x10u : 0u);
        progress.practice_mode = 0;
        if (settings.configured_difficulty > 3) settings.configured_difficulty = 1;
        state.flags |= 2u;
        state.change_screen(TitleScreen::Difficulty);
        state.menu.push();
        settings.saved_difficulty = progress.difficulty;
        state.menu.count = 4;
        state.menu.select(settings.configured_difficulty);
        break;
    case TitleMainEntry::ExtraStart:
        progress.run_mode_flags &= ~0x30u;
        progress.practice_mode = 0;
        settings.saved_difficulty = progress.difficulty;
        progress.difficulty = 4;
        state.flags |= 2u;
        state.change_screen(TitleScreen::Difficulty);
        state.menu.push();
        state.menu.count = 4;
        state.menu.select(0);
        break;
    case TitleMainEntry::Replay:
        state.flags |= 2u;
        state.menu.push();
        state.menu.count = kTitleMainEntries;
        state.menu.select(i32(TitleMainEntry::Replay));
        state.change_screen(TitleScreen::Replay);
        break;
    case TitleMainEntry::PlayerData:
        state.flags |= 2u;
        state.menu.push();
        state.menu.count = kTitleMainEntries;
        state.menu.select(i32(TitleMainEntry::PlayerData));
        state.change_screen(TitleScreen::PlayerData);
        break;
    case TitleMainEntry::SpellPractice:
        state.flags |= 2u;
        state.menu.push();
        state.menu.count = kTitleMainEntries;
        state.menu.select(i32(TitleMainEntry::SpellPractice));
        state.change_screen(TitleScreen::SpellPractice);
        break;
    case TitleMainEntry::Quit:
        state.change_screen(TitleScreen::Quit);
        break;
    case TitleMainEntry::Option:
        state.flags |= 2u;
        state.change_screen(TitleScreen::Option);
        break;
    case TitleMainEntry::MusicRoom:
        state.flags |= 2u;
        state.change_screen(TitleScreen::MusicRoom);
        break;
    case TitleMainEntry::Manual:
        state.flags |= 2u;
        state.change_screen(TitleScreen::Manual);
        break;
    default:
        break;
    }
}

bool TitleMainMenu::update(u32 pressed, u32 repeated) {
    ++menu_age;
    switch (state.substate) {
    case 0: {
        state.menu.count = kTitleMainEntries;
        state.menu.wrapping = true;
        state.menu.disabled_count = 0;
        if (!extra_gate() && !state.menu.disable(i32(TitleMainEntry::ExtraStart))) return fail(state.menu.error);
        const bool returning = (state.flags & 2u) != 0;
        if (returning) {
            if (!present(state.handles[resources.background_root])) {
                if (!create(resources.background_root)) return false;
                if (!interrupt(resources.background_root, 2, true)) return false;
            }
            if (i32(state.previous_screen) != i32(TitleScreen::Option) &&
                !interrupt(resources.background_root, 2, true))
                return false;
            if (!present(state.handles[resources.logo_root]) && !present(state.handles[resources.logo_restore_root])) {
                if (!create(resources.logo_restore_root)) return false;
            }
        } else {
            if (!present(state.handles[resources.background_root]) && !create(resources.background_root)) return false;
            if (!present(state.handles[resources.logo_root]) && !create(resources.logo_root)) return false;
        }
        state.change_substate(1);
        if (returning && !open_menu()) return false;
        [[fallthrough]];
    }
    case 1: {
        const bool returning = (state.flags & 2u) != 0;
        const i32 menu_at = returning ? 0 : 180;
        if (!returning && state.age.current == menu_at && !open_menu()) return false;
        const i32 ready_at = returning ? kTitleMenuReadyDelay : kTitleBootIntroFrames + kTitleMenuReadyDelay;
        if (state.age.current == ready_at) {
            if (!select_entry()) return false;
        }
        if (state.age.current >= ready_at) state.change_substate(2);
        break;
    }
    case 2: {
        state.menu.previous = state.menu.cursor;
        if ((pressed | repeated) & 0x10) state.menu.move(-1);
        if ((pressed | repeated) & 0x20) state.menu.move(1);
        if (!state.menu.error.empty()) return fail(state.menu.error);
        if (state.menu.previous != state.menu.cursor) {
            if (!sound(10)) return false;
            if (!interrupt(resources.main_root, 3, true) ||
                !interrupt(resources.main_root, 7 + i32(i16(state.menu.cursor)), false) || !show_menu())
                return false;
        }
        if (pressed & 0x102) {
            if (state.menu.cursor == kTitleMainEntries - 1) {
                if (!sound(9)) return false;
                state.change_substate(4);
                return true;
            }
            if (!sound(9)) return false;
            state.menu.select(kTitleMainEntries - 1);
            if (!interrupt(resources.main_root, 3, true) ||
                !interrupt(resources.main_root, i32(i16(state.menu.cursor)) + 7, false) || !show_menu())
                return false;
        }
        if (pressed & 0x80001) {
            const i32 selected = state.menu.cursor;
            if (selected < 0 || selected >= kTitleMainEntries) break;
            if (!interrupt(resources.main_root, 6, false)) return false;
            if (selected == i32(TitleMainEntry::Quit)) {
                if (!sound(9)) return false;
                state.change_substate(4);
                break;
            }
            if (!sound(7)) return false;
            state.confirmed_entry = selected;
            state.change_substate(4);
            if (selected != i32(TitleMainEntry::Option)) {
                if (state.version_bar && selected != kTitleMainEntries - 1)
                    animations.interrupt(state.version_bar, 1, false);
                if (!interrupt(resources.logo_root, 1, false)) return false;
                if (!interrupt(resources.logo_restore_root, 1, false)) return false;
                state.handles[resources.logo_root] = 0;
                state.handles[resources.logo_restore_root] = 0;
            }
        }
        break;
    }
    case 4: {
        if (state.age.current == 20 && state.handles[resources.background_root]) {
            const i32 selected = state.menu.cursor;
            const bool starts_game = selected == i32(TitleMainEntry::StartGame) ||
                                     selected == i32(TitleMainEntry::PracticeStart) ||
                                     selected == i32(TitleMainEntry::ExtraStart);
            if (starts_game) animations.interrupt(state.handles[resources.background_root], 3, true);
        }
        if (state.age.current > 19) enter(state.menu.cursor);
        break;
    }
    }
    return true;
}

void TitleMainMenu::apply_draw_layers() {
    clear_draw_layer_overrides();
    if (auto* bg = animations.registry.find(state.handles[resources.background_root])) {
        const bool selecting =
            state.handles[resources.difficulty_tree] != 0 || state.handles[resources.difficulty_tree_extra] != 0;
        if (auto* left = animations.registry.find_child_script(*bg, 90, 0))
            set_draw_layer_override(*left, selecting ? 22 : 23);
        if (auto* right = animations.registry.find_child_script(*bg, 91, 0)) set_draw_layer_override(*right, 22);
        if (auto* half = animations.registry.find_child_script(*bg, 93, 0)) {
            const i32 layer = selecting ? (state.particle_clock >= 28 ? 22 : 21) : (half->age.current <= 170 ? 23 : 21);
            set_draw_layer_override(*half, layer);
        }
    }
    if (auto* logo = animations.registry.find(state.handles[resources.logo_root])) {
        if (auto* flip = animations.registry.find_child_script(*logo, 97, 0))
            set_draw_layer_override(*flip, flip->age.current <= 60 ? 26 : 25);
        if (auto* burst = animations.registry.find_child_script(*logo, 98, 0)) set_draw_layer_override(*burst, 26);
    }
}

bool TitleMainMenu::open_menu() {
    menu_age = 0;
    if ((state.flags & 2u) && present(state.handles[resources.main_root])) {
        animations.retire(state.handles[resources.main_root]);
        state.handles[resources.main_root] = 0;
    }
    if (!present(state.handles[resources.main_root])) {
        if (!create(resources.main_root)) return false;
        auto* root = animations.registry.find(state.handles[resources.main_root]);
        if (!root) return fail("Title menu root unavailable");
    }
    if (!present(state.version_bar)) {
        state.version_bar = animations.create(resources.version, resources.version_bar, -1, 0);
        if (!state.version_bar) return fail(animations.error);
    }
    return show_menu();
}

bool TitleMainMenu::select_entry() {
    return interrupt(resources.main_root, 3, true) &&
           interrupt(resources.main_root, 17 + i32(i16(state.menu.cursor)), false) && show_menu();
}

}
