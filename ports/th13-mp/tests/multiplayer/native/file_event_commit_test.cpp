#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/ReplayFile.hpp"
#include "game/SpellState.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/TitleOptionsMenu.hpp"
#include "game/TitleState.hpp"
#include "sdl/ApplicationState.hpp"

#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <vector>

namespace {

using th13::u8;
using th13::u16;
using th13::u32;
using th13::i32;
using App = th13::sdl::ApplicationState;
using Audio = th13::sdl::AudioDevice;
using th13::sdl::PendingFileEvent;
using EventKind = PendingFileEvent::Kind;
using th13::sdl::Destination;
using th13::TitleScreen;
using th13::TitleOptionVolumes;
using th13::SpellEvent;
using th13::SpellEventKind;
using th13::kSpellCapture;

constexpr u16 kBootSeed = 0x1B2C;
constexpr float kFps = 60.0f;

constexpr const char* kFailDirA = "/nonexistent-t4fe-a/save.bin";
constexpr const char* kFailDirB = "/nonexistent-t4fe-b/save.bin";
constexpr const char* kFailDirC = "/nonexistent-t4fe-c/save.bin";

struct SavePathGuard {
    std::string previous;
    bool had = false;
    explicit SavePathGuard(const char* bad_path) {
        const char* old = std::getenv("TH13_SAVE_PATH");
        had = old != nullptr;
        if (had) previous = old;
        ::setenv("TH13_SAVE_PATH", bad_path, 1);
    }
    ~SavePathGuard() {
        if (had) ::setenv("TH13_SAVE_PATH", previous.c_str(), 1);
        else ::unsetenv("TH13_SAVE_PATH");
    }
};

const char* repo_root() {
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return root && *root ? root : "../../..";
}

bool pin_boot(App& app, const char* save_name) {
    ::setenv("TH13_SAVE_PATH", save_name, 1);
    const char* asset_root = std::getenv("TH13_NATIVE_ASSET_ROOT");
    if (!asset_root || !*asset_root) {
        std::printf("  失败 boot: TH13_NATIVE_ASSET_ROOT 未注入\n");
        return false;
    }
    if (!app.audio_device.initialize_for_test(
            asset_root, std::string(repo_root()) + "/games/th13/thbgmogg.dat")) {
        std::printf("  失败 boot: audio initialize_for_test: %s\n", app.audio_device.error.c_str());
        return false;
    }
    if (!app.initialize(false)) {
        std::printf("  失败 boot: initialize: %s\n", app.failure.c_str());
        return false;
    }
    app.visual.reseed_restore(kBootSeed);
    for (int i = 0; i < 256; ++i) (void)app.visual.step16();
    (void)app.visual.next32();
    (void)app.visual.next32();
    app.game_random.reseed(kBootSeed);
    return true;
}

bool boot_live_run(App& app, const char* save_name) {
    if (!pin_boot(app, save_name)) return false;
    if (!app.start_game(1)) {
        std::printf("  失败 boot: start_game: %s\n", app.failure.c_str());
        return false;
    }
    return app.pending_destination == i32(Destination::Run) && app.failure.empty();
}

bool drive(App& app, th13::GameInput& input, u32 held) {
    input.update(held);
    if (!app.update(input, false)) return false;
    if (!app.commit(kFps)) return false;
    return app.present();
}

bool encode_pinned(const App& app, std::vector<u8>& out) {
    th13::ReplaySaveMeta meta;
    meta.date = 1;
    meta.slow_numerator = 0.0;
    meta.slow_denominator = 1099511627776.0;
    return app.recorder.encode(out, "t4fe", 0, meta);
}

bool bytes_equal(const std::vector<u8>& a, const std::vector<u8>& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}

SpellEvent begin_event(i32 slot, const char* name) {
    SpellEvent event;
    event.kind = SpellEventKind::Begin;
    event.state.id = slot;
    event.state.name = name;
    return event;
}

SpellEvent captured_end_event(i32 slot) {
    SpellEvent event;
    event.kind = SpellEventKind::End;
    event.state.id = slot;
    event.state.flags |= kSpellCapture;
    return event;
}

TEST(file_event_commit_mark_heard_swallow_tier) {
    App app;
    CHECK(pin_boot(app, kFailDirA));
    if (!app.failure.empty()) return;
    CHECK(!app.files.music_heard(3));
    const Audio::Statistics pre = app.audio_device.statistics();
    app.mark_heard(3);
    CHECK(!app.files.music_heard(3));
    CHECK_EQ(app.pending_file_events_.size(), size_t(1));
    CHECK(app.pending_file_events_[0].kind == EventKind::MarkHeard);
    CHECK_EQ(app.pending_file_events_[0].capture_frame, app.recorder.capture_frame);
    CHECK_EQ(app.pending_file_events_[0].track, 3);
    const Audio::Statistics mid = app.audio_device.statistics();
    CHECK_EQ(mid.requests, pre.requests);
    CHECK_EQ(mid.started, pre.started);
    const u32 frames_pre = app.frames;
    CHECK(app.commit(kFps));
    CHECK(app.files.music_heard(3));
    CHECK(!app.files.error.empty());
    CHECK(app.failure.empty());
    CHECK_EQ(app.frames, frames_pre + 1u);
    CHECK(app.pending_file_events_.empty());
    CHECK(app.commit(kFps));
    CHECK(app.files.music_heard(3));
    CHECK_EQ(app.frames, frames_pre + 2u);
}

TEST(file_event_commit_persist_options_menu_driven_capture_commit) {
    App app;
    CHECK(pin_boot(app, kFailDirB));
    if (!app.failure.empty()) return;
    CHECK(app.title != nullptr);
    CHECK(app.title->option_menu().error.empty());
    CHECK_EQ(app.files.bgm_volume, 100);
    CHECK_EQ(app.files.effect_volume, 80);
    th13::GameInput input;
    int guard = 0;
    while ((app.title->state.screen != TitleScreen::Main ||
            app.title->state.substate != 2) &&
           guard++ < 600) {
        CHECK(drive(app, input, 0));
        if (!app.failure.empty()) return;
    }
    CHECK(guard < 600);
    guard = 0;
    while (app.title->state.menu.cursor != 7 && guard++ < 20) {
        CHECK(drive(app, input, 0x20));
        if (!app.failure.empty()) return;
        CHECK(drive(app, input, 0));
        if (!app.failure.empty()) return;
    }
    CHECK_EQ(app.title->state.menu.cursor, 7);
    CHECK(drive(app, input, 0x01));
    if (!app.failure.empty()) return;
    guard = 0;
    while (app.title->state.screen != TitleScreen::Option && guard++ < 60) {
        CHECK(drive(app, input, 0));
        if (!app.failure.empty()) return;
    }
    CHECK_EQ(i32(app.title->state.screen), i32(TitleScreen::Option));
    for (int f = 0; f < 10; ++f) {
        CHECK(drive(app, input, 0));
        if (!app.failure.empty()) return;
    }
    for (int k = 0; k < 4; ++k) {
        CHECK(drive(app, input, 0x40));
        if (!app.failure.empty()) return;
        CHECK(drive(app, input, 0));
        if (!app.failure.empty()) return;
    }
    CHECK_EQ(app.title->option_menu().volumes_ref().bgm, 80);
    CHECK_EQ(app.files.bgm_volume, 100);
    input.update(0x02);
    CHECK(app.update(input, false));
    if (!app.failure.empty()) return;
    CHECK_EQ(app.title->option_menu().row(), 4);
    input.update(0);
    CHECK(app.update(input, false));
    if (!app.failure.empty()) return;
    input.update(0x02);
    CHECK(app.update(input, false));
    if (!app.failure.empty()) return;
    CHECK(app.pending_file_events_.empty());
    u32 leave_frame = 0;
    guard = 0;
    while (app.pending_file_events_.empty() && guard++ < 20) {
        leave_frame = app.frames;
        input.update(0);
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
    }
    CHECK(!app.pending_file_events_.empty());
    if (app.pending_file_events_.empty()) return;
    CHECK(!app.pending_file_events_.empty());
    CHECK_EQ(app.pending_file_events_.size(), size_t(1));
    CHECK(app.pending_file_events_[0].kind == EventKind::PersistOptions);
    CHECK_EQ(app.pending_file_events_[0].capture_frame, leave_frame);
    CHECK_EQ(app.pending_file_events_[0].bgm, 80);
    CHECK_EQ(app.pending_file_events_[0].effect, 80);
    CHECK_EQ(app.files.bgm_volume, 100);
    CHECK_EQ(app.files.effect_volume, 80);
    CHECK_EQ(app.audio_device.music_volume, 80);
    CHECK_EQ(app.audio_device.effect_volume, 80);
    CHECK(app.title->option_menu().error.empty());
    CHECK(app.commit(kFps));
    CHECK(app.failure.empty());
    CHECK_EQ(app.files.bgm_volume, 80);
    CHECK_EQ(app.files.effect_volume, 80);
    CHECK_EQ(app.title->option_menu().error,
             std::string("Title option persistence unavailable"));
    CHECK_EQ(i32(app.title->state.screen), i32(TitleScreen::Main));
    CHECK(app.pending_file_events_.empty());
    CHECK_EQ(app.frames, leave_frame + 1u);
    CHECK(app.present());
    th13::TitleOptionVolumes volumes;
    volumes.bgm = 55;
    volumes.effect = 66;
    CHECK(app.persist_options(volumes));
    CHECK_EQ(app.files.bgm_volume, 80);
    CHECK_EQ(app.files.effect_volume, 80);
    CHECK_EQ(app.pending_file_events_.size(), size_t(1));
    CHECK(app.commit(kFps));
    CHECK(app.failure.empty());
    CHECK_EQ(app.files.bgm_volume, 55);
    CHECK_EQ(app.files.effect_volume, 66);
    CHECK_EQ(app.title->option_menu().error,
             std::string("Title option persistence unavailable"));
    const u32 frames_after = app.frames;
    CHECK(app.commit(kFps));
    CHECK(app.pending_file_events_.empty());
    CHECK_EQ(app.files.bgm_volume, 55);
    CHECK_EQ(app.frames, frames_after + 1u);
}

TEST(file_event_commit_spell_event_capture_defer_frame_fail) {
    {
        App app;
        CHECK(pin_boot(app, "t4fe-spell-a.bin"));
        if (!app.failure.empty()) return;
        app.progress.character = 0;
        const i32 attempts_before =
            app.files.spell_counters(0, 2) ? app.files.spell_counters(0, 2)->attempts[0] : -1;
        const i32 agg_before =
            app.files.spell_counters(4, 2) ? app.files.spell_counters(4, 2)->attempts[0] : -1;
        CHECK(attempts_before >= 0 && agg_before >= 0);
        CHECK(app.record_spell_event(begin_event(2, "T4spellA")));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(app.pending_file_events_[0].kind == EventKind::SpellEvent);
        CHECK_EQ(app.pending_file_events_[0].capture_frame, app.recorder.capture_frame);
        CHECK_EQ(app.pending_file_events_[0].spell_event.state.id, 2);
        CHECK_EQ(app.pending_file_events_[0].character, 0);
        CHECK_EQ(app.files.spell_counters(0, 2)->attempts[0], attempts_before);
        CHECK_EQ(app.files.spell_counters(4, 2)->attempts[0], agg_before);
        CHECK(app.commit(kFps));
        CHECK(app.files.error.empty());
        CHECK_EQ(app.files.spell_counters(0, 2)->attempts[0], attempts_before + 1);
        CHECK_EQ(app.files.spell_counters(4, 2)->attempts[0], agg_before + 1);
        CHECK(app.record_spell_event(captured_end_event(2)));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        const i32 captures_before =
            app.files.spell_counters(0, 2) ? app.files.spell_counters(0, 2)->captures[0] : -1;
        CHECK(app.commit(kFps));
        CHECK_EQ(app.files.spell_counters(0, 2)->captures[0], captures_before + 1);
        SpellEvent missed;
        missed.kind = SpellEventKind::End;
        missed.state.id = 2;
        CHECK(app.record_spell_event(missed));
        CHECK(app.pending_file_events_.empty());
        app.replay_active = true;
        CHECK(app.record_spell_event(begin_event(2, "T4spellA")));
        CHECK(app.pending_file_events_.empty());
        app.replay_active = false;
        const i32 practice_before =
            app.files.spell_counters(0, 3) ? app.files.spell_counters(0, 3)->attempts[1] : -1;
        app.progress.run_mode_flags =
            (app.progress.run_mode_flags & ~0x30u) | 0x20u;
        CHECK(app.record_spell_event(begin_event(3, "T4spellP")));
        CHECK(app.pending_file_events_[0].spell_practice);
        CHECK_EQ(app.files.spell_counters(0, 3)->attempts[1], practice_before);
        CHECK(app.commit(kFps));
        CHECK_EQ(app.files.spell_counters(0, 3)->attempts[1], practice_before + 1);
        CHECK(app.files.error.empty());
    }
    {
        App app;
        CHECK(pin_boot(app, kFailDirA));
        if (!app.failure.empty()) return;
        app.progress.character = 0;
        const i32 attempts_before =
            app.files.spell_counters(0, 2) ? app.files.spell_counters(0, 2)->attempts[0] : -1;
        CHECK(app.record_spell_event(begin_event(2, "T4spellF")));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(!app.commit(kFps));
        CHECK_EQ(app.failure, std::string("Save file open failed: ") + kFailDirA);
        CHECK_EQ(app.files.spell_counters(0, 2)->attempts[0], attempts_before + 1);
        CHECK_EQ(app.frames, 0u);
        th13::GameInput input{};
        CHECK(!app.update(input, false));
    }
    {
        App app;
        CHECK(pin_boot(app, "t4fe-spell-c.bin"));
        if (!app.failure.empty()) return;
        app.progress.character = 0;
        CHECK(app.record_spell_event(begin_event(-1, "T4bad")));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(!app.commit(kFps));
        CHECK_EQ(app.failure, std::string("Invalid spell record event"));
        CHECK_EQ(app.frames, 0u);
    }
}

TEST(file_event_commit_spell_practice_score_capture_defer_frame_fail) {
    {
        App app;
        CHECK(pin_boot(app, "t4fe-sps-a.bin"));
        if (!app.failure.empty()) return;
        app.progress.character = 0;
        app.progress.selected_spell_id = 0;
        const i32 best_before =
            app.files.spell_counters(0, 0) ? app.files.spell_counters(0, 0)->practice_best_score : -1;
        CHECK(app.record_spell_practice_score(12345));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(app.pending_file_events_[0].kind == EventKind::SpellPracticeScore);
        CHECK_EQ(app.pending_file_events_[0].score, 12345);
        CHECK_EQ(app.files.spell_counters(0, 0)->practice_best_score, best_before);
        CHECK(app.commit(kFps));
        CHECK(app.files.error.empty());
        CHECK_EQ(app.files.spell_counters(0, 0)->practice_best_score,
                 best_before > 12340 ? best_before : 12340);
        app.replay_active = true;
        CHECK(app.record_spell_practice_score(1));
        CHECK(app.pending_file_events_.empty());
        app.replay_active = false;
    }
    {
        App app;
        CHECK(pin_boot(app, kFailDirB));
        if (!app.failure.empty()) return;
        app.progress.character = 0;
        app.progress.selected_spell_id = 0;
        CHECK(app.record_spell_practice_score(500));
        CHECK(!app.commit(kFps));
        CHECK_EQ(app.failure, std::string("Save file open failed: ") + kFailDirB);
        CHECK_EQ(app.frames, 0u);
    }
}

TEST(file_event_commit_stage_clear_gate_capture_defer_frame_fail) {
    {
        App app;
        CHECK(pin_boot(app, "t4fe-sc-g.bin"));
        if (!app.failure.empty()) return;
        CHECK(app.record_stage_clear());
        CHECK(app.pending_file_events_.empty());
        app.progress.stage = 6;
        app.progress.practice_mode = 1;
        CHECK(app.record_stage_clear());
        CHECK(app.pending_file_events_.empty());
        app.progress.practice_mode = 0;
        app.progress.run_mode_flags |= 0x30u;
        CHECK(app.record_stage_clear());
        CHECK(app.pending_file_events_.empty());
        app.progress.run_mode_flags &= ~0x30u;
        CHECK(app.record_stage_clear());
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(app.pending_file_events_[0].kind == EventKind::StageClear);
        CHECK_EQ(app.pending_file_events_[0].character, app.progress.character);
        CHECK_EQ(app.pending_file_events_[0].difficulty, app.progress.difficulty);
        CHECK_EQ(app.pending_file_events_[0].no_continue, app.progress.continues == 0);
    }
    {
        App app;
        CHECK(pin_boot(app, "t4fe-sc-a.bin"));
        if (!app.failure.empty()) return;
        app.progress.character = 1;
        app.progress.difficulty = 2;
        app.progress.continues = 0;
        app.progress.stage = 6;
        const u32 clear_before = app.files.ordinary_records[1].clear_count[2];
        const u32 nc_before = app.files.ordinary_records[1].no_continue_clear_count[2];
        CHECK(app.record_stage_clear());
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK_EQ(app.files.ordinary_records[1].clear_count[2], clear_before);
        app.ordinary_records_dirty = true;
        CHECK(app.commit(kFps));
        CHECK(app.files.error.empty());
        CHECK_EQ(app.files.ordinary_records[1].clear_count[2], clear_before + 1u);
        CHECK_EQ(app.files.ordinary_records[1].no_continue_clear_count[2], nc_before + 1u);
        CHECK(app.files.cleared(1, 2));
        CHECK(!app.ordinary_records_dirty);
    }
    {
        App app;
        CHECK(pin_boot(app, kFailDirC));
        if (!app.failure.empty()) return;
        app.progress.character = 0;
        app.progress.difficulty = 1;
        app.progress.continues = 3;
        app.progress.stage = 7;
        const u32 clear_before = app.files.ordinary_records[0].clear_count[1];
        app.ordinary_records_dirty = true;
        CHECK(app.record_stage_clear());
        SavePathGuard guard(kFailDirC);
        CHECK(!app.commit(kFps));
        CHECK_EQ(app.failure, std::string("Save file open failed: ") + kFailDirC);
        CHECK_EQ(app.files.ordinary_records[0].clear_count[1], clear_before + 1u);
        CHECK(!app.files.cleared(0, 1));
        CHECK(app.ordinary_records_dirty);
        CHECK_EQ(app.frames, 0u);
    }
}

TEST(file_event_commit_pause_option_capture_defer_frame_fail) {
    {
        App app;
        CHECK(pin_boot(app, "t4fe-po-a.bin"));
        if (!app.failure.empty()) return;
        CHECK(!app.change_pause_option(-1, 5));
        CHECK(!app.change_pause_option(2, 5));
        CHECK(app.pending_file_events_.empty());
        CHECK(app.failure.empty());
        app.files.bgm_volume = 50;
        const i32 device_bgm_before = app.audio_device.music_volume;
        CHECK(app.change_pause_option(0, 30));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(app.pending_file_events_[0].kind == EventKind::PauseOption);
        CHECK_EQ(app.pending_file_events_[0].row, 0);
        CHECK_EQ(app.pending_file_events_[0].delta, 30);
        CHECK_EQ(app.files.bgm_volume, 50);
        CHECK_EQ(app.audio_device.music_volume, device_bgm_before);
        CHECK(app.commit(kFps));
        CHECK(app.files.error.empty());
        CHECK_EQ(app.files.bgm_volume, 80);
        CHECK_EQ(app.audio_device.music_volume, 80);
        app.files.effect_volume = 10;
        CHECK(app.change_pause_option(1, -30));
        CHECK(app.commit(kFps));
        CHECK_EQ(app.files.effect_volume, 0);
        CHECK_EQ(app.audio_device.effect_volume, 0);
        app.files.bgm_volume = 90;
        CHECK(app.change_pause_option(0, 30));
        CHECK(app.commit(kFps));
        CHECK_EQ(app.files.bgm_volume, 100);
    }
    {
        App app;
        CHECK(pin_boot(app, kFailDirA));
        if (!app.failure.empty()) return;
        app.files.bgm_volume = 50;
        CHECK(app.change_pause_option(0, 30));
        SavePathGuard guard(kFailDirA);
        CHECK(!app.commit(kFps));
        CHECK_EQ(app.failure, std::string("Save file open failed: ") + kFailDirA);
        CHECK_EQ(app.files.bgm_volume, 80);
        CHECK_EQ(app.audio_device.music_volume, 80);
        CHECK_EQ(app.frames, 0u);
        th13::GameInput input{};
        CHECK(!app.update(input, false));
    }
}

TEST(file_event_commit_result_entry_capture_defer_and_commit_truncation) {
    {
        App app;
        CHECK(pin_boot(app, "t4fe-re-a.bin"));
        if (!app.failure.empty()) return;
        app.progress.character = 2;
        app.progress.difficulty = 1;
        app.progress.continues = 1;
        app.selection_score.score = 1000000;
        const th13::TitlePlayerDataScore baseline =
            app.files.ordinary_records[2].ranking[1][0];
        CHECK(app.record_result_entry("T4R", 1));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(app.pending_file_events_[0].kind == EventKind::ResultEntry);
        CHECK_EQ(app.pending_file_events_[0].result.score, 1000000);
        CHECK_EQ(app.pending_file_events_[0].result.stage, 1);
        CHECK_EQ(app.pending_file_events_[0].result.name, std::string("T4R"));
        CHECK_EQ(app.pending_file_events_[0].result.available, true);
        CHECK(app.pending_file_events_[0].result.timestamp > 0);
        CHECK(app.files.ordinary_records[2].ranking[1][0].score == baseline.score &&
              app.files.ordinary_records[2].ranking[1][0].available == baseline.available);
        CHECK(app.commit(kFps));
        CHECK(app.files.error.empty());
        CHECK_EQ(app.files.ordinary_records[2].ranking[1][0].score, 1000000);
        CHECK_EQ(app.files.ordinary_records[2].ranking[1][0].name, std::string("T4R"));
    }
    App app;
    CHECK(boot_live_run(app, "t4fe-re-trunc.bin"));
    if (!app.failure.empty()) return;
    app.progress.character = 3;
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    CHECK_EQ(app.pending_load, 1u);
    input.update(0);
    CHECK(app.update(input, false));
    if (!app.failure.empty()) return;
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(1));
    CHECK_EQ(app.pending_play_frames_, 1u);
    std::vector<u8> enc_pre;
    CHECK(encode_pinned(app, enc_pre));
    const i32 mirror_pre = app.files.ordinary_records[3].total_frames;
    const u32 frames_pre = app.frames;
    app.selection_score.score = 999999;
    CHECK(app.record_result_entry("T4X", 1));
    CHECK_EQ(app.pending_file_events_.size(), size_t(1));
    CHECK(app.set_voice_pan(55, 192.f));
    CHECK_EQ(app.pending_voice_pans_.size(), size_t(1));
    const u32 pending_pre = app.audio_device.statistics().pending_voices;
    CHECK(app.audio_device.enqueue(44));
    CHECK_EQ(u32(app.audio_device.statistics().pending_voices), pending_pre + 1u);
    {
        SavePathGuard guard(kFailDirB);
        CHECK(!app.commit(kFps));
        CHECK_EQ(app.failure, std::string("Save file open failed: ") + kFailDirB);
    }
    CHECK(app.recorder.pending_inputs.empty());
    std::vector<u8> enc_post;
    CHECK(encode_pinned(app, enc_post));
    CHECK(!bytes_equal(enc_post, enc_pre));
    const i32 run_difficulty = app.progress.difficulty;
    CHECK(run_difficulty >= 0 && run_difficulty < 5);
    CHECK_EQ(app.files.ordinary_records[3].ranking[size_t(run_difficulty)][0].score, 999999);
    CHECK_EQ(app.pending_voice_pans_.size(), size_t(1));
    CHECK_EQ(app.pending_play_frames_, 1u);
    CHECK_EQ(app.files.ordinary_records[3].total_frames, mirror_pre);
    CHECK_EQ(u32(app.audio_device.statistics().pending_voices), pending_pre + 1u);
    CHECK_EQ(app.frames, frames_pre);
    input.update(0);
    CHECK(!app.update(input, false));
    CHECK(!app.step(input, kFps, false));
    CHECK_EQ(app.frames, frames_pre);
}

