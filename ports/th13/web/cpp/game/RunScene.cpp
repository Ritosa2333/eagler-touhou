#include "RunScene.hpp"

#include <algorithm>
#include <cstdio>

namespace th13 {
RunDialogObserver run_dialog_observer = nullptr;

namespace {
constexpr i32 kRunUpdatePriority = 20;
constexpr std::array<i32, 5> kStageDrawPriorities = {20, 24, 19, 32, 29};
constexpr std::array<i32, 2> kSceneryDrawPriorities = {2, 5};
}

RunScene::RunScene(SessionState& p, HudPlayerState& player, HudScoreState& score, AnmManager& manager,
                   AnmRenderer& scene_view, AnmEnvironment& e, RunSceneServices& h, FrameScheduler& f,
                   StagePresentation::Banks banks, i32 front, i32 ascii, i32 scenery_bank, const std::string& std_name,
                   GameRandom* shared_random, GameHud* shared_hud, StagePlayerState* shared_player)
    : progress(p), animations(manager), scene_renderer(scene_view), environment(e), host(h), scheduler(f),
      player_state(player), score_state(score), ascii_bank(ascii), scenery_bank(scenery_bank), scenery_std(std_name),
      msg_presentation(
          std::make_unique<MsgPresentation>(animations, MsgPresentation::Banks{banks.player, banks.stage})),
      post_flow(p, player, score, *this),
      owned_hud(shared_hud ? nullptr : std::make_unique<GameHud>(animations, p, player, score, front, ascii)),
      hud(shared_hud ? *shared_hud : *owned_hud), battle(shared_random, shared_player),
      presentation(std::make_unique<StagePresentation>(animations, scene_renderer, banks, kPlayerShotSprite)) {
    spell_presentation =
        std::make_unique<SpellPresentation>(animations, SpellPresentation::Banks{banks.stage, p.stage});
    age.set(0);
    callbacks[0].owner = callbacks[1].owner = this;
    callbacks[0].enabled = callbacks[1].enabled = true;
    callbacks[0].run = [](void* p) {
        return static_cast<RunScene*>(p)->update() ? i32(FrameAction::Continue) : i32(FrameAction::Error);
    };
    callbacks[1].run = [](void* p) {
        return static_cast<RunScene*>(p)->draw() ? i32(FrameAction::Continue) : i32(FrameAction::Error);
    };
    if (scheduler.add(callbacks[0], FramePass::Update, kRunUpdatePriority) < 0)
        fail("Run scene callback registration failed");
    if (scheduler.add(callbacks[1], FramePass::Draw, kRunTextPriority) < 0)
        fail("Run scene text callback registration failed");
    callbacks[2].owner = this;
    callbacks[2].enabled = true;
    callbacks[2].run = [](void* p) {
        auto& scene = *static_cast<RunScene*>(p);
        if (scene.hud.draw_trance_countdown(scene.host)) return i32(FrameAction::Continue);
        scene.fail(scene.hud.error);
        return i32(FrameAction::Error);
    };
    if (scheduler.add(callbacks[2], FramePass::Draw, 51) < 0) fail("Trance countdown callback registration failed");
    laser_presentation = std::make_unique<LaserPresentation>(manager, scene_view, battle.sim, banks.bullet);
    battle.sim.defer_laser_update = true;
    battle.sim.defer_bullet_update = true;
    battle.defer_item_side = true;
    battle.sim.laser_sound = [this](i32 id) { return host.hud_sound(id); };
    battle.sim.enemy_sound = [this](i32 id) { return host.hud_sound(id); };
    battle.sim.enemy_position_sound = [this](i32 id, float x) { return host.hud_sound(id, x); };
    battle.sim.time_rate_changed = [this](float rate) { animations.rate = rate; };
    battle.sim.screen_shake = [this](i32 duration, i32 start, i32 end) { return start_shake(duration, start, end); };
    battle.bomb_screen_shake = [this](i32 magnitude, i32 rise, i32 hold, i32 fall) {
        return start_shake(ScreenShake::bomb(magnitude, rise, hold, fall));
    };
    battle.reimu_explosion_shake = [this]() { return start_shake(8, 6, 6); };
    environment.stage_difficulty = p.difficulty;
    environment.chapter = &p.chapter;
    battle.sim.scene_effect = [this, banks](i32 kind, const Vec3& position) {
        const auto action = scene_effect_action(kind);
        if (action == SceneEffectAction::None) {
            std::fprintf(stderr, "[scene-effect] out-of-table kind %d at (%.1f,%.1f)\n", kind, position.x, position.y);
            return true;
        }
        const u32 handle = animations.create(banks.effect, action == SceneEffectAction::Effect136 ? 136 : 0, -1, 0,
                                             action == SceneEffectAction::Effect136 ? Vec3{} : position);
        auto* vm = animations.registry.find(handle);
        return vm && animations.spawn_effect(*vm, kind);
    };
    battle.sim.enemy_hit_sound = [this](i32 id, float x) { return host.hud_sound(id, x); };
    battle.sim.enemy_fog = [this](StageEnemy& enemy, bool reset) { return presentation->enemy_fog(enemy, reset); };
    battle.sim.enemy_death_effect = [this](i32 file, i32 script, const Vec3& pos, float angle) {
        return presentation->enemy_death(file, script, pos, angle);
    };
    battle.sim.bullet_cancel_effect = [this](i32 script, const Vec3& pos, const Vec3& displacement) {
        return presentation->bullet_cancel(script, pos, displacement);
    };
    battle.sim.stationary_cancel_effect = [this](i32 script, const Vec3& pos) {
        return presentation->bullet_cancel(script, pos, {}, false);
    };
    battle.sim.player_effect = [this](i32 script, const Vec3& pos, i32 layer) {
        const u32 h = presentation->player_effect(script, pos, layer);
        return h != 0;
    };
    battle.sim.graze_effect = [this](const Vec3& pos) { return presentation->graze_effect(pos); };
    battle.beam_hit_effect = [this](const Vec3& position, const Vec3& displacement, float angle) {
        return presentation->beam_hit_effect(position, displacement, angle);
    };
    battle.player_shot_birth = [this](PlayerShot& shot, u32 index) {
        return presentation->initialize_player_shot(shot, index);
    };
    battle.youmu_bomb_update = [this](StageBattle& state) { return presentation->sync_youmu_slash(state); };
    battle.youmu_charge_effect = [this](i32 action, const Vec3& position) {
        return presentation->youmu_charge_effect(action, position);
    };
    battle.sim.flush_anm_slots = [this](StageEnemy* enemy) {
        if (!presentation->flush_ecl_animations(battle, enemy, true)) {
            battle.sim.error_ = presentation->error();
            return false;
        }
        return true;
    };
    battle.sim.ecl_effect_create = [this](i32 file, i32 script, const Vec3& position) {
        if (!presentation->create_ecl_effect(file, script, position)) {
            battle.sim.error_ = presentation->error();
            return false;
        }
        return true;
    };
    battle.sim.enemy_animation = [this](StageEnemy& enemy, i32 slot) -> AnmVm* {
        if (!presentation->flush_ecl_animations(battle, &enemy, true)) {
            battle.sim.error_ = presentation->error();
            return nullptr;
        }
        auto* vm = animations.registry.find(enemy.anm_slot_handle[slot]);
        if (!vm) enemy.anm_slot_handle[slot] = 0;
        return vm;
    };
    battle.sim.enemy_animation_interrupt = [this](StageEnemy& enemy, i32 slot, i32 label) {
        if (!presentation->flush_ecl_animations(battle, &enemy, true)) {
            battle.sim.error_ = presentation->error();
            return false;
        }
        if (!animations.interrupt(enemy.anm_slot_handle[slot], label, false)) {
            battle.sim.error_ = animations.error;
            return false;
        }
        return true;
    };
    laser_update.owner = this;
    laser_update.enabled = true;
    laser_update.run = [](void* owner) -> i32 { return static_cast<RunScene*>(owner)->update_lasers() ? 1 : 5; };
    if (scheduler.add(laser_update, FramePass::Update, 22) < 0) fail("Laser update registration failed");
    bullet_update.owner = this;
    bullet_update.enabled = true;
    bullet_update.run = [](void* owner) -> i32 { return static_cast<RunScene*>(owner)->update_bullets() ? 1 : 5; };
    if (scheduler.add(bullet_update, FramePass::Update, 23) < 0) fail("Bullet update registration failed");
    item_update.owner = this;
    item_update.enabled = true;
    item_update.run = [](void* owner) -> i32 { return static_cast<RunScene*>(owner)->update_items() ? 1 : 5; };
    if (scheduler.add(item_update, FramePass::Update, 24) < 0) fail("Item update registration failed");
    spirit_update.owner = this;
    spirit_update.enabled = true;
    spirit_update.run = [](void* owner) -> i32 { return static_cast<RunScene*>(owner)->update_spirits() ? 1 : 5; };
    if (scheduler.add(spirit_update, FramePass::Update, 25) < 0) fail("Spirit update registration failed");
    boss_hud_update.owner = this;
    boss_hud_update.enabled = true;
    boss_hud_update.run = [](void* owner) -> i32 { return static_cast<RunScene*>(owner)->update_boss_hud() ? 1 : 5; };
    if (scheduler.add(boss_hud_update, FramePass::Update, 28) < 0) fail("Boss HUD update registration failed");
    for (auto& callback : spell_callbacks) {
        callback.owner = this;
        callback.enabled = true;
    }
    spell_callbacks[0].run = [](void* owner) { return static_cast<RunScene*>(owner)->update_spell() ? 1 : 5; };
    spell_callbacks[1].run = [](void* owner) { return static_cast<RunScene*>(owner)->draw_spell() ? 1 : 5; };
    if (scheduler.add(spell_callbacks[0], FramePass::Update, 26) < 0 ||
        scheduler.add(spell_callbacks[1], FramePass::Draw, 10) < 0)
        fail("Spell callback registration failed");
    scenery_update.owner = this;
    scenery_update.enabled = true;
    scenery_update.run = [](void* owner) -> i32 { return static_cast<RunScene*>(owner)->update_scenery() ? 1 : 5; };
    if (scheduler.add(scenery_update, FramePass::Update, 13) < 0) fail("Scenery update registration failed");
    for (u32 index = 0; index < scenery_draws.size(); index++) {
        auto& draw = scenery_draws[index];
        draw.owner = this;
        draw.pass = index;
        draw.callback.owner = &draw;
        draw.callback.enabled = true;
        draw.callback.run = [](void* p) -> i32 {
            auto* draw = static_cast<StageDraw*>(p);
            return draw->owner->draw_scenery(draw->pass) ? i32(FrameAction::Continue) : i32(FrameAction::Error);
        };
        if (scheduler.add(draw.callback, FramePass::Draw, kSceneryDrawPriorities[index]) < 0)
            fail("Scenery draw callback registration failed");
    }
}

RunScene::~RunScene() {
    battle.sim.time_rate_changed = {};
    battle.sim.screen_shake = {};
    battle.bomb_screen_shake = {};
    battle.reimu_explosion_shake = {};
    battle.sim.stage_fog = {};
    battle.sim.stage_script_interrupt = {};
    battle.sim.stage_logo = {};
    for (auto& shake : shakes)
        scheduler.remove(shake->callback);
    battle.sim.enemy_fog = {};
    battle.sim.enemy_sound = {};
    battle.sim.enemy_position_sound = {};
    battle.sim.enemy_hit_sound = {};
    battle.sim.enemy_death_effect = {};
    battle.sim.player_effect = {};
    battle.beam_hit_effect = {};
    battle.option_power_changed = {};
    battle.player_shot_birth = {};
    battle.youmu_bomb_update = {};
    battle.youmu_charge_effect = {};
    battle.reimu_bomb_orb_birth = {};
    battle.sim.bullet_animation = {};
    battle.sim.bullet_animation_initialize = {};
    battle.sim.scene_effect = {};
    battle.sim.enemy_animation = {};
    battle.sim.enemy_animation_interrupt = {};
    battle.sim.flush_anm_slots = {};
    scheduler.remove(laser_update);
    scheduler.remove(bullet_update);
    scheduler.remove(scenery_update);
    scheduler.remove(item_update);
    scheduler.remove(spirit_update);
    scheduler.remove(boss_hud_update);
    laser_presentation.reset();
    battle.sim.laser_sound = {};
    for (auto& callback : callbacks)
        scheduler.remove(callback);
    for (auto& callback : spell_callbacks)
        scheduler.remove(callback);
    if (spell_presentation) spell_presentation->release();
    for (auto& draw : stage_draws)
        scheduler.remove(draw.callback);
    scheduler.remove(score_popup_draw);
    for (auto& draw : scenery_draws)
        scheduler.remove(draw.callback);
    host.dialogue_end();
    if (msg_presentation) msg_presentation->release_all();
    if (presentation) presentation->release();
}

bool RunScene::fail(const std::string& why) {
    if (error.empty()) error = why.empty() ? "Run scene unavailable" : why;
    return false;
}

bool RunScene::initialize(const HudEntryContext& hud_context) {
    if (!hud.finish_stage_clear()) return fail(hud.error);
    if (!error.empty()) return false;
    seed_native_camera_block_if_cleared(environment);
    battle.sim.stage_logo = [this] { return hud.stage_logo(0) || fail(hud.error); };
    battle.sim.bullet_animation = [this](StageBullet& bullet, bool birth) {
        if (presentation->tick_bullet(bullet, birth)) return true;
        battle.sim.error_ = presentation->error();
        return false;
    };
    battle.sim.bullet_animation_initialize = [this](StageBullet& bullet, i32 interrupt) {
        if (presentation->initialize_bullet(bullet, interrupt)) return true;
        battle.sim.error_ = presentation->error();
        return false;
    };
    battle.item_manager.spirit_animation_alive = [this](u32 slot, u32 serial) {
        return presentation->spirit_animation_alive(slot, serial);
    };
    battle.item_manager.spirit_pickup_effect = [this](i32 script, float x, float y, float z) {
        return presentation->spirit_pickup(script, {x, y, z});
    };
    battle.item_manager.spirit_spawn_animation = [this](u32 slot, u32 serial, i32 kind, u32 flags, float x, float y,
                                                        float z) {
        return presentation->spirit_spawn_animation(slot, serial, kind, flags, x, y, z);
    };
    battle.option_power_changed = [this](StageBattle& active) {
        return presentation->sync_player_options(active, true);
    };
    battle.sim.spell_event = [this](const SpellEvent& event, const StageEnemy& enemy) {
        if (event.kind == SpellEventKind::Begin) hud.boss_hud.spell_started();
        if (event.kind == SpellEventKind::End) hud.boss_hud.spell_ended();
        if (event.kind == SpellEventKind::Begin)
            host.update_spell_time(true);
        else if (event.kind == SpellEventKind::End)
            host.update_spell_time(false);
        if (!host.record_spell_event(event)) return fail("Spell record persistence failed");
        if (!spell_presentation->consume(
                event, enemy.position, [this](i32 id) { return host.hud_sound(id); }, progress.chapter))
            return fail(spell_presentation->error);
        if (event.kind == SpellEventKind::Begin && !host.rasterize_spell_name(*spell_presentation))
            return fail("Spell name rasterization failed");
        return true;
    };
    RunSceneAssets provider;
    provider.host = &host;
    provider.sim = &battle.sim;
    provider.anim_list = &battle.sim.anim_list;
    char stage_ecl[16];
    std::snprintf(stage_ecl, sizeof stage_ecl, "st%02d.ecl", int(progress.stage));
    if (restore_after_player_prepare) {
        if (!battle.prepare_player(provider, progress, player_state, battle_sht.empty() ? nullptr : battle_sht.c_str(),
                                   true))
            return fail(battle.error());
        if (!presentation->initialize_player_effects(battle)) return fail(presentation->error());
        if (!restore_after_player_prepare()) return fail("Replay stage restore failed");
    }
    const u8 rank_mask = ecl_difficulty_mask(progress.difficulty);
    if (!rank_mask) return fail("Invalid ECL difficulty");
    if (!battle.load(provider, stage_ecl, rank_mask, host, progress, player_state, score_state, nullptr,
                     battle_sht.empty() ? nullptr : battle_sht.c_str()))
        return fail(battle.error().empty() ? "Stage battle load failed" : battle.error());
    if (!restore_after_player_prepare && !presentation->initialize_player_effects(battle))
        return fail(presentation->error());
    battle.reimu_bomb_orb_birth = [this](StageBattle& active, i32 index) {
        return presentation->reimu_bomb_orb_birth(active, index);
    };
    battle.sim.ecl_slot_anm = true;
    presentation->enemies_from_ecl = true;
    battle.sim.retire_sink = [this](u32 serial) {
        presentation->retire_enemy_fog(serial);
        for (const auto& enemy : battle.sim.enemies)
            if (enemy->serial == serial) {
                for (u32& handle : enemy->anm_slot_handle)
                    if (handle && !animations.retire(handle)) fail("Enemy retire animation: " + animations.error);
                return;
            }
    };
    std::vector<u8> bytes;
    if (!host.read_asset(scenery_std, bytes)) return fail("Stage STD missing: " + scenery_std);
    if (!scenery) scenery = std::make_unique<StageScenery>();
    if (!scenery->ready()) {
        if (!scenery->file.open(bytes.data(), u32(bytes.size())))
            return fail("Stage STD invalid: " + scenery->file.error);
        if (!host.preload_scenery_animation(scenery_bank, scenery->file.animation_name))
            return fail("Stage scenery ANM unavailable: " + scenery->file.animation_name);
        if (!scenery->initialize(animations, scenery_bank))
            return fail(scenery->error.empty() ? "Stage scenery init failed" : scenery->error);
    }
    battle.sim.stage_fog = [this](i32 duration, i32 mode, u32 color, float near_distance, float far_distance) {
        auto& state = scenery->script.state;
        state.fog.start = state.camera.fog;
        state.fog.end.set(color, near_distance, far_distance);
        state.fog.duration = duration;
        state.fog.mode = mode;
        state.fog.timer.set(0);
    };
    battle.sim.stage_script_interrupt = [this](i32 id) { return scenery->script.interrupt(id); };
    for (u32 index = 0; index < stage_draws.size(); index++) {
        auto& draw = stage_draws[index];
        draw.owner = this;
        draw.pass = index;
        draw.callback.owner = &draw;
        draw.callback.enabled = true;
        draw.callback.run = [](void* p) -> i32 {
            auto* draw = static_cast<StageDraw*>(p);
            return draw->owner->draw_stage(draw->pass) ? i32(FrameAction::Continue) : i32(FrameAction::Error);
        };
        if (scheduler.add(draw.callback, FramePass::Draw, kStageDrawPriorities[index]) < 0)
            return fail("Stage draw callback registration failed");
    }
    score_popup_draw.owner = this;
    score_popup_draw.enabled = true;
    score_popup_draw.run = [](void* p) -> i32 {
        return static_cast<RunScene*>(p)->draw_score_popups() ? i32(FrameAction::Continue) : i32(FrameAction::Error);
    };
    if (scheduler.add(score_popup_draw, FramePass::Draw, 45) < 0)
        return fail("Score popup draw callback registration failed");
    char msg_name[16];
    std::snprintf(msg_name, sizeof msg_name, "st%02d%c.msg", int(progress.stage), 'a' + (progress.character & 3));
    std::vector<u8> msg_bytes;
    if (host.read_asset(msg_name, msg_bytes, false)) {
        std::string msg_error;
        if (dialog_file.open(msg_bytes.data(), u32(msg_bytes.size()), msg_error)) {
            dialog_ready = true;
            battle.sim.dialog_host = this;
        } else {
            return fail("Stage msg invalid: " + std::string(msg_name) + ": " + msg_error);
        }
    }
    if (dialog_ready && !msg_presentation->bind_stage_scripts(int(progress.stage), battle.sim.anim_list,
                                                              [this](const std::string& name, std::vector<u8>& bytes) {
                                                                  return host.read_asset(name, bytes);
                                                              }))
        return fail("Dialog presentation bind: " + msg_presentation->error);
    hud.prepare_stage_assets();
    if (!hud.initialize(hud_context)) return fail(hud.error);
    if (!score_popups.initialize(ascii_bank)) return fail(score_popups.error);
    return true;
}

bool RunScene::update_spell() {
    if (transition_wait) return true;
    if (!error.empty()) return false;
    const bool hold = environment.paused || (progress.scene_flags & 0x70u);
    if (!hold) {
        if (!battle.sim.spell.step(battle.bomb_timer > 0, &progress.rate)) return fail(battle.sim.spell.error);
        battle.sim.spell.update_visibility(battle.player_y);
        spell_presentation->sync(battle.sim.spell.state());
        if (const auto* boss = battle.sim.boss(); boss && boss->script_id == battle.sim.boss_slot[0])
            spell_presentation->update_background(boss->position);
    }
    if (trance_active(progress)) battle.sim.spell.trance_active(battle.bomb_timer > 0);
    return true;
}

bool RunScene::update_bullets() {
    if (transition_wait) return true;
    if (!error.empty()) return false;
    if (environment.paused || (progress.scene_flags & 0x70u)) return true;
    if (battle.sim.interactive) battle.sim.advance_bullets();
    if (!battle.sim.error().empty()) return fail(battle.sim.error());
    return true;
}

bool RunScene::update_lasers() {
    if (transition_wait) return true;
    if (!error.empty()) return false;
    if (environment.paused || (progress.scene_flags & 0x70u)) return true;
    battle.sim.advance_lasers();
    if (!battle.sim.error().empty())
        return fail(laser_presentation->error.empty() ? battle.sim.error() : laser_presentation->error);
    if (!battle.sim.anm_effects.empty() && !presentation->flush_ecl_animations(battle))
        return fail(presentation->error());
    return true;
}

bool RunScene::update_items() {
    if (transition_wait) return true;
    if (!error.empty()) return false;
    if (environment.paused || (progress.scene_flags & 0x70u)) return true;
    battle.update_items_phase();
    if (!battle.error().empty()) return fail(battle.error());
    if (!presentation->sync_items_phase(battle)) return fail(presentation->error());
    return true;
}

bool RunScene::update_spirits() {
    if (transition_wait) return true;
    if (!error.empty()) return false;
    if (environment.paused || (progress.scene_flags & 0x70u)) return true;
    battle.update_spirits_phase();
    if (!battle.error().empty()) return fail(battle.error());
    if (!presentation->sync_item_spirits_phase(battle)) return fail(presentation->error());
    return true;
}

bool RunScene::draw_score_popups() {
    if (!error.empty()) return false;
    if (transition_wait) return true;
    if (!host.camera(DrawCamera::Fullscreen, false)) return fail("Score popup camera unavailable");
    scene_renderer.clip = GraphicsViewport{u32(truncate_int(float(float(environment.screen_width) - 408.f) * .5f)),
                                           u32(truncate_int(float(float(environment.screen_height) - 476.f) * .5f)),
                                           408,
                                           476,
                                           0,
                                           1};
    scene_renderer.clip_set = true;
    const bool ok = score_popups.draw(scene_renderer, battle.player_x, battle.player_y);
    scene_renderer.clip_set = false;
    if (!ok) return fail(score_popups.error);
    return true;
}

bool RunScene::draw_spell() {
    if (!error.empty()) return false;
    i32 captures = 0, attempts = 0;
    host.spell_counts(battle.sim.spell.state().id, captures, attempts);
    if (!spell_presentation->draw(host, captures, attempts)) return fail(spell_presentation->error);
    if (trance_active(progress)) battle.sim.spell.trance_active(battle.bomb_timer > 0);
    spell_presentation->sync(battle.sim.spell.state());
    return true;
}

bool RunScene::start_shake(i32 duration, i32 start, i32 end) { return start_shake(ScreenShake(duration, start, end)); }

bool RunScene::start_shake(ScreenShake effect) {
    auto shake = std::make_unique<ShakeCallback>(*this, effect);
    auto& callback = shake->callback;
    callback.owner = shake.get();
    callback.enabled = true;
    callback.run = [](void* owner) -> i32 {
        auto& shake = *static_cast<ShakeCallback*>(owner);
        auto& scene = shake.scene;
        if (scene.environment.paused || (scene.progress.scene_flags & 0x70u)) return 1;
        if (shake.effect.envelope && (scene.progress.scene_flags & 0x07u)) return 1;
        auto& random = shake.effect.envelope ? scene.battle.sim.random : scene.animations.random_source();
        return shake.effect.advance(random, scene.progress.rate, scene.environment.screen_shake_offset) ? 1 : 7;
    };
    callback.cleanup = [](void* owner) -> i32 {
        auto& shake = *static_cast<ShakeCallback*>(owner);
        shake.scene.scheduler.remove(shake.callback);
        return 0;
    };
    if (scheduler.add(callback, FramePass::Update, 16) < 0) return false;
    shakes.push_back(std::move(shake));
    return true;
}

bool RunScene::update() {
    dialogue_update_pending_ = false;
    shakes.erase(std::remove_if(shakes.begin(), shakes.end(),
                                [this](const auto& shake) { return !scheduler.contains(shake->callback); }),
                 shakes.end());
    if (!error.empty()) return false;
    if (environment.paused) return true;
    if ((progress.scene_flags & 0x4010u) == 0x4010u) return true;
    const bool clear_roll_frozen =
        (hud.flags & 0x100u) != 0 && hud.clear_age.current < 120 && (progress.scene_flags & 0x10u) == 0;
    if (!(progress.scene_flags & 4u) && !clear_roll_frozen) hud.update_score();
    if (progress.scene_flags & 0x70u) return true;
    const GameInput* effective_input = input_frame;
    if (playback_input) {
        effective_input = playback_input();
        if (!effective_input) return true;
        if (damage_probe_enabled)
            std::printf("PBI sf=%d flags=%u wait=%d held=%x pressed=%x\n", progress.stage_frame, progress.scene_flags,
                        transition_wait ? 1 : 0, effective_input->held, effective_input->pressed);
    }
    if (player_effect_activation_pending) {
        if (!presentation->activate_player_effects(battle)) return fail(presentation->error());
        player_effect_activation_pending = false;
    }
    static const GameInput no_input{};
    const GameInput& selected_input = effective_input ? *effective_input : no_input;
    if (record_input) record_input(selected_input);
    if (transition_wait) {
        if (!battle.step_transition(selected_input)) return fail(battle.error());
        if (!presentation->sync(battle, selected_input.held)) return fail(presentation->error());
        hud.tick_stage_clear();
        HudFrameContext frame;
        const Vec3 player_position{battle.player_x, battle.player_y, 0};
        frame.player_position = &player_position;
        hud.update(frame, host);
        return hud.error.empty() || fail(hud.error);
    }
    progress.stage_frame++;
    age.tick(&progress.rate);
    static const GameInput kNoInput{};
    const GameInput& input = effective_input ? *effective_input : kNoInput;
    score_popups.update(animations.rate);
    if (!battle.step(input)) return fail(battle.error());
    hud.tick_stage_clear();
    dialogue_advance_.z_pressed = (input.pressed & 0x01u) != 0;
    dialogue_advance_.held = input.held;
    dialogue_advance_.pressed = input.pressed;
    dialogue_advance_.z_frames = input.duration[0];
    dialogue_advance_.ctrl_frames = input.duration[9];
    dialogue_update_pending_ = true;

    if (!presentation->sync(battle, input.held)) return fail(presentation->error());
    HudFrameContext frame;
    frame.dialogue = dialog.active();
    const Vec3 player_position{battle.player_x, battle.player_y, 0};
    frame.player_position = &player_position;
    hud.update(frame, host);
    post_flow.update();
    if (!post_flow.error.empty()) return fail(post_flow.error);
    return hud.error.empty() || fail(hud.error);
}

bool RunScene::update_boss_hud() {
    if (!error.empty()) return false;
    if (environment.paused || (progress.scene_flags & 0x4010u) == 0x4010u || (progress.scene_flags & 0x70u))
        return true;
    BossFrameInput boss_view;
    boss_view.boss = battle.sim.boss();
    boss_view.boss_slot_present = battle.sim.boss_slot[0] >= 0;
    boss_view.dialog_active = dialog.active();
    boss_view.player_x = battle.player_x;
    boss_view.player_y = battle.player_y;
    boss_view.boss_life = boss_view.boss ? i32(boss_view.boss->life) : 0;
    boss_view.trance_mode = battle.sim.spell.state().active();
    boss_view.boss_leaving = battle.sim.boss_leaving;
    boss_view.life_initial = boss_view.boss ? boss_view.boss->spell_base_life : 0;
    boss_view.timer_seconds = battle.sim.boss_timer_seconds;
    boss_view.timer_centiseconds = battle.sim.boss_timer_centiseconds;
    boss_view.remaining_stars = battle.sim.boss_remaining_stars;
    constexpr i32 name_rows[7][2] = {{-1, 0}, {-1, 1}, {2, 3}, {-1, 4}, {5, 6}, {-1, 7}, {8, 9}};
    if (progress.stage >= 1 && progress.stage <= 7) {
        const i32 name = name_rows[progress.stage - 1][progress.chapter >= 5 ? 1 : 0];
        boss_view.name_script = name >= 0 ? 155 + name : -1;
    }
    for (size_t i = 0; i < 4; ++i) {
        boss_view.markers[i].ratio = battle.sim.boss_markers[0][i].ratio;
        boss_view.markers[i].frame = battle.sim.boss_markers[0][i].color;
    }
    if (!boss_view.boss) battle.sim.boss_markers[0] = {};
    if (!hud.update_boss(boss_view, host)) return fail(hud.error.empty() ? "Boss HUD update failed" : hud.error);
    if (!dialogue_update_pending_) return true;
    dialogue_update_pending_ = false;
    if (dialog.active()) dialog.step(dialogue_advance_);
    if (dialog.active()) {
        const auto& state = dialog.state();
        const u32 rows = u32(std::min<size_t>(state.lines_meta.size(), 2));
        for (u32 row = 0; row < rows; row++) {
            const MsgLine& line = state.lines_meta[row];
            if (!host.dialogue_line(i32(row), line, state.box_x, state.box_y))
                return fail("Dialogue line " + std::to_string(row) + " submission failed");
        }
        if (!rows && dialogue_rows_ != 0) host.dialogue_clear();
        dialogue_rows_ = i32(rows);
    } else if (dialogue_rows_ != 0) {
        host.dialogue_clear();
        dialogue_rows_ = 0;
    }
    return error.empty();
}

void RunScene::open_dialog(i32 entry) {
    if (run_dialog_observer) run_dialog_observer(entry);
    if (entry == -2) {
        if (battle.sim.spell.state().flags & kSpellTimedOut) {
            if (!host.request_game_over()) fail("Spell Practice game-over request failed");
        } else {
            msg_stage_clear();
        }
        return;
    }
    if (entry == -1 || entry == -3) {
        if (!host.queue_music(2, entry == -1 ? 1 : 0, "")) fail("ECL music queue failed");
        return;
    }
    if (!dialog_ready) return;
    host.dialogue_end();
    dialogue_rows_ = 0;
    if (msg_presentation) msg_presentation->release_all();
    std::string dialog_error;
    if (!dialog.open(dialog_file, u32(entry), *this, dialog_error)) {
        fail("Dialog entry " + std::to_string(entry) + ": " + dialog_error);
        return;
    }
    if (!host.dialogue_begin()) {
        fail("Dialog text begin failed");
        return;
    }
    battle.sim.clear_bullets();
    battle.sim.cancel_lasers(false);
    if (!battle.sim.clear_enemies())
        fail(battle.sim.error());
    else if (!battle.end_trance())
        fail(battle.error());
}

void RunScene::msg_bgm(i32 opcode) {
    if (!msg_presentation) return;
    switch (opcode) {
    case 2:
        if (!msg_presentation->side_portrait(dialog.state().entry))
            fail("Dialog side portrait: " + msg_presentation->error);
        break;
    case 19:
        if (!host.queue_music(2, 1, ""))
            fail("Boss music queue failed");
        else if (!hud.stage_music_logo())
            fail(hud.error);
        break;
    case 20:
        if (!msg_presentation->nameplate(progress.chapter)) fail("Dialog nameplate: " + msg_presentation->error);
        break;
    case 31:
        if (!msg_presentation->boss_side_portrait()) fail("Dialog boss portrait: " + msg_presentation->error);
        break;
    default:
        break;
    }
}

void RunScene::msg_portrait_load() {
    if (msg_presentation && !msg_presentation->player_portrait(progress.character & 3))
        fail("Dialog player portrait: " + msg_presentation->error);
}

void RunScene::msg_text_anchor_y(float value) {
    if (!host.dialogue_anchor_y(value)) fail("Dialogue text anchor update failed");
}

void RunScene::msg_stage_clear() {
    if (!host.record_stage_clear()) {
        fail("Stage clear record persistence failed");
        return;
    }
    if (!host.record_stage_passed(progress.character, progress.difficulty, progress.stage)) {
        fail("Stage pass record persistence failed");
        return;
    }
    if ((progress.run_mode_flags & 0x30u) == 0x20u && progress.practice_mode == 0 &&
        !host.record_spell_practice_score(score_state.score)) {
        fail("Spell Practice score persistence failed");
        return;
    }
    if ((progress.run_mode_flags & 0x30u) != 0x20u && !hud.begin_stage_clear()) {
        fail(hud.error);
        return;
    }
    if (!presentation->hide_player_options()) {
        fail(presentation->error());
        return;
    }
    post_flow.on_stage_clear();
    if (!post_flow.error.empty()) fail(post_flow.error);
}

void RunScene::msg_portrait_interrupt(i32 opcode, i32 label) {
    if (!msg_presentation) return;
    const bool ok =
        opcode == 0x0d ? msg_presentation->interrupt_player(label) : msg_presentation->interrupt_side(label);
    if (!ok) fail("Dialog portrait interrupt: " + msg_presentation->error);
}

void RunScene::msg_speaker_switch(i32 opcode) {
    if (!msg_presentation) return;
    const bool ok = opcode == 0x08 ? msg_presentation->interrupt_player(3) && msg_presentation->interrupt_side(2)
                                   : msg_presentation->interrupt_side(3) && msg_presentation->interrupt_player(2);
    if (!ok) fail("Dialog speaker switch: " + msg_presentation->error);
}

void RunScene::msg_release(i32 opcode) {
    if (!msg_presentation) return;
    switch (opcode) {
    case 0x04:
        msg_presentation->release_player();
        break;
    case 0x05:
        msg_presentation->release_side();
        break;
    case 0x17:
        if (!msg_presentation->fade_player(7)) fail("Dialog portrait fade: " + msg_presentation->error);
        break;
    case 0x18:
        if (!msg_presentation->fade_side(7)) fail("Dialog portrait fade: " + msg_presentation->error);
        break;
    case 0x06:
        msg_presentation->release_nameplates();
        if (!host.dialogue_fade(1)) fail("Dialog text fade (opcode 6) failed");
        break;
    case 0x12:
        msg_presentation->release_nameplates();
        if (!host.dialogue_fade(3)) fail("Dialog text fade (opcode 18) failed");
        break;
    default:
        break;
    }
    if (!msg_presentation->error.empty()) fail("Dialog release: " + msg_presentation->error);
}

void RunScene::msg_music_fade(i32 opcode, float seconds) {
    const i32 duration = opcode == 22 ? (progress.stage == 6 ? 2 : 8) : truncate_int(seconds);
    if (!host.queue_music(5, duration, "")) fail("Dialog music fade queue failed");
}

void RunScene::msg_advance_blip() {
    if (!host.hud_sound(0)) fail("Dialog advance sound failed");
}

void RunScene::msg_dialog_end() {
    host.dialogue_end();
    dialogue_rows_ = 0;
    if (msg_presentation) {
        msg_presentation->release_all();
        if (!msg_presentation->error.empty()) fail("Dialog end release: " + msg_presentation->error);
    }
}

bool RunScene::draw_stage(u32 pass) {
    if (transition_wait && pass >= 2) return true;
    if (!error.empty()) return false;
    if (!host.camera_if_changed(DrawCamera::Fullscreen, false)) return fail("Stage camera unavailable");
    const GraphicsViewport clip{u32(truncate_int(float(float(environment.screen_width) - 408.f) * .5f)),
                                u32(truncate_int(float(float(environment.screen_height) - 476.f) * .5f)),
                                408,
                                476,
                                0,
                                1};
    scene_renderer.clip = clip;
    scene_renderer.clip_set = true;
    const bool ok = pass == 4 ? laser_presentation->draw() : presentation->draw(pass);
    scene_renderer.clip_set = false;
    if (!ok) return fail(pass == 4 ? laser_presentation->error : presentation->error());
    return true;
}

bool RunScene::prepare_transition(i32 bank, const std::string& name) {
    transition_wait = true;
    scenery_started = false;
    for (auto& enemy : battle.sim.enemies)
        if (battle.sim.retire_sink) battle.sim.retire_sink(enemy->serial);
    battle.sim.enemies.clear();
    battle.sim.bullets.clear();
    battle.item_manager.reset_items();
    for (auto& spirit : battle.item_manager.spirits())
        spirit = Spirit{};
    presentation->release_stage();
    previous_scenery = std::move(scenery);
    previous_scenery_bank = scenery_bank;
    scenery_bank = bank;
    scenery_std = name;
    scenery = std::make_unique<StageScenery>();
    std::vector<u8> bytes;
    if (!host.read_asset(name, bytes) || !scenery->file.open(bytes.data(), u32(bytes.size())))
        return fail("Next stage STD load failed");
    if (!host.preload_scenery_animation(bank, scenery->file.animation_name) || !scenery->initialize(animations, bank))
        return fail("Next stage scenery load failed");
    return true;
}

bool RunScene::start_transition() {
    if (progress.high_score < score_state.score) progress.high_score = score_state.score;
    if (!hud.finish_stage_clear()) return fail(hud.error);
    if (previous_scenery) previous_scenery->begin_exit();
    if (scenery) scenery->begin_enter();
    scenery_started = true;
    return host.open_game_fade(false, 30, true) || fail("Stage fade-out unavailable");
}

void RunScene::preserve_presentation_from(RunScene& old) {
    presentation = std::move(old.presentation);
    if (presentation) presentation->install_main_update_hook();
    scenery = std::move(old.scenery);
    previous_scenery = std::move(old.previous_scenery);
    previous_scenery_bank = old.previous_scenery_bank;
    scenery_bank = old.scenery_bank;
}

bool RunScene::update_scenery() {
    if (environment.paused || (progress.scene_flags & 0x70u)) return true;
    if (previous_scenery && previous_scenery->retired()) {
        previous_scenery.reset();
        animations.unload(previous_scenery_bank);
        previous_scenery_bank = -1;
    }
    if (scenery_started && scenery && !scenery->update(animations, environment, progress.rate))
        return fail(scenery->error);
    if (previous_scenery && !previous_scenery->update(animations, environment, progress.rate))
        return fail(previous_scenery->error);
    return true;
}

bool RunScene::draw_scenery(u32 pass) {
    if (!error.empty()) return false;
    auto& graphics = host.stage_graphics();
    auto& renderer = scene_renderer;
    const GraphicsViewport viewport{116, 2, 408, 476, 0, 1};
    for (auto* scene : {scenery_started ? scenery.get() : nullptr, previous_scenery.get()}) {
        if (!scene || !scene->ready() || scene->retired()) continue;
        if (pass == 0 && scene->draw_camera_enabled()) {
            renderer.invalidate();
            graphics.pipeline().fogMode = touhou::graphics::FogMode::Linear;
            graphics.set_depth_test(true);
            graphics.set_depth_compare(touhou::graphics::Compare::LessEqual);
            graphics.set_depth_mask(true);
            if (!graphics.clear_target(scene->draw_clear_color(), &viewport)) return fail("Scenery clear failed");
        }
        if (pass == 0) scene->finish_draw_pass(2);
        const bool saved_tint_enabled = renderer.tint_enabled;
        const u32 saved_tint = renderer.tint;
        if (pass == 0) {
            renderer.tint = scene->take_draw_tint();
            renderer.tint_enabled = (renderer.tint >> 24) != 0;
        }
        if (!scene->draw(animations, renderer, graphics, pass == 0 ? 0 : 8, pass == 0 ? 7 : 11, viewport,
                         {float(viewport.width), float(viewport.height)}, environment.screen_shake_offset))
            return fail(scene->error);
        renderer.flush();
        renderer.tint_enabled = saved_tint_enabled;
        renderer.tint = saved_tint;
        if (pass == 1) scene->finish_draw_pass(5);
        if (scene->fade_in_requested()) {
            scene->clear_fade_in_request();
            if (!host.open_game_fade(true, 30, true)) return fail("Stage fade-in unavailable");
        }
    }
    if (pass == 1) {
        renderer.invalidate();
        graphics.set_fog(false);
        graphics.set_depth_compare(touhou::graphics::Compare::Always);
        graphics.set_depth_mask(false);
        if (!host.camera(DrawCamera::Fullscreen, false)) return fail("Scenery camera restoration failed");
    }
    return true;
}

bool RunScene::draw() {
    if (!error.empty()) return false;
    if (!spell_presentation->draw_capture_times(host, host.spell_elapsed_time()))
        return fail(spell_presentation->error);
    hud.draw(HudDrawContext{}, host);
    return hud.error.empty() || fail(hud.error);
}

}
