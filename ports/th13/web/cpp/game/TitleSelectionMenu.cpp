#include "TitleSelectionMenu.hpp"

#include <vector>

namespace th13 {
namespace {
void requeue_particles_to_back(AnmManager& animations, i32 bank, i32 script) {
    auto* resource = animations.resource(bank);
    if (!resource) return;
    std::vector<u32> handles;
    for (u32 index = 0;; index++) {
        const u32 handle = animations.registry.ordered_handle(false, index);
        if (!handle) break;
        AnmVm* vm = animations.registry.find(handle);
        if (vm && vm->resource == resource && vm->source_script == script) handles.push_back(handle);
        if (index > 8191) break;
    }
    for (const u32 handle : handles)
        if (AnmVm* vm = animations.registry.find(handle)) animations.registry.requeue_to_back(*vm);
}
}

bool TitleDifficultyMenu::check(bool ok) {
    if (!ok && error.empty()) error = state.menu.error.empty() ? visuals.error : state.menu.error;
    return ok;
}

bool TitleDifficultyMenu::sound(i32 id) {
    if (play_sound && play_sound(id)) return true;
    error = "Title difficulty sound service unavailable";
    return false;
}

bool TitleDifficultyMenu::build_difficulty() {
    const i32 tree = progress.difficulty >= 4 ? resources.difficulty_tree_extra : resources.difficulty_tree;
    if (state.handles[tree]) {
        if (!check(visuals.interrupt(tree, 1, false))) return false;
        state.handles[tree] = 0;
    }
    if (!check(visuals.create(tree))) return false;
    if (!check(visuals.create(resources.difficulty_badge))) return false;
    auto* tree_vm = animations.registry.find(state.handles[tree]);
    if (!tree_vm) return check(false);
    if (!check(visuals.interrupt(tree, 3, true))) return false;
    return check(visuals.interrupt(tree, 17 + state.menu.cursor, false));
}

void TitleDifficultyMenu::finish_menu_hides() {
    auto* menu_root = animations.registry.find(state.handles[0]);
    if (!menu_root) return;
    for (i32 item = 0; item < kTitleMainEntries; item++) {
        auto* white = animations.registry.find_child_script(*menu_root, item + 13, 0);
        if (white) {
            white->interpolators.alpha.duration = 0;
            if (item == state.confirmed_entry) {
                white->visible = false;
                white->visual.flags &= ~1u;
            } else {
                white->visual.layer = 25;
            }
        }
        if (item == state.confirmed_entry) {
            auto* row = animations.registry.find_child_script(*menu_root, item + 3, 0);
            if (!row) continue;
            row->interpolators.alpha.duration = 0;
            row->interpolators.color.duration = 0;
            row->visual.color = 0xcbffffffu;
            row->visual.secondary_color = 0xcbffffffu;
        }
    }
}

void TitleDifficultyMenu::spawn_particles(i32 count) {
    for (i32 i = 0; i < count; i++) {
        if (!animations.create(resources.ascii, resources.difficulty_particle, -1, 2)) {
            check(false);
            return;
        }
    }
}

void TitleDifficultyMenu::reorder_difficulty_draws() {
    if (auto* bg = animations.registry.find(state.handles[89])) {
        if (auto* left = animations.registry.find_child_script(*bg, 90, 0)) animations.registry.requeue_to_back(*left);
    }
    const i32 tree = progress.difficulty >= 4 ? resources.difficulty_tree_extra : resources.difficulty_tree;
    if (auto* root = animations.registry.find(state.handles[tree])) {
        for (const i32 script : {83, 84}) {
            if (auto* shape = animations.registry.find_child_script(*root, script, 0))
                animations.registry.requeue_to_back(*shape);
        }
    }
}

void TitleDifficultyMenu::advance_choreography() {
    const i32 f = frames;
    if (f < 0) return;
    static constexpr u8 kExtraAlpha[8] = {63, 56, 48, 40, 31, 23, 16, 8};
    static constexpr u8 kGreyAlpha[8] = {101, 89, 76, 63, 50, 38, 25, 12};
    static constexpr u8 kSelectedAlpha[16] = {203, 191, 178, 165, 152, 140, 127, 114, 101, 89, 76, 63, 50, 38, 25, 12};
    if (auto* menu_root = animations.registry.find(state.handles[0])) {
        for (i32 item = 0; item < kTitleMainEntries && f <= 8; item++) {
            if (item == state.confirmed_entry) continue;
            auto* child = animations.registry.find_child_script(*menu_root, item + 13, 0);
            if (!child) continue;
            if (f == 8) {
                child->visible = false;
                child->visual.flags &= ~1u;
                continue;
            }
            const u32 alpha = item == 1 ? kExtraAlpha[f] : kGreyAlpha[f];
            child->visual.color = alpha << 24 | (child->visual.color & 0xffffffu);
        }
        if (auto* selected = animations.registry.find_child_script(*menu_root, state.confirmed_entry + 3, 0)) {
            if (f < 16) {
                selected->visual.color = u32(kSelectedAlpha[f]) << 24 | (selected->visual.color & 0xffffffu);
            } else if (f == 16) {
                selected->visible = false;
                selected->visual.flags &= ~1u;
            }
        }
    }
    if (f == 1)
        spawn_particles(16);
    else if (f > 1 && (f - 1) % 3 == 0)
        spawn_particles(1);
    state.particle_clock = u32(f + 1);
    requeue_particles_to_back(animations, resources.ascii, resources.difficulty_particle);
    reorder_difficulty_draws();
}

void TitleDifficultyMenu::retire_difficulty() { visuals.retire(resources.difficulty_badge); }

void TitleDifficultyMenu::choose_character() noexcept {
    state.change_screen(TitleScreen::Character);
    ++state.particle_clock;
    if (progress.difficulty < 4) {
        settings.configured_difficulty = state.menu.cursor;
        progress.difficulty = state.menu.cursor;
    }
    state.menu.push();
    state.menu.wrapping = true;
    state.menu.count = kTitleCharacterRows;
    state.menu.select(settings.preferred_character + kTitleCharacterFirst);
    progress.character = settings.preferred_character;
}

void TitleDifficultyMenu::return_main() {
    progress.run_mode_flags &= ~0x30u;
    progress.practice_mode = 0;
    state.change_screen(TitleScreen::Main);
    state.menu.pop();
}

bool TitleDifficultyMenu::update(u32 pressed, u32 repeated) {
    switch (state.substate) {
    case 0:
        state.menu.wrapping = false;
        state.menu.count = ((i32(progress.difficulty >= 4) - 1) & 3) + 1;
        frames = 0;
        if (!build_difficulty()) return false;
        finish_menu_hides();
        state.change_substate(1);
        [[fallthrough]];
    case 1:
    case 2:
    case 3:
    case 4:
        advance_choreography();
        frames++;
        break;
    }
    switch (state.substate) {
    case 1:
        if (state.age.current > 6) state.change_substate(2);
        break;
    case 2:
        state.menu.previous = state.menu.cursor;
        if (progress.difficulty < 4) {
            if ((pressed | repeated) & 0x10) state.menu.move(-1);
            if ((pressed | repeated) & 0x20) state.menu.move(1);
            if (!state.menu.error.empty()) return check(false);
            if (state.menu.previous != state.menu.cursor) {
                if (!sound(10)) return false;
                if (!check(visuals.interrupt(resources.difficulty_tree, 3, true))) return false;
                if (!check(visuals.interrupt(resources.difficulty_tree, 7 + state.menu.cursor, false))) return false;
            }
        }
        if (pressed & 0x102) {
            const i32 tree = progress.difficulty >= 4 ? resources.difficulty_tree_extra : resources.difficulty_tree;
            if (!check(visuals.interrupt(tree, 1, false))) return false;
            state.handles[tree] = 0;
            state.change_substate(4);
            return sound(9);
        }
        if (pressed & 0x80001) {
            const i32 tree = progress.difficulty >= 4 ? resources.difficulty_tree_extra : resources.difficulty_tree;
            if (!check(visuals.interrupt(tree, 6, false))) return false;
            if (progress.difficulty < 4 && state.menu.cursor < 4) {
                auto* tree_vm = animations.registry.find(state.handles[tree]);
                if (!tree_vm) return check(false);
                auto* row = animations.registry.find_child_script(*tree_vm, 113 + state.menu.cursor, 0);
                if (!row) return check(false);
                if (!check(visuals.handle_interrupt(animations.registry.handle(*row), 2, false))) return false;
            }
            if (!check(visuals.retire(resources.difficulty_badge))) return false;
            state.change_substate(3);
            return sound(7);
        }
        break;
    case 3:
        if (state.age.current > 13) choose_character();
        break;
    case 4:
        if (state.age.current > 5) {
            if (progress.difficulty < 4) {
                settings.configured_difficulty = state.menu.cursor;
                progress.difficulty = state.menu.cursor;
            } else {
                settings.configured_difficulty = settings.saved_difficulty;
                progress.difficulty = settings.saved_difficulty;
            }
            retire_difficulty();
            return_main();
        }
        break;
    }
    return true;
}

bool TitleCharacterMenu::check(bool ok, const char* reason) {
    if (!ok && error.empty()) error = reason;
    return ok;
}

bool TitleCharacterMenu::visual(bool ok) {
    if (!ok && error.empty()) error = visuals.error;
    return ok;
}

bool TitleCharacterMenu::character_interrupt(i32 character, i32 label, bool immediate) {
    const bool extra = progress.difficulty == 4;
    const i32 root = extra ? resources.char_menu_root_ex : resources.char_menu_root;
    const i32 full = label + character;
    return visual(visuals.interrupt(root, full, immediate));
}

bool TitleCharacterMenu::exit_portrait(i32 character, i32 label) { return character_interrupt(character, label, true); }

bool TitleCharacterMenu::enter_portrait(i32 character, i32 label) {
    return character_interrupt(character, label, false);
}

void TitleCharacterMenu::return_difficulty() {
    visuals.retire(resources.difficulty_badge + 1);
    visuals.retire(progress.difficulty == 4 ? resources.char_menu_root_ex : resources.char_menu_root);
    visuals.retire(resources.character_root);
    state.change_screen(TitleScreen::Difficulty);
    state.menu.pop();
}

bool TitleCharacterMenu::update(u32 pressed, u32 repeated) {
    if (state.particle_clock % 3 == 2 && animations.resource(resources.ascii)) {
        if (!animations.create(resources.ascii, resources.difficulty_particle, -1, 2))
            return check(false, "Character particle spawn failed");
        requeue_particles_to_back(animations, resources.ascii, resources.difficulty_particle);
    }
    ++state.particle_clock;
    switch (state.substate) {
    case 0: {
        state.menu.count = kTitleCharacterRows;
        if (progress.difficulty == 4) {
            state.menu.disabled_count = 0;
            i32 first_unlocked = -1;
            for (i32 c = 0; c < 4; c++) {
                if (host.extra_unlocked(c)) {
                    if (first_unlocked < 0) first_unlocked = c;
                } else {
                    state.menu.disable(kTitleCharacterFirst + c);
                }
            }
            const i32 cur_char = title_character_at(state.menu.cursor);
            if (cur_char >= 0 && cur_char < 4 && !host.extra_unlocked(cur_char)) {
                if (first_unlocked >= 0) {
                    state.menu.select(kTitleCharacterFirst + first_unlocked);
                }
            }
        }
        if (!visual(visuals.create(resources.difficulty_badge + 1))) return false;
        const i32 menu_root = progress.difficulty == 4 ? resources.char_menu_root_ex : resources.char_menu_root;
        if (!animations.registry.find(state.handles[menu_root])) {
            if (!visual(visuals.retire(menu_root))) return false;
            if (!visual(visuals.create(menu_root))) return false;
            if (!visual(visuals.interrupt(menu_root, 3, true))) return false;
            if (!enter_portrait(title_character_at(state.menu.cursor), 7)) return false;
        }
        if (AnmVm* menu = animations.registry.find(state.handles[menu_root])) {
            const std::function<void(AnmVm&)> hide_tree = [&](AnmVm& vm) {
                vm.visual.flags &= ~2u;
                for (AnmVm* child : animations.registry.children(vm))
                    hide_tree(*child);
            };
            for (i32 c = 0; c < 4; c++) {
                if (host.character_cleared(c, progress.difficulty)) continue;
                if (AnmVm* tag = animations.registry.find_child_script(*menu, resources.clear_tag_base + c, 0))
                    hide_tree(*tag);
            }
        }
        state.change_substate(1);
        [[fallthrough]];
    }
    case 1:
        if (state.age.current > 6) state.change_substate(2);
        break;
    case 2: {
        state.menu.previous = state.menu.cursor;
        if ((pressed | repeated) & 0x40) {
            if (!check(host.sound(10), "Character navigation sound failed")) return false;
            if (!exit_portrait(title_character_at(state.menu.cursor), 22)) return false;
            state.menu.move(-1);
            if (!state.menu.error.empty()) return check(false, state.menu.error.c_str());
            if (!enter_portrait(title_character_at(state.menu.cursor), 12)) return false;
        }
        if ((pressed | repeated) & 0x80) {
            if (!check(host.sound(10), "Character navigation sound failed")) return false;
            if (!exit_portrait(title_character_at(state.menu.cursor), 17)) return false;
            state.menu.move(1);
            if (!state.menu.error.empty()) return check(false, state.menu.error.c_str());
            if (!enter_portrait(title_character_at(state.menu.cursor), 7)) return false;
        }
        if (pressed & 0x102) {
            state.change_substate(4);
            return check(host.sound(9), "Character cancel sound failed");
        }
        if (pressed & 0x80001) {
            const i32 menu_root = progress.difficulty == 4 ? resources.char_menu_root_ex : resources.char_menu_root;
            if (!visual(visuals.child_interrupt(
                    menu_root, resources.portrait_base + title_character_at(state.menu.cursor), 6, false)))
                return false;
            if (!check(host.sound(7), "Character confirm sound failed")) return false;
            state.change_substate(3);
        }
        break;
    }
    case 3:
        if (state.age.current == 10) {
            if ((progress.run_mode_flags & 0x30u) == 0x10u) {
                progress.character = settings.preferred_character = title_character_at(state.menu.cursor);
                state.menu.push();
                state.menu.count = 6;
                state.menu.wrapping = true;
                state.menu.select(settings.practice_stage);
                if (!visual(visuals.retire(resources.difficulty_badge + 1))) return false;
                state.change_screen(TitleScreen::Stage);
                return true;
            }
            if (!check(host.begin_transition(state.start_effect), "Game start transition unavailable")) return false;
        }
        if (state.age.current > 39) {
            progress.character = settings.preferred_character = title_character_at(state.menu.cursor);
            state.menu.push();
            state.change_screen(TitleScreen::StartGame);
            visuals.retire(resources.difficulty_badge + 1);
            progress.stage = progress.difficulty == 4 ? 7 : 1;
            progress.starting_stage = progress.stage;
            progress.practice_mode = 0;
            if (!check(host.start_game(progress.stage), "Game scene destination failed")) return false;
        }
        break;
    case 4:
        if (state.age.current > 5) return_difficulty();
        break;
    }
    return true;
}

bool TitleStageMenu::check(bool ok, const char* reason) {
    if (!ok && error.empty()) error = reason;
    return ok;
}

bool TitleStageMenu::available(i32 stage) {
    return host.practice_stage_played(progress.character, progress.difficulty, stage);
}

bool TitleStageMenu::update(u32 pressed, u32 repeated) {
    switch (state.substate) {
    case 0:
        state.menu.count = 6;
        state.menu.wrapping = true;
        state.menu.select(settings.practice_stage);
        if (!animations.registry.find(state.handles[89])) {
            if (!check(visuals.create(89) && visuals.interrupt(89, 3, true), "Practice return background unavailable"))
                return false;
        }
        if (!check(visuals.create(109) && visuals.create(112), "Practice stage animations unavailable")) return false;
        state.change_substate(1);
        [[fallthrough]];
    case 1:
        if (state.age.current > 10) state.change_substate(2);
        break;
    case 2:
        state.menu.previous = state.menu.cursor;
        if ((pressed | repeated) & 0x10) state.menu.move(-1);
        if ((pressed | repeated) & 0x20) state.menu.move(1);
        if (!check(state.menu.error.empty(), "Practice stage cursor failed")) return false;
        if (state.menu.previous != state.menu.cursor && !check(host.sound(10), "Practice stage cursor sound failed"))
            return false;
        if (pressed & 0x102) {
            settings.practice_stage = state.menu.cursor;
            state.change_substate(4);
            return check(host.sound(9), "Practice stage cancel sound failed");
        }
        if (pressed & 0x80001) {
            if (!available(state.menu.cursor + 1)) return check(host.sound(16), "Locked practice stage sound failed");
            settings.practice_stage = state.menu.cursor;
            state.change_substate(3);
            return check(host.sound(7) && host.sound(50), "Practice stage confirm sound failed");
        }
        break;
    case 3:
        if (state.age.current == 10) {
            if (!check(host.begin_transition(state.start_effect), "Practice start transition unavailable"))
                return false;
        }
        if (state.age.current > 39) {
            progress.stage = progress.starting_stage = state.menu.cursor + 1;
            progress.practice_mode = 1;
            progress.run_mode_flags = (progress.run_mode_flags & ~0x30u) | 0x10u;
            state.menu.push();
            state.change_screen(TitleScreen::StartGame);
            return check(host.start_game(progress.stage), "Practice game destination failed");
        }
        break;
    case 4:
        if (state.age.current > 5) {
            if (!check(visuals.retire(109) && visuals.retire(112), "Practice stage animation exit failed"))
                return false;
            state.menu.pop();
            state.change_screen(TitleScreen::Character);
        }
        break;
    }
    return true;
}

bool TitleStageMenu::draw_texts(HudDrawServices& host) {
    if (state.screen != TitleScreen::Stage || (state.substate != 2 && state.substate != 3)) return true;
    for (i32 row = 0; row < 6; ++row) {
        const bool played = available(row + 1);
        HudTextStyle style;
        style.color = row != state.menu.cursor                            ? 0xff808080u
                      : !played                                           ? 0xffdfdfdfu
                      : state.substate == 3 && state.age.current % 4 >= 2 ? 0xff000000u
                                                                          : 0xffffff00u;
        const std::string text = "Stage " + std::to_string(row + 1) + (played ? "" : "  ---------");
        if (!check(host.hud_text({{296, float(216 + row * 18), 0}, style, text}), "Practice stage text failed"))
            return false;
    }
    return true;
}

}