TEST(file_event_commit_music_unlocked_capture_validate_frame_fail) {
    {
        App app;
        CHECK(pin_boot(app, "t4fe-mu-a.bin"));
        if (!app.failure.empty()) return;
        CHECK(!app.files.music_heard(5));
        CHECK(app.record_music_unlocked(5));
        CHECK_EQ(app.pending_file_events_.size(), size_t(1));
        CHECK(app.pending_file_events_[0].kind == EventKind::MusicUnlocked);
        CHECK_EQ(app.pending_file_events_[0].track, 5);
        CHECK(!app.files.music_heard(5));
        CHECK(app.commit(kFps));
        CHECK(app.files.error.empty());
        CHECK(app.files.music_heard(5));
    }
    {
        App app;
        CHECK(pin_boot(app, "t4fe-mu-b.bin"));
        if (!app.failure.empty()) return;
        CHECK(!app.record_music_unlocked(32));
        CHECK_EQ(app.failure, std::string("Ending music index out of range"));
        CHECK(app.pending_file_events_.empty());
        CHECK(!app.record_music_unlocked(-1));
        CHECK(app.pending_file_events_.empty());
    }
    {
        App app;
        CHECK(pin_boot(app, kFailDirC));
        if (!app.failure.empty()) return;
        CHECK(app.record_music_unlocked(7));
        SavePathGuard guard(kFailDirC);
        CHECK(!app.commit(kFps));
        CHECK_EQ(app.failure, std::string("Save file open failed: ") + kFailDirC);
        CHECK(app.files.music_heard(7));
        CHECK_EQ(app.frames, 0u);
    }
}

