#pragma once

#include "AnmDrawSchedule.hpp"
#include "FrameScheduler.hpp"
#include "GameHud.hpp"
#include "HudDrawData.hpp"
#include "HudFrameData.hpp"
#include "LaserPresentation.hpp"
#include "MsgPlayer.hpp"
#include "MsgPresentation.hpp"
#include "PostGameFlow.hpp"
#include "ScorePopups.hpp"
#include "ScreenShake.hpp"
#include "ScreenViews.hpp"
#include "SessionState.hpp"
#include "SpellPresentation.hpp"
#include "StageBattle.hpp"
#include "StagePresentation.hpp"
#include "StageScenery.hpp"

namespace th13 {
using RunDialogObserver = void (*)(i32 entry);
extern RunDialogObserver run_dialog_observer;

inline constexpr i32 kRunTextPriority = 46;

struct RunSceneServices : HudDrawServices, HudFrameServices, StageBattleServices, AnmDrawServices {
    bool hud_sound(i32 id) override = 0;
    bool hud_sound(i32 id, float x) override {
        (void)x;
        return hud_sound(id);
    }
    virtual bool request_pause() = 0;
    virtual bool request_practice_end() { return false; }
    virtual bool record_spell_event(const SpellEvent&) { return true; }
    virtual void update_spell_time(bool) {}
    virtual i32 spell_elapsed_time() const { return 0; }
    virtual bool record_spell_practice_score(i32) { return true; }
    virtual bool record_stage_clear() { return true; }
    virtual bool record_stage_passed(i32, i32, i32) { return true; }
    virtual bool request_stage6_fade() { return false; }
    virtual bool request_extra_results() { return false; }
    virtual bool open_game_fade(bool, i32, bool) { return false; }
    virtual bool dialogue_anchor_y(float) { return true; }
    virtual bool rasterize_spell_name(SpellPresentation&) { return false; }
    virtual void spell_counts(i32, i32& captures, i32& attempts) const { captures = attempts = 0; }
    virtual bool request_destination(i32 raw) {
        (void)raw;
        return false;
    }
    virtual bool read_asset(const std::string& name, std::vector<u8>& bytes, bool required = true) = 0;
    virtual ZunGraphics& stage_graphics() = 0;
    virtual bool preload_scenery_animation(i32 bank, const std::string& name) = 0;
    virtual bool queue_music(i32 code, i32 value, const std::string& name) {
        (void)code;
        (void)value;
        (void)name;
        return false;
    }
    virtual bool dialogue_begin() { return true; }
    virtual bool dialogue_line(i32 row, const MsgLine& line, float box_x, float box_y) {
        (void)row;
        (void)line;
        (void)box_x;
        (void)box_y;
        return true;
    }
    virtual bool dialogue_fade(i32 label) {
        (void)label;
        return true;
    }
    virtual void dialogue_clear() {}
    virtual void dialogue_end() {}
};

class RunSceneAssets final : public EclResourceProvider {
public:
    RunSceneServices* host = nullptr;
    StageSim* sim = nullptr;
    std::vector<std::string>* anim_list = nullptr;
    bool read(const std::string& name, std::vector<u8>& data) override { return host && host->read_asset(name, data); }
    bool animation(u32 slot, const std::string& name) override {
        std::vector<u8> probe;
        if (!host || !host->read_asset(name, probe)) {
            if (sim) sim->error_ = "missing ANM dependency: " + name;
            return false;
        }
        (void)slot;
        if (anim_list) anim_list->push_back(name);
        return true;
    }
};

class RunScene final : private StageDialogHost, private MsgHost, private PostGamePlatform {
    SessionState& progress;
    AnmManager& animations;
    AnmRenderer& scene_renderer;
    AnmEnvironment& environment;
    RunSceneServices& host;
    FrameScheduler& scheduler;
    HudPlayerState& player_state;
    HudScoreState& score_state;
    std::array<FrameCallback, 3> callbacks;
    std::array<FrameCallback, 2> spell_callbacks{};
    FrameCallback laser_update{};
    FrameCallback bullet_update{};
    FrameCallback item_update{};
    FrameCallback spirit_update{};
    FrameCallback boss_hud_update{};
    struct ShakeCallback {
        RunScene& scene;
        ScreenShake effect;
        FrameCallback callback;
        ShakeCallback(RunScene& owner, i32 duration, i32 start, i32 end) : scene(owner), effect(duration, start, end) {}
        ShakeCallback(RunScene& owner, ScreenShake value) : scene(owner), effect(value) {}
    };
    std::vector<std::unique_ptr<ShakeCallback>> shakes;
    bool start_shake(i32 duration, i32 start, i32 end);
    bool start_shake(ScreenShake effect);
    std::unique_ptr<LaserPresentation> laser_presentation;
    bool update_lasers();
    bool update_bullets();
    bool update_items();
    bool update_spirits();
    bool update_boss_hud();
    std::unique_ptr<SpellPresentation> spell_presentation;
    bool update_spell();
    bool draw_spell();
    bool draw_score_popups();
    struct StageDraw {
        FrameCallback callback;
        RunScene* owner = nullptr;
        u32 pass = 0;
    };
    std::array<StageDraw, 5> stage_draws;
    i32 ascii_bank = -1;
    ScorePopups score_popups{animations, environment};
    FrameCallback score_popup_draw{};
    std::array<StageDraw, 2> scenery_draws;
    i32 scenery_bank = -1;
    std::string scenery_std;
    FrameCallback scenery_update{};
    bool update_scenery();
    const GameInput* input_frame = nullptr;
    bool player_effect_activation_pending = true;
    MsgFile dialog_file;
    MsgPlayer dialog;
    bool dialog_ready = false;
    MsgAdvance dialogue_advance_{};
    bool dialogue_update_pending_ = false;
    i32 dialogue_rows_ = 0;
    u32 team_wipe_frames_ = 0;
    bool team_wipe_game_over_ = false;
    std::unique_ptr<MsgPresentation> msg_presentation;
    PostGameFlow post_flow;
    std::unique_ptr<GameHud> owned_hud;
    bool fail(const std::string&);

public:
    static constexpr i32 kPlayerShotSprite = 31;
    GameHud& hud;
    MpHudSeats mp_hud_seats_;
    void sync_mp_hud_view();
    HudScoreState& score_view() const { return score_state; }
    StageBattle battle;
    std::unique_ptr<StagePresentation> presentation;
    std::unique_ptr<StageScenery> scenery;
    std::unique_ptr<StageScenery> previous_scenery;
    i32 previous_scenery_bank = -1;
    bool transition_wait = false, scenery_started = true;
    bool prepare_transition(i32 bank, const std::string& name);
    bool start_transition();
    void preserve_presentation_from(RunScene& old);
    std::function<void(const GameInput&)> record_input;
    std::string battle_sht;
    u32 input_held = 0, input_pressed = 0, input_repeated = 0;
    Timer age{0, 0, 0, 0, 0};
    std::string error;
    RunScene(SessionState&, HudPlayerState&, HudScoreState&, AnmManager&, AnmRenderer&, AnmEnvironment&,
             RunSceneServices&, FrameScheduler&, StagePresentation::Banks banks, i32 front_bank, i32 ascii_bank,
             i32 scenery_bank, const std::string& std_name, GameRandom* shared_random = nullptr,
             GameHud* shared_hud = nullptr, StagePlayerState* shared_player = nullptr);
    ~RunScene();
    RunScene(const RunScene&) = delete;
    RunScene& operator=(const RunScene&) = delete;
    void controls(const GameInput& input) { input_frame = &input; }
    std::function<const GameInput*()> playback_input;
    std::function<bool()> restore_after_player_prepare;
    bool initialize(const HudEntryContext& = {});
    bool update();
    bool enable_multiplayer(i32 seat_count, const std::array<i32, 3>& characters) {
        if (error.empty() && !battle.error().empty()) error = battle.error();
        RunSceneAssets provider;
        provider.host = &host;
        provider.sim = &battle.sim;
        provider.anim_list = &battle.sim.anim_list;
        if (!battle.enable_seats(provider, seat_count, characters)) {
            if (error.empty()) error = battle.error();
            return false;
        }
        if (!hud.enable_multiplayer(seat_count)) {
            if (error.empty()) error = hud.error;
            return false;
        }
        sync_mp_hud_view();
        return true;
    }
    bool set_dialogue_visible(bool visible) { return !msg_presentation || msg_presentation->set_visible(visible); }
    bool spawn_score_popup(i32 value, float x, float y, u32 color) { return score_popups.spawn(value, x, y, color); }
    bool show_notice(i32 front_script) { return !spell_presentation || spell_presentation->show_notice(front_script); }
    bool show_spirit_notice(i32 level, float x, float y, float z) {
        return !spell_presentation || spell_presentation->show_spirit_notice(level, x, y, z);
    }
    bool inject_shake(ScreenShake effect) { return start_shake(effect); }
    u32 dialog_clock() const { return dialog.clock(); }
    struct RollbackState {
        PostGameFlow::RollbackState post;
        GameHud::RollbackState hud;
        SpellPresentation::RollbackState spell;
        MsgPresentation::RollbackState portraits;
        ScorePopups::RollbackState popups;
        LaserPresentation::SnapshotData lasers;
        Vec3 camera_origin, screen_translation, reference_position, background_delta, stage_direction;
        Vec2 shake_offset;
        GameRandom environment_rng{0};
        float animation_rate;
        bool paused;
        i32 dialogue_rows;
        Timer age;
        bool player_activation_pending;
        bool transition_wait, scenery_started;
        u32 team_wipe_frames;
        bool team_wipe_game_over;
    };
    RollbackState capture_rollback() const {
        return {post_flow.capture_rollback(),
                hud.capture_rollback(),
                spell_presentation ? spell_presentation->capture_rollback() : SpellPresentation::RollbackState{},
                msg_presentation ? msg_presentation->capture_rollback() : MsgPresentation::RollbackState{},
                score_popups.capture_rollback(),
                laser_presentation ? laser_presentation->capture() : LaserPresentation::SnapshotData{},
                environment.camera_origin,
                environment.screen_translation,
                environment.reference_position,
                environment.background_delta,
                environment.stage_direction,
                environment.screen_shake_offset,
                environment.game_rng,
                animations.rate,
                environment.paused,
                dialogue_rows_,
                age,
                player_effect_activation_pending,
                transition_wait,
                scenery_started,
                team_wipe_frames_,
                team_wipe_game_over_};
    }
    void restore_rollback(const RollbackState& state, std::unordered_map<const AnmVm*, AnmVm*>& remapped) {
        post_flow.restore_rollback(state.post);
        hud.restore_rollback(state.hud);
        if (spell_presentation) spell_presentation->restore_rollback(state.spell);
        if (msg_presentation) msg_presentation->restore_rollback(state.portraits);
        score_popups.restore_rollback(state.popups);
        if (laser_presentation) laser_presentation->restore(state.lasers, remapped);
        environment.camera_origin = state.camera_origin;
        environment.screen_translation = state.screen_translation;
        environment.reference_position = state.reference_position;
        environment.background_delta = state.background_delta;
        environment.stage_direction = state.stage_direction;
        environment.screen_shake_offset = state.shake_offset;
        environment.game_rng = state.environment_rng;
        animations.rate = state.animation_rate;
        environment.paused = state.paused;
        dialogue_rows_ = state.dialogue_rows;
        age = state.age;
        player_effect_activation_pending = state.player_activation_pending;
        transition_wait = state.transition_wait;
        scenery_started = state.scenery_started;
        team_wipe_frames_ = state.team_wipe_frames;
        team_wipe_game_over_ = state.team_wipe_game_over;
    }
    bool draw();
    bool draw_stage(u32 pass);
    bool draw_scenery(u32 pass);
    void capture_shakes(std::vector<ScreenShake>& out) const {
        out.clear();
        out.reserve(shakes.size());
        for (const auto& shake : shakes)
            out.push_back(shake->effect);
    }
    bool restore_shakes(const std::vector<ScreenShake>& source) {
        for (auto& shake : shakes)
            scheduler.remove(shake->callback);
        shakes.clear();
        for (const ScreenShake& effect : source)
            if (!start_shake(effect)) return false;
        return true;
    }
    struct DialogSnapshot {
        MsgPlayer::Snapshot player;
        MsgAdvance advance{};
        bool ready = false;
    };
    DialogSnapshot capture_dialog() const {
        DialogSnapshot out;
        out.player = dialog.capture();
        out.advance = dialogue_advance_;
        out.ready = dialog_ready;
        return out;
    }
    void restore_dialog(const DialogSnapshot& source) {
        dialog.restore(source.player);
        dialogue_advance_ = source.advance;
        dialog_ready = source.ready;
    }
    bool capture_scenery(StageScenery::ExtraSnapshot& extra, StageScriptState& script_state, bool& has) const {
        has = scenery != nullptr;
        if (!has) return true;
        extra = scenery->capture_extra();
        script_state = scenery->script.state;
        return true;
    }
    bool restore_scenery(const StageScenery::ExtraSnapshot& extra, const StageScriptState& script_state, bool has) {
        if (!has) return true;
        if (!scenery) return false;
        scenery->restore_extra(extra);
        scenery->script.state = script_state;
        return true;
    }
    void request_pause() { host.request_pause(); }
    void request_game_over() { host.request_game_over(); }
    void open_dialog(i32 entry) override;
    bool dialog_active() const override { return dialog.active(); }
    i32 dialog_entry() const { return dialog.state().entry; }
    bool dialog_wait_released() const override { return dialog.ecl_wait_released(); }

