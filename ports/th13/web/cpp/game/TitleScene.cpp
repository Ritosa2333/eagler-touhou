#include "TitleScene.hpp"

namespace th13 {

TitleScene::TitleScene(SessionState& p, TitleSelectionSettings& choice, AnmManager& a,
                       TitleScenePlatform& h, FrameScheduler& schedule, TitleSceneResources bank)
    : progress(p), settings(choice), animations(a), host(h), scheduler(schedule), resources(bank),
      main(state, p, choice, a, bank), difficulty(state, p, choice, a, bank),
      character(state, p, choice, a, *this, bank),
      stage(state, p, choice, a, *this, bank), replay(state, a, h, bank),
      option(state, a, h, option_volumes, bank),
      key_config(state, a, key_config_host, bank), music_room(state, h), manual(state, h),
      frame(state, p, *this, schedule, a) {
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
        if (scene.player_data && scene.player_data_view &&
            scene.state.screen == TitleScreen::PlayerData &&
            !scene.player_data_view->draw(*scene.player_data, scene.host))
            return scene.check(false, scene.player_data_view->error) ? 1 : 5;
        if (scene.spell_practice && scene.spell_practice_view &&
            scene.state.screen == TitleScreen::SpellPractice && scene.spell_launch_age_ < 0 &&
            !scene.spell_practice_view->draw(*scene.spell_practice, scene.host, scene.host))
            return scene.check(false, scene.spell_practice_view->error) ? 1 : 5;
        return scene.check(scene.stage.draw_texts(scene.host), scene.stage.error) ? 1 : 5;
    };
    check(scheduler.add(stage_text_draw, FramePass::Draw, 2) >= 0,
          "Practice stage text callback registration failed");
}