TEST(file_event_commit_capture_order_truncation) {
    App app;
    CHECK(pin_boot(app, "t4fe-order.bin"));
    if (!app.failure.empty()) return;
    app.progress.character = 0;
    app.mark_heard(2);
    CHECK(app.record_spell_event(begin_event(-1, "T4order")));
    app.mark_heard(3);
    CHECK_EQ(app.pending_file_events_.size(), size_t(3));
    CHECK_EQ(app.pending_file_events_[0].kind, EventKind::MarkHeard);
    CHECK(app.pending_file_events_[1].kind == EventKind::SpellEvent);
    CHECK_EQ(app.pending_file_events_[2].kind, EventKind::MarkHeard);
    CHECK(!app.commit(kFps));
    CHECK(app.files.music_heard(2));
    CHECK_EQ(app.failure, std::string("Invalid spell record event"));
    CHECK(!app.files.music_heard(3));
    CHECK_EQ(app.pending_file_events_.size(), size_t(1));
    CHECK(app.pending_file_events_[0].kind == EventKind::MarkHeard);
    CHECK_EQ(app.pending_file_events_[0].track, 3);
    CHECK_EQ(app.frames, 0u);
}

TEST(file_event_commit_save_replay_slot_confirmed_tier_in_place) {
    {
        App app;
        CHECK(pin_boot(app, "t4fe-srs-neg.bin"));
        if (!app.failure.empty()) return;
        CHECK(!app.save_replay_slot(0, "PLAYER"));
        CHECK_EQ(app.failure, std::string("Invalid replay slot"));
        CHECK(!app.save_replay_slot(26, "PLAYER"));
        CHECK(app.pending_file_events_.empty());
        CHECK_EQ(app.frames, 0u);
    }
    App app;
    CHECK(boot_live_run(app, "t4fe-srs-a.bin"));
    if (!app.failure.empty()) return;
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    input.update(0);
    CHECK(app.update(input, false));
    if (!app.failure.empty()) return;
    CHECK(app.recorder.ready());
    const u32 frames_pre = app.frames;
    CHECK(app.pending_file_events_.empty());
    CHECK(app.save_replay_slot(1, "PLAYER"));
    CHECK(app.failure.empty());
    CHECK(app.pending_file_events_.empty());
    CHECK(app.replay_slots[0].decoded != nullptr);
    CHECK_EQ(app.replay_filename(0), std::string("th13_01.rpy"));
    CHECK_EQ(app.frames, frames_pre);
    if (FILE* file = std::fopen("th13_01.rpy", "rb")) {
        std::fclose(file);
    } else {
        std::printf("  失败: 确认拍未落盘 th13_01.rpy（cwd=%s）\n", ".");
        CHECK(false);
    }
}

}

int main() { return th13test::run_all("m1.unit_file_event_commit"); }