    enum class SceneEffectAction { None, Effect0, Effect136 };
    static constexpr SceneEffectAction scene_effect_action(i32 kind) noexcept {
        return kind == 6                                                         ? SceneEffectAction::Effect136
               : (kind == 0 || kind == 1 || kind == 2 || kind == 3 || kind == 5) ? SceneEffectAction::Effect0
                                                                                 : SceneEffectAction::None;
    }

private:
    void msg_bgm(i32 opcode) override;
    void msg_portrait_load() override;
    void msg_text_anchor_y(float value) override;
    void msg_stage_clear() override;
    void msg_portrait_interrupt(i32 opcode, i32 label) override;
    void msg_speaker_switch(i32 opcode) override;
    void msg_release(i32 opcode) override;
    void msg_music_fade(i32 opcode, float seconds) override;
    void msg_advance_blip() override;
    void msg_dialog_end() override;
    void request_destination(i32 raw) override {
        if (!host.request_destination(raw)) fail("Post-game destination request failed: " + std::to_string(raw));
    }
    bool request_practice_end() override {
        if (!host.request_practice_end()) {
            fail("Practice end request failed");
            return false;
        }
        return true;
    }
    bool request_stage6_fade() override { return host.request_stage6_fade(); }
    bool request_extra_results() override { return host.request_extra_results(); }
    void bgm(i32 id) override { msg_bgm(id); }
};

}
