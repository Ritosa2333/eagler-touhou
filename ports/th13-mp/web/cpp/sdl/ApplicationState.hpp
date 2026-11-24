#pragma once

#include "../game/SpellCatalog.hpp"
#include "../game/SpellElapsedTime.hpp"
#include "../game/TitlePlayerDataMenu.hpp"
#include "../game/TitleSpellPracticeMenu.hpp"
#include "../multiplayer/HostState.hpp"

namespace th13::sdl {

struct TitlePlayerDataHost : TitlePlayerDataPlatform {
    std::function<i32(i32)> count_records;
    std::function<bool(i32, i32, i32, TitlePlayerDataRecord&)> read_record;
    std::function<bool(i32, i32, TitlePlayerDataSummary&)> read_summary;
    bool open_player_data() override { return true; }
    void close_player_data() override {}
    i32 difficulty_count() const override { return kPlayerDataDifficulties; }
    i32 character_count() const override { return kPlayerDataCharacters; }
    i32 record_count(i32 difficulty) const override {
        if (difficulty < 0 || difficulty >= kPlayerDataDifficulties) return 0;
        return count_records ? count_records(difficulty) : 0;
    }
    bool record(i32 difficulty, i32 character, i32 ordinal, TitlePlayerDataRecord& out) const override {
        return read_record && read_record(difficulty, character, ordinal, out);
    }
    bool summary(i32 difficulty, i32 character, TitlePlayerDataSummary& out) const override {
        return read_summary && read_summary(difficulty, character, out);
    }
};

struct TitleSpellPracticeHost : TitleSpellPracticePlatform {
    const SpellCatalog* catalog = nullptr;
    SpellCatalog::Check encountered;
    SpellCatalog::Check captured;
    SpellCatalog::Check named;
    i32 stage_count() const override { return kSpellPracticeStages; }
    bool stage_available(i32 stage) const override { return stage >= 1 && stage <= kSpellPracticeStages; }
    std::string stage_name(i32 stage) const override {
        static constexpr const char* kNames[kSpellPracticeStages + 1] = {nullptr,   "Stage 1", "Stage 2", "Stage 3",
                                                                         "Stage 4", "Stage 5", "Stage 6", "Extra  "};
        if (stage < 1 || stage > kSpellPracticeStages) return {};
        return kNames[stage];
    }
    i32 spell_count(i32 stage) const override {
        if (stage < 1 || stage > kSpellPracticeStages) return 0;
        return kSpellPracticeSlots[stage - 1];
    }
    bool spell_unlocked(i32 stage, i32 slot) const override {
        return catalog && catalog->spell_unlocked(stage, slot, encountered);
    }
    std::string spell_name(i32 stage, i32 slot) const override {
        return catalog ? catalog->spell_name(stage, slot, encountered) : std::string{};
    }
    i32 spell_id(i32 stage, i32 slot, i32 difficulty) const override {
        const auto* entry = catalog ? catalog->variant(stage, slot, difficulty) : nullptr;
        return entry ? entry->id : -1;
    }
    i32 spell_rank(i32 stage, i32 slot, i32 difficulty) const override {
        const auto* entry = catalog ? catalog->variant(stage, slot, difficulty) : nullptr;
        return entry ? entry->rank : -1;
    }
    bool spell_variant_available(i32 stage, i32 slot, i32 difficulty) const override {
        return catalog && catalog->variant_available(stage, slot, difficulty, encountered, captured);
    }
    std::string spell_variant_name(i32 stage, i32 slot, i32 difficulty) const override {
        const auto* entry = catalog ? catalog->variant(stage, slot, difficulty) : nullptr;
        return entry && named && named(entry->id) ? entry->name : std::string{};
    }
    i32 character_count() const override { return kSpellPracticeCharacters; }
    std::string character_name(i32 character) const override {
        static constexpr const char* kNames[kSpellPracticeCharacters] = {"Reimu  ", "Marisa ", "Sanae  ", "Youmu  "};
        if (character < 0 || character >= kSpellPracticeCharacters) return {};
        return kNames[character];
    }
};

inline bool transition_gate_player_update_runs(bool paused, u32 scene_flags) noexcept {
    return !paused && (scene_flags & 0x70u) == 0;
}

inline bool transition_gate_should_interrupt(u32 frames, bool paused, u32 scene_flags) noexcept {
    return frames >= 29 && transition_gate_player_update_runs(paused, scene_flags);
}

}

#ifndef TH13_APPLICATION_PLATFORMS_ONLY

