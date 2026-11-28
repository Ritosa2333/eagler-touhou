#include "TitleScene.hpp"
#include "AnmRenderer.hpp"

namespace th13 {

TitleScene::TitleScene(SessionState& p, TitleSelectionSettings& choice, AnmManager& a, TitleScenePlatform& h,
                       FrameScheduler& schedule, TitleSceneResources bank)
    : progress(p), settings(choice), animations(a), host(h), scheduler(schedule), resources(bank),
      main(state, p, choice, a, bank), difficulty(state, p, choice, a, bank),
      character(state, p, choice, a, *this, bank), stage(state, p, choice, a, *this, bank), replay(state, a, h, bank),
      option(state, a, h, option_volumes, bank), key_config(state, a, key_config_host, bank), music_room(state, h),
      manual(state, h), result(state, h, a, bank.title), frame(state, p, *this, schedule, a) {
    state.screen = TitleScreen::Initialize;
    state.flags = 0;
    main.play_sound = [this](i32 id) { return host.sound(id); };
    main.extra_unlocked = [this](i32 character) { return host.extra_unlocked(character); };
    difficulty.play_sound = [this](i32 id) { return host.sound(id); };
    replay.play_sound = [this](i32 id) { return host.sound(id); };
    option.play_sound = [this](i32 id) { return host.sound(id); };
    key_config.play_sound = [this](i32 id) { return host.sound(id); };
    music_room.play_sound = [this](i32 id) { return host.sound(id); };
    stage_text_draw.owner = this;
    stage_text_draw.enabled = true;
    stage_text_draw.run = [](void* owner) {
        auto& scene = *static_cast<TitleScene*>(owner);
        scene.main.apply_draw_layers();
        if (scene.player_data && scene.player_data_view && scene.state.screen == TitleScreen::PlayerData &&
            !scene.player_data_view->draw(*scene.player_data, scene.host))
            return scene.check(false, scene.player_data_view->error) ? 1 : 5;
        if (scene.spell_practice && scene.spell_practice_view && scene.state.screen == TitleScreen::SpellPractice &&
            scene.spell_launch_age_ < 0 &&
            !scene.spell_practice_view->draw(*scene.spell_practice, scene.host, scene.host))
            return scene.check(false, scene.spell_practice_view->error) ? 1 : 5;
        if ((scene.state.screen == TitleScreen::ResultName || scene.state.screen == TitleScreen::ResultReplaySlot) &&
            !scene.result.draw_texts(scene.host))
            return scene.check(false, scene.result.error) ? 1 : 5;
        return scene.check(scene.stage.draw_texts(scene.host), scene.stage.error) ? 1 : 5;
    };
    check(scheduler.add(stage_text_draw, FramePass::Draw, 2) >= 0, "Practice stage text callback registration failed");
}

TitleScene::~TitleScene() {
    scheduler.remove(stage_text_draw);
    clear_draw_layer_overrides();
    if (player_data && player_data->opened()) host.close_player_data();
    if (music_room_view) music_room_view->release();
    if (state.screen == TitleScreen::Manual) host.close_manual();
    for (auto& handle : state.handles)
        animations.retire(handle);
    animations.retire(state.version_bar);
    animations.retire(state.start_effect);
    animations.retire(state.replay_ascii_root);
    for (const i32 bank : {resources.title, resources.version, resources.help})
        animations.retire_resource(bank);
}

bool TitleScene::check(bool ok, const std::string& why) {
    if (!ok && error.empty()) error = why.empty() ? "Title scene operation failed" : why;
    return ok;
}

bool TitleScene::clear_title_overlay() { return host.clear_title_overlay(); }
bool TitleScene::queue_title_music(i32 code, i32 value, const std::string& name) {
    return host.queue_title_music(code, value, name);
}
bool TitleScene::clear_current_wave() { return host.clear_current_wave(); }
bool TitleScene::title_exit(i32 value) { return host.title_exit(value); }
bool TitleScene::fade_out_title_music() { return host.fade_out_title_music(); }

bool TitleScene::sound(i32 id) { return host.sound(id); }

bool TitleScene::begin_transition(u32& animation) { return host.begin_transition(animation); }

bool TitleScene::start_game(i32 stage) { return host.start_game(stage); }

bool TitleScene::character_cleared(i32 character, i32 difficulty) {
    return host.character_cleared(character, difficulty);
}

bool TitleScene::extra_unlocked(i32 character) { return host.extra_unlocked(character); }

bool TitleScene::practice_stage_played(i32 character, i32 difficulty, i32 stage) {
    return host.practice_stage_played(character, difficulty, stage);
}

bool TitleScene::draw_title_replay_texts() { return check(replay.draw_texts(host), replay.error); }

bool TitleScene::draw_title_music_room_texts() {
    if (host.record_menu_views()) return true;
    return check(music_room.draw_texts(host), music_room.error);
}

bool TitleScene::restore_practice_selection() {
    if (!returns_to_practice_selection(progress, 1, false)) return check(false, "Invalid Practice return session");
    settings.configured_difficulty = progress.difficulty;
    settings.preferred_character = progress.character;
    settings.practice_stage = progress.starting_stage - 1;
    state.menu.depth = state.menu.disabled_count = 0;
    state.menu.count = kTitleMainEntries;
    state.menu.select(i32(TitleMainEntry::PracticeStart));
    state.menu.push();
    state.menu.count = 4;
    state.menu.select(progress.difficulty);
    state.menu.push();
    state.menu.count = kTitleCharacterRows;
    state.menu.select(progress.character + kTitleCharacterFirst);
    state.menu.push();
    state.menu.count = 6;
    state.menu.select(settings.practice_stage);
    state.change_screen(TitleScreen::Stage);
    return true;
}

bool TitleScene::restore_spell_practice_selection() {
    if (!returns_to_spell_practice_selection(progress, 1, false))
        return check(false, "Invalid Spell Practice return session");
    const i32 stage = progress.starting_stage;
    for (i32 slot = 0; slot < kSpellPracticeSlots[stage - 1]; ++slot)
        for (i32 difficulty = 0; difficulty < kSpellPracticeDifficulties; ++difficulty)
            if (host.spell_id(stage, slot, difficulty) == progress.selected_spell_id) {
                spell_restore_ = SpellPracticeCommand{};
                spell_restore_.stage = stage;
                spell_restore_.spell_slot = slot;
                spell_restore_.difficulty = difficulty;
                spell_restore_.spell_id = progress.selected_spell_id;
                spell_restore_pending_ = true;
                settings.preferred_character = progress.character;
                if (!animations.registry.find(state.handles[resources.background_root])) {
                    state.handles[resources.background_root] =
                        animations.create(resources.title, resources.background_root, -1, 0);
                    if (!state.handles[resources.background_root])
                        return check(false, animations.error.empty() ? "Spell Practice return background unavailable"
                                                                     : animations.error);
                }
                if (!animations.interrupt(state.handles[resources.background_root], 3, true))
                    return check(false, animations.error.empty() ? "Spell Practice return background interrupt failed"
                                                                 : animations.error);
                state.flags |= 2u;
                state.menu.depth = state.menu.disabled_count = 0;
                state.menu.count = kTitleMainEntries;
                state.menu.select(i32(TitleMainEntry::SpellPractice));
                state.menu.push();
                state.menu.count = kTitleMainEntries;
                state.menu.select(i32(TitleMainEntry::SpellPractice));
                state.change_screen(TitleScreen::SpellPractice);
                return true;
            }
    return check(false, "Spell Practice selection no longer in catalog");
}

bool TitleScene::update_player_data(const TitleFrameInput& value) {
    if (!player_data) {
        player_data = std::make_unique<TitlePlayerDataMenu>(state, host);
        player_data->play_sound = [this](i32 id) { return host.sound(id); };
        if (host.record_menu_views()) player_data_view = std::make_unique<TitlePlayerDataView>(animations);
    }
    if (!player_data->update(value.pressed, value.repeated)) return check(false, player_data->error);
    if (player_data_view &&
        !player_data_view->update(*player_data,
                                  [this](AnmVm& vm, u32 color, const std::string& text, AnmTextAlignment alignment) {
                                      return host.rasterize_title_text(vm, color, text, alignment);
                                  }))
        return check(false, player_data_view->error);
    if (i32(state.screen) != i32(TitleScreen::PlayerData)) {
        if (player_data_view) player_data_view->release();
        player_data.reset();
    }
    return true;
}

bool TitleScene::update_spell_practice(const TitleFrameInput& value) {
    if (spell_launch_age_ >= 0) {
        ++spell_launch_age_;
        if (spell_launch_age_ == 10 && !host.begin_transition(state.start_effect))
            return check(false, "Spell Practice transition failed");
        if (spell_launch_age_ > 39) {
            if (!host.start_spell_practice(pending_spell_)) return check(false, "Spell Practice launch failed");
            settings.preferred_character = pending_spell_.character;
            spell_launch_age_ = -1;
            spell_practice_launch_status_.clear();
            state.change_screen(TitleScreen::StartGame);
        }
        return true;
    }
    if (!spell_practice) {
        spell_practice = std::make_unique<TitleSpellPracticeMenu>(host, settings.preferred_character);
        if (spell_restore_pending_) {
            if (!spell_practice->restore_selection(spell_restore_.stage, spell_restore_.spell_slot,
                                                   spell_restore_.difficulty))
                return check(false, "Spell Practice restore rejected by catalog");
            spell_restore_pending_ = false;
        }
        spell_practice_launch_status_.clear();
        if (host.record_menu_views()) {
            spell_practice_view = std::make_unique<TitleSpellPracticeView>(animations, resources.title);
            if (!spell_practice_view->update(*spell_practice, host,
                                             [this](AnmVm& vm, u32 color, const std::string& text) {
                                                 return host.rasterize_title_text(vm, color, text);
                                             }))
                return check(false, spell_practice_view->error);
            spell_menu_delay_ = 11;
        }
    }
    SpellPracticeInput input;
    const u32 navigation = value.pressed | value.repeated;
    input.up = (navigation & 0x10) != 0;
    input.down = (navigation & 0x20) != 0;
    input.left = (navigation & 0x40) != 0;
    input.right = (navigation & 0x80) != 0;
    input.confirm = (value.pressed & 0x80001) != 0;
    input.cancel = (value.pressed & 0x102) != 0;
    bool leaving_delay = false;
    if (spell_practice_view) {
        if (spell_menu_delay_ > 0) {
            if (--spell_menu_delay_ > 0) return true;
            input = delayed_spell_input_;
            delayed_spell_input_ = {};
            leaving_delay = true;
        } else if ((input.confirm && spell_practice->level() != SpellPracticeLevel::Difficulty) || input.cancel) {
            SpellPracticeInput navigation = input;
            navigation.confirm = navigation.cancel = false;
            spell_practice->update(navigation);
            input.up = input.down = input.left = input.right = false;
            const bool accepted = input.cancel || spell_practice->level() == SpellPracticeLevel::Stage ||
                                  (spell_practice->cursor() >= 0 &&
                                   size_t(spell_practice->cursor()) < spell_practice->spell_rows().size() &&
                                   spell_practice->spell_rows()[size_t(spell_practice->cursor())].unlocked);
            if (accepted) {
                if (!spell_practice_view->update(*spell_practice, host,
                                                 [this](AnmVm& vm, u32 color, const std::string& text) {
                                                     return host.rasterize_title_text(vm, color, text);
                                                 }))
                    return check(false, spell_practice_view->error);
                if (input.confirm && !spell_practice_view->confirm(*spell_practice))
                    return check(false, spell_practice_view->error);
                if (!host.sound(input.cancel ? 9 : 7)) return check(false, "Spell Practice menu sound failed");
                delayed_spell_input_ = input;
                spell_menu_delay_ = input.cancel ? 6 : spell_practice->level() == SpellPracticeLevel::Stage ? 20 : 14;
                return true;
            }
        }
    }
    const auto previous_level = spell_practice->level();
    const i32 previous_cursor = spell_practice->cursor();
    const i32 previous_character = spell_practice->character();
    const bool terminal = spell_practice->update(input);
    if (spell_practice_view &&
        (previous_cursor != spell_practice->cursor() || previous_character != spell_practice->character()) &&
        !host.sound(10))
        return check(false, "Spell Practice navigation sound failed");
    if (spell_practice_view && input.confirm && !terminal && spell_practice->level() == previous_level &&
        !spell_practice->blocked.empty() && !host.sound(16))
        return check(false, "Spell Practice locked sound failed");
    if (spell_practice_view &&
        !spell_practice_view->update(*spell_practice, host, [this](AnmVm& vm, u32 color, const std::string& text) {
            return host.rasterize_title_text(vm, color, text);
        }))
        return check(false, spell_practice_view->error);
    if (spell_practice_view && leaving_delay && !terminal && spell_practice->level() != previous_level)
        spell_menu_delay_ = 11;
    if (!terminal) return true;
    switch (spell_practice->command().action) {
    case SpellPracticeAction::Cancel:
        spell_practice->clear_command();
        if (spell_practice_view) spell_practice_view->release();
        spell_practice.reset();
        spell_practice_launch_status_.clear();
        if (state.menu.depth > 0) state.menu.pop();
        state.change_screen(TitleScreen::Main);
        break;
    case SpellPracticeAction::Launch:
        if (host.can_start_spell_practice(spell_practice->command())) {
            if (spell_practice_view && !spell_practice_view->confirm(*spell_practice))
                return check(false, spell_practice_view->error);
            pending_spell_ = spell_practice->command();
            spell_launch_age_ = 0;
            if (!host.sound(7) || !host.fade_out_title_music() || !host.sound(0x32))
                return check(false, "Spell Practice launch sound failed");
        } else {
            spell_practice_launch_status_ = "Spell practice launch unavailable: run routing not implemented";
        }
        break;
    default:
        break;
    }
    return true;
}

bool TitleScene::update_title_menu(TitleScreen screen, const TitleFrameInput& value) {
    input = value;
    switch (screen) {
    case TitleScreen::Main:
        return check(main.update(value.pressed, value.repeated), main.error);
    case TitleScreen::PlayerData:
        return update_player_data(value);
    case TitleScreen::Option:
        return check(option.update(value.pressed, value.repeated), option.error);
    case TitleScreen::KeyConfig:
        return check(key_config.update(value.pressed, value.repeated), key_config.error);
    case TitleScreen::Difficulty:
        return check(difficulty.update(value.pressed, value.repeated), difficulty.error);
    case TitleScreen::Character:
        return check(character.update(value.pressed, value.repeated), character.error);
    case TitleScreen::Stage:
        return check(stage.update(value.pressed, value.repeated), stage.error);
    case TitleScreen::Replay:
        return check(replay.update(value.pressed, value.repeated), replay.error);
    case TitleScreen::ResultName:
    case TitleScreen::ResultReplaySlot:
        return check(result.update(value.pressed, value.repeated), result.error);
    case TitleScreen::MusicRoom:
        if (host.record_menu_views() && !music_room_view)
            music_room_view = std::make_unique<TitleMusicRoomView>(animations, resources.title, 0, resources.ascii);
        if (!check(music_room.update(value.pressed, value.repeated), music_room.error)) return false;
        if (music_room_view &&
            !music_room_view->update(state, music_room, host,
                                     [this](AnmVm& vm, u32 color, const std::string& text, AnmTextAlignment alignment) {
                                         return host.rasterize_title_text(vm, color, text, alignment);
                                     }))
            return check(false, music_room_view->error);
        if (state.screen != TitleScreen::MusicRoom && music_room_view) {
            music_room_view->release();
            music_room_view.reset();
        }
        return true;
    case TitleScreen::Manual:
        return check(manual.update(value.pressed, value.repeated), manual.error);
    case TitleScreen::SpellPractice:
        return update_spell_practice(value);
    default:
        return check(false, "Unsupported title scene menu");
    }
}

}