TitleScene::~TitleScene() {
    scheduler.remove(stage_text_draw);
    // PlayerData 宿主表按 0x453170 case3 收面释放；场景提前析构也要闭合。
    if (player_data && player_data->opened()) host.close_player_data();
    for (auto& handle : state.handles) animations.retire(handle);
    animations.retire(state.version_bar);
    animations.retire(state.start_effect);
    animations.retire(state.replay_ascii_root);  // [R3-M] ascii s19 粒子根
    // 析构 bank 清单（蓝图 §五-12）：title=16 / title_v=17 / help=19（help 补漏）。
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

bool TitleScene::begin_transition(u32& animation) {
    // 开始游戏转场特效（effect bank script 0；th15 用 create(8,0)）。
    animation = animations.create(8, 0, -1, 0);
    return animation != 0;
}

bool TitleScene::start_game(i32 stage) {
    // destination=13：进入对局（原版 supervisor 语义，数值经 ApplicationState 隔离）。
    return host.start_game(stage);
}

bool TitleScene::character_cleared(i32 character, i32 difficulty) {
    // [r4-M] clear-tag 可见门数据源转发（TitleMenuServices → 宿主持久进度）。
    return host.character_cleared(character, difficulty);
}

bool TitleScene::extra_unlocked(i32 character) {
    // Extra 行解锁门转发（0x44c570 case0 +0x5c40 bit1 → 宿主 FileStore 持久位）。
    return host.extra_unlocked(character);
}

bool TitleScene::practice_stage_played(i32 character, i32 difficulty, i32 stage) {
    return host.practice_stage_played(character, difficulty, stage);
}

void TitleScene::advance_main_menu_selection() { main.advance_selection(); }

bool TitleScene::draw_title_replay_texts() {
    // Replay 屏动态文字：TitleFrame Draw 回调（优先级 2，先于全部 ANM 层）逐帧
    // 入队，AsciiFrame（Draw 78）同帧消费——原版 supervisor 0x452c30 的 ASCII
    // 队列通道等价路径。
    return check(replay.draw_texts(host), replay.error);
}

bool TitleScene::draw_title_music_room_texts() {
    // [r4-M2] 音楽室列表/注释：同 Replay 文本队列通道（0x4554f0 值域）。
    return check(music_room.draw_texts(host), music_room.error);
}

bool TitleScene::restore_practice_selection() {
    if (!returns_to_practice_selection(progress, 1, false))
        return check(false, "Invalid Practice return session");
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

// 屏值 10（0x453170）：每次进屏新建对合状态机。宿主未接 = open 如实失败
// （TitlePlayerDataMenu 默认平台），不以空表/零值冒充记录；成功后 X/Esc 走
// menu 自身 case2→case3，收面时 close/pop/change_screen(1) 全在菜单内完成。
bool TitleScene::update_player_data(const TitleFrameInput& value) {
    if (!player_data) {
        player_data = std::make_unique<TitlePlayerDataMenu>(state, host);
        player_data->play_sound = [this](i32 id) { return host.sound(id); };
        if (host.record_menu_views())
            player_data_view = std::make_unique<TitlePlayerDataView>(animations);
    }
    if (!player_data->update(value.pressed, value.repeated))
        return check(false, player_data->error);
    if (player_data_view && !player_data_view->update(*player_data,
            [this](AnmVm& vm, u32 color, const std::string& text, AnmTextAlignment alignment) {
                return host.rasterize_title_text(vm, color, text, alignment);
            })) return check(false, player_data_view->error);
    if (i32(state.screen) != i32(TitleScreen::PlayerData)) {
        // case3 已 close_player_data + pop + change_screen(Main)：丢弃旧实例，
        // 下次进屏从 case0 重新开表。
        if (player_data_view) player_data_view->release();
        player_data.reset();
    }
    return true;
}

// 屏值 17（0x4560a0/0x456650/0x4576c0）：三级菜单。
// `TitleSpellPracticeMenu::update` 的 false = 普通导航/禁用项（blocked），不是
// 帧错误；true = Cancel/Launch 终局事件。Cancel：光标栈 pop 回主菜单；Launch：
// 原版启动拍将角色、关卡、rank 与 DAT_004be7d4 一起交给宿主。
bool TitleScene::update_spell_practice(const TitleFrameInput& value) {
    if (spell_launch_age_ >= 0) {
        ++spell_launch_age_;
        // 004576c0 case3: transition at age10, launch after age39.
        if (spell_launch_age_ == 10 &&
            !host.begin_transition(state.start_effect))
            return check(false, "Spell Practice transition failed");
        if (spell_launch_age_ > 39) {
            if (!host.start_spell_practice(pending_spell_))
                return check(false, "Spell Practice launch failed");
            settings.preferred_character = pending_spell_.character;
            spell_launch_age_ = -1;
            spell_practice_launch_status_.clear();
            state.change_screen(TitleScreen::StartGame);
        }
        return true;
    }
    if (!spell_practice) {
        spell_practice = std::make_unique<TitleSpellPracticeMenu>(
            host, settings.preferred_character);
        spell_practice_launch_status_.clear();
        if (host.record_menu_views()) {
            spell_practice_view = std::make_unique<TitleSpellPracticeView>(animations, resources.title);
            if (!spell_practice_view->update(*spell_practice, host,
                    [this](AnmVm& vm, u32 color, const std::string& text) {
                        return host.rasterize_title_text(vm, color, text);
                    })) return check(false, spell_practice_view->error);
            spell_menu_delay_ = 11; // 004560a0 case1: age >10 before input.
        }
    }
    SpellPracticeInput input;
    const u32 navigation = value.pressed | value.repeated;
    input.up = (navigation & 0x10) != 0;
    input.down = (navigation & 0x20) != 0;
    input.left = (navigation & 0x40) != 0;
    input.right = (navigation & 0x80) != 0;
    input.confirm = (value.pressed & 0x80001) != 0;  // Z/Enter：单帧，不随重复
    input.cancel = (value.pressed & 0x102) != 0;     // X/Esc
    bool leaving_delay = false;
    if (spell_practice_view) {
        if (spell_menu_delay_ > 0) {
            if (--spell_menu_delay_ > 0) return true;
            input = delayed_spell_input_;
            delayed_spell_input_ = {};
            leaving_delay = true;
        } else if ((input.confirm && spell_practice->level() != SpellPracticeLevel::Difficulty) ||
                   input.cancel) {
            // Original consumes direction/character changes before confirm.
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
                        })) return check(false, spell_practice_view->error);
                if (input.confirm && !spell_practice_view->confirm(*spell_practice))
                    return check(false, spell_practice_view->error);
                if (!host.sound(input.cancel ? 9 : 7))
                    return check(false, "Spell Practice menu sound failed");
                delayed_spell_input_ = input;
                spell_menu_delay_ = input.cancel ? 6 :
                    spell_practice->level() == SpellPracticeLevel::Stage ? 20 : 14;
                return true;
            }
        }
    }
    const auto previous_level = spell_practice->level();
    const i32 previous_cursor = spell_practice->cursor();
    const i32 previous_character = spell_practice->character();
    const bool terminal = spell_practice->update(input);
    if (spell_practice_view &&
        (previous_cursor != spell_practice->cursor() ||
         previous_character != spell_practice->character()) &&
        !host.sound(10)) return check(false, "Spell Practice navigation sound failed");
    if (spell_practice_view && input.confirm && !terminal &&
        spell_practice->level() == previous_level && !spell_practice->blocked.empty() &&
        !host.sound(16)) return check(false, "Spell Practice locked sound failed");
    if (spell_practice_view && !spell_practice_view->update(*spell_practice, host,
            [this](AnmVm& vm, u32 color, const std::string& text) {
                return host.rasterize_title_text(vm, color, text);
            })) return check(false, spell_practice_view->error);
    if (spell_practice_view && leaving_delay && !terminal &&
        spell_practice->level() != previous_level)
        spell_menu_delay_ = 11; // each new selector enters through case1 age>10
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
            spell_practice_launch_status_ =
                "Spell practice launch unavailable: run routing not implemented";
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
        // 屏值 10（0x44c570 case a → 0x453170）：记录浏览对合。
        return update_player_data(value);
    case TitleScreen::Option:
        // 屏值 3（0x44c570 case 3 → 0x44dd00）：Option 屏驱动。
        return check(option.update(value.pressed, value.repeated), option.error);
    case TitleScreen::KeyConfig:
        // 屏值 4（0x44c570 case 4 → 0x44fcc0）：手柄五键配置屏。
        return check(key_config.update(value.pressed, value.repeated), key_config.error);
    case TitleScreen::Difficulty:
        return check(difficulty.update(value.pressed, value.repeated), difficulty.error);
    case TitleScreen::Character:
        return check(character.update(value.pressed, value.repeated), character.error);
    case TitleScreen::Stage:
        return check(stage.update(value.pressed, value.repeated), stage.error);
    case TitleScreen::Replay:
        return check(replay.update(value.pressed, value.repeated), replay.error);
    case TitleScreen::MusicRoom:
        // 屏值 13（0x44c570 case d → 0x4554f0）：音楽室驱动。
        return check(music_room.update(value.pressed, value.repeated), music_room.error);
    case TitleScreen::Manual:
        // 屏值 16（0x44c570 case 10 → 0x455330）：マニュアル supervisor wrapper。
        return check(manual.update(value.pressed, value.repeated), manual.error);
    case TitleScreen::SpellPractice:
        // 屏值 17（0x44c570 case 0x11/0x12/0x13）：三级符卡练习菜单。
        return update_spell_practice(value);
    default:
        return check(false, "Unsupported title scene menu");
    }
}

}  // namespace th13