#include "../game/DialogueText.hpp"
#include "../game/EndingScene.hpp"
#include "../game/ManualViewer.hpp"
#include "../game/PlayfieldTargets.hpp"
#include "../game/ReplayFile.hpp"
#include "../game/ReplayRecorder.hpp"
#include "../game/ReplaySession.hpp"
#include "../game/RunPause.hpp"
#include "../game/RunScene.hpp"
#include "../game/RunTransitionEffect.hpp"
#include "../game/TitleMusicRoomMenu.hpp"
#include "../game/TitleOptionsMenu.hpp"
#include "../game/TitleScene.hpp"
#include "Application.hpp"
#include "AudioDevice.hpp"
#include "FileStore.hpp"
#include "FontDevice.hpp"
#include "SceneDisplay.hpp"
#include "StageAssetStager.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace th13::sdl {

enum class Destination : i32 {
    Quit = 3,
    Title = 4,
    TitleFromGameOver = 2,
    TitleFromReplay = 14,
    Run = 13,
    NextStage = 100,
    Ending = 15,
    AfterEnding = 16
};

struct PendingReplayRecorder final {
    ReplayRecorder stream;
    u32 capture_frame = 0;
    struct PendingInput {
        u32 frame = 0;
        GameInput input{};
        u8 slow = 60;
        bool edge_reset = false;
    };
    struct PendingSpellTime {
        u32 frame = 0;
        u32 index = 0;
        i32 packed = 0;
    };
    std::vector<PendingInput> pending_inputs;
    std::vector<PendingSpellTime> pending_spell_times;
    bool start(const std::vector<u8>& top) { return stream.start(top); }
    bool begin_stage(const std::vector<u8>& header) { return stream.begin_stage(header); }
    bool capture_input(const GameInput& input, u8 slow_rate, bool edge_reset) {
        if (!stream.ready()) return false;
        pending_inputs.push_back({capture_frame, input, slow_rate, edge_reset});
        return true;
    }
    bool record_spell_time(u32 index, i32 packed) {
        if (!stream.ready() || index >= (0xc4u - 0x70u) / 4u) return false;
        pending_spell_times.push_back({capture_frame, index, packed});
        return true;
    }
    bool ready() const { return stream.ready(); }
    void clear() {
        stream.clear();
        pending_inputs.clear();
        pending_spell_times.clear();
    }
    bool encode(std::vector<u8>& out, const std::string& name, i32 score) const {
        return stream.encode(out, name, score);
    }
    bool encode(std::vector<u8>& out, const std::string& name, i32 score, const ReplaySaveMeta& meta) const {
        return stream.encode(out, name, score, meta);
    }
};

struct PendingFileEvent {
    enum class Kind : i32 {
        SpellEvent,
        SpellPracticeScore,
        StageClear,
        PauseOption,
        ResultEntry,
        MusicUnlocked,
        MarkHeard,
        PersistOptions,
    };
    Kind kind = Kind::MarkHeard;
    u32 capture_frame = 0;
    SpellEvent spell_event{};
    bool spell_practice = false;
    i32 character = 0;
    i32 difficulty = 0;
    bool no_continue = false;
    i32 spell_id = 0;
    i32 score = 0;
    i32 row = 0, delta = 0;
    TitlePlayerDataScore result{};
    i32 track = 0;
    i32 bgm = 0, effect = 0;
};

struct PendingVoicePan {
    u32 capture_frame = 0;
    i32 id = 0;
    float pan = 0;
};

struct ApplicationState final : TitleScenePlatform, RunSceneServices, RunPausePlatform, EndingPlatform {
    GraphicsDevice graphics;
    FontDevice fonts;
    std::string fonts_failure;
    AudioDevice audio_device;
    std::string game_over_music;
    u32 game_over_music_cursor = 0;
    std::function<void(const AudioDevice::RequestObservation&)> audio_debug_observer_;
    bool mp_audio_wired_ = false;
    FileStore files;
    GameRandom visual;
    u16 boot_seed = 0;
    std::array<u8, 256> visual_font_noise_table{};
    AnmEnvironment environment;
    AnmManager animations{visual, environment};
    DialogueText dialogue_text;
    AnmRenderer renderer{graphics};
    ScreenViews views{renderer, environment};
    AsciiText captions{animations, environment};
    FrameScheduler scheduler;
    AnmEnvironment loading_environment;
    AnmManager loading_animations{visual, loading_environment};
    bool platform_prepared = false;
    u32 loading_signature = 0, loading_prayer = 0;
    u32 transition_prayer = 0;
    RunTransitionEffect run_transition{animations};
    i32 transition_scenery_bank = -1;
    std::string transition_scenery_std;
    bool transition_scenery_pending = false;
    static constexpr unsigned kDebugLoadingFrames = 3;
    FrameCallback loading_battle_draw{};
    FrameCallback fps_text_draw{};
    float last_fps_ = 60;
    float fps_color_override_ = -1;
    void set_fps_color_override(float value) { fps_color_override_ = value; }
    double slow_actual_ = 0.0, slow_budget_ = 0.0;
    double slow_window_time_ = 0.0;
    u32 slow_window_frames_ = 0;
    float slow_window_fps_ = 60.0f;
    double slow_num_ = 0.0, slow_den_ = 0.0;
    void reset_slow_rate() {
        slow_actual_ = 0.0;
        slow_budget_ = 0.0;
        slow_window_time_ = 0.0;
        slow_window_frames_ = 0;
        slow_num_ = 0.0;
        slow_den_ = 0.0;
    }
    unsigned pending_load = 0, loading_frames = 0;
    bool loading_startup = false;
    double loading_debt = 0;
    SessionState progress;
    HudPlayerState selection_player;
    HudScoreState selection_score;
    TitleSelectionSettings selection;
    GameRandom game_random;
    std::unique_ptr<SceneDisplay> display;
    std::array<FrameCallback, 2> animation_updates;
    std::unique_ptr<ManualViewer> manual_viewer;
    std::unique_ptr<TitleScene> title;
    std::unique_ptr<GameHud> run_hud;
    std::unique_ptr<StagePlayerState> run_player;
    std::unique_ptr<RunScene> run;
    std::unique_ptr<RunPause> pause;
    std::unique_ptr<EndingScene> ending;
    std::array<u32, 5> ending_text_vms{};
    std::array<u32, 24> ending_anm_vms{};
    struct EndingFade {
        ApplicationState* owner = nullptr;
        i32 kind = 0, duration = 0, time = 0, alpha = 0;
        bool playfield_only = false;
        FrameCallback update, draw;
    };
    std::vector<std::unique_ptr<EndingFade>> ending_fades;
    std::unique_ptr<PlayfieldTargets> playfield_targets;
    i32 pending_destination = -1, selected_stage = 1, current_destination = 13;
    u32 frames = 0;
    ReplayFile replay_file;
    PendingReplayRecorder recorder;
    u32 pending_play_frames_ = 0;
    std::vector<PendingFileEvent> pending_file_events_;
    std::vector<PendingVoicePan> pending_voice_pans_;
    bool record_edge_reset_pending_ = false;
    std::vector<u8> replay_top_pin_;
    bool replay_top_pin_enabled_ = false;
    SpellTimeClock spell_time_clock;
    i32 spell_time_stage = 0, spell_time_packed = 0;
    std::array<std::array<i32, 21>, 8> spell_time_pin_{};
    bool spell_time_pin_enabled_ = false;
    bool stage_transition_loading = false, stage_transition_active = false;
    i32 stage_startup_age = -1;
    u32 transition_gate_frames = 0;
    bool transition_gate_active = false;
    i32 next_scenery_bank = 26;
    bool prepare_next_stage();
    bool activate_next_stage();
    bool begin_record_stage();
    std::unique_ptr<ReplayInputCursor> replay_cursor;
    bool replay_active = false;
    i32 replay_catalog_index = 0;
    bool replay_finished = false;
    bool title_flow_ran = false;
    u32 replay_consumed = 0;
    FrameCallback replay_fast_forward{}, ending_update{};
    const GameInput* physical_input = nullptr;
    u32 replay_pass_start = 0;
    bool restart_update = false;
    GameInput replay_input;
    const ReplayStage* pending_replay_stage = nullptr;
    u32 pending_replay_stage_index = 0;
    bool pending_transition_restore = false;
    bool focus_pause_enabled = true;
    bool initialized = false, exiting = false;
    std::array<i32, 16> projected{};
    std::string failure;
    std::unordered_map<std::string, std::vector<u8>> resource_cache;

    bool loading_waiting() const;
    bool complete_loading();
    bool render_loading();
    bool advance_loading(double);
    bool advance_loading_caption();
    bool draw_loading_battle();
    bool commit_loading_battle_caption();
    bool draw_fps_text();
    bool flush_animation_captures();
    bool fail(const std::string&);
    bool prepare_platform();
    bool prepare_loading();
    bool draw_loading(bool signature);
    bool initialize(bool);
    ~ApplicationState();
    bool update_scene_pass(const GameInput& input);
    bool update(const GameInput& input, bool lost_focus);
    bool commit(float presentation_fps);
    bool present();
    bool commit_pending_replay_records();
    bool commit_pending_file_events();
    void commit_pending_voice_pans();
    bool commit_pending_play_frames();
    bool commit_pending_multiplayer_audio();
    std::function<bool()> capture_multiplayer_host();
    bool capture_multiplayer_host_state(multiplayer::HostState&);
    u64 digest_multiplayer_host();
    std::function<bool()> capture_dialogue_rollback();
    u64 digest_dialogue_rollback();
    std::function<bool()> multiplayer_replay_available;
    std::function<bool(i32, const std::string&)> multiplayer_replay_save;
    void queue_multiplayer_outputs();
    void wire_multiplayer_audio();
    bool step(const GameInput& input, float fps, bool lost_focus);
    bool apply_destination();
    bool quit();
    bool begin_title(i32 reason);
    bool begin_run();
    bool begin_run_transition();
    bool stage_start_run_transition();
    bool release_run_transition();
    i32 mp_seat_count = 0;
    std::array<i32, 3> mp_characters{0, 0, 0};
    i32 mp_local_seat = 0;
    bool mp_history_playback = false;
    bool mp_challenge = false;
    bool mp_keep_bullets_on_miss = false;
    bool mp_legacy_stage_positions = false;
    bool mp_legacy_boss_mitigation = false;
    bool mp_legacy_pickup_rewards = false;
    std::array<GameInput, 3> mp_seat_inputs{};
    u32 mp_transition_handle_ = 0;
    bool configure_multiplayer(i32 count, const std::array<i32, 3>& characters) {
        return configure_multiplayer(count, characters, 0);
    }
    bool configure_multiplayer(i32 count, const std::array<i32, 3>& characters, i32 local_seat) {
        if (count != 0 && (count < 2 || count > 3)) return false;
        if (local_seat < 0 || (count && local_seat >= count)) return false;
        for (i32 i = 0; i < count; ++i)
            if (characters[size_t(i)] < 0 || characters[size_t(i)] > 3) return false;
        mp_seat_count = count;
        mp_characters = characters;
        mp_local_seat = local_seat;
        mp_history_playback = false;
        mp_challenge = false;
        mp_keep_bullets_on_miss = false;
        mp_legacy_stage_positions = false;
        mp_legacy_boss_mitigation = false;
        mp_legacy_pickup_rewards = false;
        return true;
    }
    bool begin_replay_run(const u8* data, size_t size, u32 stage_index);
    bool begin_replay_stage(u32 stage_index, bool stage_transition = false);
    u32 replay_take_input();
    void release_run(bool preserve_hud = false);
    const i32* projection();
    bool read(const std::string&, std::vector<u8>&, bool required = true);
    StageAssetStager stage_assets;
    bool adopt_staged_bank(i32 bank, const std::string& key);
    struct ReplayCatalogSlot {
        std::string name;
        std::vector<u8> bytes;
        std::shared_ptr<ReplayFile> decoded;
    };
    std::array<ReplayCatalogSlot, 75> replay_slots{};
    std::string replay_import_error_;
    i32 pending_replay_file = -1, pending_replay_stage_slot = -1;
    bool import_replay_bytes(const u8* data, size_t size, const std::string& name);
    i32 replay_count() const override;
    const ReplayFile* replay_entry(i32 index) const override;
    std::string replay_filename(i32 index) const override;
    bool request_replay_file() override;
    std::string replay_import_error() const override;
    bool start_replay(i32 file_index, i32 stage_index) override;
    bool clear_title_overlay() override;
    bool queue_title_music(i32 code, i32 value, const std::string&) override;
    bool clear_current_wave() override;
    bool title_exit(i32 destination) override;
    bool fade_out_title_music() override;
    bool sound(i32) override;
    bool begin_transition(u32&) override;
    bool start_game(i32 stage) override;
    bool start_spell_practice(const SpellPracticeCommand&) override;
    bool can_start_spell_practice(const SpellPracticeCommand&) const override;
    bool record_menu_views() const override { return true; }
    bool open_manual() override;
    bool update_manual(u32 pressed, u32 repeated) override;
    bool manual_finished() const override;
    void close_manual() override;
    bool rasterize_title_text(AnmVm&, u32, const std::string&, AnmTextAlignment = AnmTextAlignment::Left) override;
    bool request_destination(i32 raw) override;
    bool hud_text(const HudTextDraw&) override;
    bool character_cleared(i32 character, i32 difficulty) override;
    bool extra_unlocked(i32 character) override;
    bool practice_stage_played(i32 character, i32 difficulty, i32 stage) override;
    void apply_volumes(i32 bgm, i32 effect) override;
    bool persist_options(const TitleOptionVolumes& volumes) override;
    bool open_key_config() override;
    u32 raw_pad_buttons = 0;
    const std::array<i16, 5>& pad_bindings() const noexcept { return files.pad_bindings; }
    bool persist_pad_bindings(const std::array<i16, 5>& bindings) {
        files.pad_bindings = bindings;
        return files.save();
    }
    TitlePlayerDataHost player_data_host;
    TitleSpellPracticeHost spell_practice_host;
    SpellCatalog spell_catalog;
    void connect_record_menus();
    bool open_player_data() override { return player_data_host.open_player_data(); }
    void close_player_data() override { player_data_host.close_player_data(); }
    i32 difficulty_count() const override { return player_data_host.difficulty_count(); }
    i32 record_count(i32 difficulty) const override { return player_data_host.record_count(difficulty); }
    bool record(i32 difficulty, i32 character, i32 ordinal, TitlePlayerDataRecord& out) const override {
        return player_data_host.record(difficulty, character, ordinal, out);
    }
    bool summary(i32 difficulty, i32 character, TitlePlayerDataSummary& out) const override {
        return player_data_host.summary(difficulty, character, out);
    }
    i32 stage_count() const override { return spell_practice_host.stage_count(); }
    bool stage_available(i32 stage) const override { return spell_practice_host.stage_available(stage); }
    std::string stage_name(i32 stage) const override { return spell_practice_host.stage_name(stage); }
    i32 spell_count(i32 stage) const override { return spell_practice_host.spell_count(stage); }
    bool spell_unlocked(i32 stage, i32 slot) const override { return spell_practice_host.spell_unlocked(stage, slot); }
    std::string spell_name(i32 stage, i32 slot) const override { return spell_practice_host.spell_name(stage, slot); }
    i32 spell_id(i32 stage, i32 slot, i32 difficulty) const override {
        return spell_practice_host.spell_id(stage, slot, difficulty);
    }
    i32 spell_rank(i32 stage, i32 slot, i32 difficulty) const override {
        return spell_practice_host.spell_rank(stage, slot, difficulty);
    }
    bool spell_variant_available(i32 stage, i32 slot, i32 difficulty) const override {
        return spell_practice_host.spell_variant_available(stage, slot, difficulty);
    }
    std::string spell_variant_name(i32 stage, i32 slot, i32 difficulty) const override {
        return spell_practice_host.spell_variant_name(stage, slot, difficulty);
    }
    bool spell_record(i32 character, i32 id, SpellPracticeRecord& out) const override {
        const auto* own = files.spell_counters(character, id);
        const auto* all = files.spell_counters(4, id);
        if (!own || !all) return false;
        for (i32 mode = 0; mode < 2; ++mode) {
            out.captures[mode] = own->captures[mode];
            out.attempts[mode] = own->attempts[mode];
        }
        out.best_score = own->practice_best_score;
        out.encountered = all->attempts[0] > 0 || all->attempts[1] > 0;
        return true;
    }
    i32 character_count() const override { return kPlayerDataCharacters; }
    std::string character_name(i32 character) const override { return spell_practice_host.character_name(character); }
    i32 track_count() const override;
    std::string track_title(i32 track) override;
    std::string track_comment(i32 track, i32 line) override;
    bool track_heard(i32 track) const override;
    void mark_heard(i32 track) override;
    bool play_track(i32 track) override;
    bool play_locked_response() override;
    bool resume_title_music() override;
    struct MusicRoomTrack {
        std::string name;
        std::string title;
        std::array<std::string, 8> comment;
    };
    std::vector<MusicRoomTrack> music_room;
    void load_music_room_catalog();
    bool request_pause() override;
    bool request_game_over() override;
    bool request_player_game_over() override;
    bool request_practice_end() override;
    bool request_stage6_fade() override;
    bool request_extra_results() override;
    bool record_spell_event(const SpellEvent&) override;
    void update_spell_time(bool) override;
    i32 spell_elapsed_time() const override { return spell_time_packed; }
    bool record_spell_practice_score(i32 score) override;
    bool record_stage_clear() override;
    bool ordinary_records_dirty = false;
    bool rasterize_spell_name(SpellPresentation&) override;
    void spell_counts(i32, i32& captures, i32& attempts) const override;
    bool hud_sound(i32) override;
    bool hud_sound(i32, float) override;
    bool stop_sound(i32) override;
    bool set_voice_pan(i32, float) override;
    bool dialogue_begin() override;
    bool dialogue_line(i32 row, const MsgLine& line, float box_x, float box_y) override;
    bool dialogue_fade(i32 label) override;
    bool dialogue_anchor_y(float) override;
    void dialogue_clear() override;
    void dialogue_end() override;
    bool read_asset(const std::string& name, std::vector<u8>& bytes, bool required = true) override {
        return read(name, bytes, required);
    }
    bool camera(DrawCamera index, bool refresh) override { return views.camera(index, refresh); }
    bool camera_if_changed(DrawCamera index, bool refresh) override { return views.camera_if_changed(index, refresh); }
    ZunGraphics& stage_graphics() override { return graphics; }
    bool preload_scenery_animation(i32 bank, const std::string& name) override;
    bool queue_music(i32 code, i32 value, const std::string& name) override {
        return audio_device.queue_music(code, value, name) || fail(audio_device.error);
    }
    bool trance_music(bool active) override { return audio_device.trance_music(active) || fail(audio_device.error); }
    bool trance_gauge_refresh() override {
        if (!run) return true;
        return run->hud.trance_label_refresh() || fail(run->hud.error);
    }
    bool score_popup(i32 value, float x, float y, u32 color) override {
        if (!run) return true;
        return run->spawn_score_popup(value, x, y, color);
    }
    bool hud_notice(i32 front_script) override;
    bool hud_spirit_notice(i32 level, float x, float y, float z) override;
    bool suspend_music() override;
    bool resume_music() override;
    bool restart_run() override;
    bool return_to_title() override;
    bool continue_run() override;
    bool save_replay() override;
    bool can_save_replay() const override;
    bool save_replay_slot(i32, const std::string&) override;
    bool save_replay_slot(i32, const std::string&, const ReplaySaveMeta* pinned);
    bool can_open_pause_options() const override;
    bool open_pause_options() override;
    bool change_pause_option(i32, i32) override;
    i32 pause_option_value(i32) const override;
    bool record_result_name(const std::string&) override;
    bool record_clear_result_name(const std::string&) override;
    bool record_result_entry(const std::string& name, i32 stage);
    std::string last_result_name() const override;
    bool extra_results_open() const;
    bool result_needs_name() const override;
    i32 result_ranking(TitlePlayerDataScore rows[10]) const override;
    bool dialogue_visible(bool visible) override;
    bool result_replay_save_available() const override;
    bool result_ranking_available() const override;
    bool result_screen_data(TitleResultScreenData&) const override;
    EndingSaveBytes* save_bytes() override;
    bool fade_out_music(i32 frames) override;
    bool record_music_unlocked(i32 index) override;
    bool advance_sound() override;
    bool load_ending_anm(i32 slot, const std::string& name) override;
    bool cue_ending_anm(i32 vm_slot, i32 bank_slot, i32 script) override;
    void release_ending_anm(i32 slot) override;
    bool initialize_ending_text();
    bool ending_page_reset() override;
    bool ending_page_line(i32 row, u32 color, const std::string& cp932) override;
    bool ending_page_clear() override;
    void release_ending_page() override;
    bool open_ending_window(i32 kind, i32 duration) override;
    bool open_game_fade(bool entering, i32 duration, bool playfield_only) override;
    void clear_ending_windows();
    struct EndingSaveMirror final : EndingSaveBytes {
        explicit EndingSaveMirror(std::array<u8, 13>* storage) : bytes(storage) {}
        u8 read(i32 index) const override {
            return bytes && index >= 0 && index < i32(bytes->size()) ? (*bytes)[u32(index)] : u8(0);
        }
        void write(i32 index, u8 value) override {
            if (bytes && index >= 0 && index < i32(bytes->size())) (*bytes)[u32(index)] = value;
        }
        std::array<u8, 13>* bytes;
    } ending_save_view{&files.ending_seen_bytes};
    bool life_hud(i32, i32) override;
    bool bomb_hud(i32, i32) override;
    bool life_hud_seat(i32 seat, i32 lives, i32 pieces) override;
    bool bomb_hud_seat(i32 seat, i32 stock, i32 pieces) override;
    bool game_over();
};

}

#endif
