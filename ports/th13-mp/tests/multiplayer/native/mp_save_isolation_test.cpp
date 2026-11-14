// RULES R-X: MP results and archives must not modify retail single-player data.
// The terminal fixtures exercise persistence APIs; they do not claim a game clear.
#include "../../harness/test_framework.hpp"
#include "sdl/ApplicationState.hpp"
#include "multiplayer/Session.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <unistd.h>

using namespace th13;
using namespace th13test;

namespace {
using App = sdl::ApplicationState;

std::string save_path(const std::string& name) {
    return "save-isolation-" + std::to_string(::getpid()) + "-" + name;
}

std::vector<u8> bytes(const std::string& path) {
    FILE* file = std::fopen(path.c_str(), "rb");
    if (!file) return {};
    std::vector<u8> result;
    int value;
    while ((value = std::fgetc(file)) != EOF) result.push_back(u8(value));
    std::fclose(file);
    return result;
}

SpellEvent spell(SpellEventKind kind) {
    SpellEvent event;
    event.kind = kind;
    event.state.id = 0;
    event.state.name = "ISOLATION";
    event.state.flags = kSpellCapture;
    return event;
}

void progress(App& app) {
    app.progress.character = 0;
    app.progress.difficulty = 1;
    app.progress.stage = 6;
    app.progress.practice_mode = 0;
    app.progress.run_mode_flags = 0;
    app.progress.continues = 0;
    app.progress.selected_spell_id = 0;
    app.selection_score.score = 314159;
}

bool enqueue(App& app) {
    app.mark_heard(21);
    return app.record_spell_event(spell(SpellEventKind::Begin)) &&
        app.record_spell_event(spell(SpellEventKind::End)) &&
        app.record_spell_practice_score(200) && app.record_stage_clear() &&
        app.record_result_name("GUARD") && app.record_clear_result_name("GUARD") &&
        app.record_music_unlocked(20);
}

bool flush(App& app) {
    return app.commit_pending_file_events() && app.commit_pending_play_frames();
}

void stale_events(App& app) {
    for (auto kind : {sdl::PendingFileEvent::Kind::SpellEvent,
            sdl::PendingFileEvent::Kind::SpellPracticeScore,
            sdl::PendingFileEvent::Kind::StageClear,
            sdl::PendingFileEvent::Kind::ResultEntry,
            sdl::PendingFileEvent::Kind::MusicUnlocked,
            sdl::PendingFileEvent::Kind::MarkHeard}) {
        sdl::PendingFileEvent event;
        event.kind = kind;
        event.character = 0;
        event.difficulty = 1;
        event.spell_id = 0;
        event.score = 700;
        event.no_continue = true;
        event.spell_event = spell(SpellEventKind::Begin);
        event.result.available = true;
        event.result.name = "STALE";
        event.result.timestamp = 123;
        event.result.score = 700;
        event.result.stage = 8;
        event.track = 22;
        app.pending_file_events_.push_back(event);
    }
    app.pending_play_frames_ = 9;
}

bool seed_save(App& app) {
    // Preserve existing nonzero data, not just an initially empty FileStore.
    app.files.record_run_start(1);
    app.files.record_run_clear(1, 2, true);
    app.files.set_extra_unlocked(1);
    app.files.record_spell_attempt(1, 0, "BASE", false);
    app.files.ending_seen_bytes[3] = 17;
    return app.files.save_score();
}
} // namespace

TEST(mp_save_isolation_keeps_single_player_persistence_working) {
    multiplayer::session.Stop();
    ::setenv("TH13_SAVE_PATH", save_path("sp.bin").c_str(), 1);
    App app;
    progress(app);
    CHECK(app.files.initialize());
    CHECK(enqueue(app));
    app.pending_play_frames_ = 7;
    CHECK(flush(app));
    const auto* counts = app.files.spell_counters(0, 0);
    const auto& records = app.files.ordinary_records[0];
    CHECK(counts && counts->attempts[0] == 1 && counts->captures[0] == 1);
    CHECK(records.clear_count[1] == 1 && records.no_continue_clear_count[1] == 1);
    CHECK((app.files.cleared_mask & 2) && records.total_frames == 7);
    bool ranked = false;
    for (const auto& row : records.ranking[1])
        ranked |= row.name == "GUARD" && row.score == 314159;
    CHECK(ranked);
    CHECK(app.files.music_heard(20) && app.files.music_heard(21));
    const auto before = app.files.ending_seen_bytes;
    EndingScene ending(app.progress, app);
    CHECK(ending.initialize() && app.files.ending_seen_bytes != before);
    CHECK(app.files.save_score());
}

TEST(mp_save_isolation_still_persists_device_volume_options) {
    multiplayer::session.Stop();
    for (unsigned count : {2u, 3u}) for (unsigned phase = 0; phase < 2; ++phase) {
        const auto file = save_path("options-" + std::to_string(count) + "-" +
            std::to_string(phase) + ".bin");
        ::setenv("TH13_SAVE_PATH", file.c_str(), 1);
        App app;
        CHECK(app.files.initialize());
        CHECK(app.configure_multiplayer(count, {0, 0, 0}, 0));
        CHECK(seed_save(app));
        const auto before = bytes(file);
        CHECK(!before.empty());
        // Compare the entire persisted file to an independent FileStore write
        // with only the expected device-volume values changed.
        auto expected = app.files;
        if (phase == 0) {
            CHECK(app.change_pause_option(0, -7));
            expected.bgm_volume -= 7;
        } else {
            CHECK(app.persist_options({37, 46}));
            expected.bgm_volume = 37;
            expected.effect_volume = 46;
        }
        CHECK(flush(app));
        const auto actual = bytes(file);
        CHECK(actual != before);
        CHECK_EQ(app.files.bgm_volume, expected.bgm_volume);
        CHECK_EQ(app.files.effect_volume, expected.effect_volume);
        ::setenv("TH13_SAVE_PATH", (file + ".expected").c_str(), 1);
        CHECK(expected.save());
        CHECK_EQ(actual, bytes(file + ".expected"));
    }
}

TEST(mp_save_isolation_preserves_existing_sp_bytes_for_new_stale_and_terminal_events) {
    multiplayer::session.Stop();
    for (unsigned count : {2u, 3u}) for (unsigned phase = 0; phase < 3; ++phase) {
        const auto file = save_path("mp-" + std::to_string(count) + "-" +
            std::to_string(phase) + ".bin");
        ::setenv("TH13_SAVE_PATH", file.c_str(), 1);
        App app;
        progress(app);
        CHECK(app.files.initialize());
        CHECK(app.configure_multiplayer(count, {0, 0, 0}, 0));
        CHECK(seed_save(app));
        const auto before = bytes(file);
        CHECK(!before.empty());
        if (phase == 0) {
            CHECK(enqueue(app));
            app.pending_play_frames_ = 7;
        } else if (phase == 1) {
            stale_events(app);
        } else {
            EndingScene ending(app.progress, app);
            CHECK(ending.initialize());
            CHECK(enqueue(app));
        }
        CHECK(flush(app));
        CHECK(app.pending_file_events_.empty() && app.pending_play_frames_ == 0);
        CHECK(app.files.save_score());
        CHECK_EQ(bytes(file), before);
    }
}

TEST(mp_save_isolation_preserves_sp_data_through_real_begin_and_title_return) {
    multiplayer::session.Stop();
    const char* assets = std::getenv("TH13_NATIVE_ASSET_ROOT");
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    CHECK(assets && root);
    if (!assets || !root) return;
    for (unsigned count : {2u, 3u}) {
        const auto file = save_path("lifecycle-" + std::to_string(count) + ".bin");
        ::setenv("TH13_SAVE_PATH", file.c_str(), 1);
        App app;
        CHECK(app.audio_device.initialize_for_test(assets,
            std::string(root) + "/games/th13/thbgmogg.dat"));
        CHECK(app.initialize(false));
        CHECK(app.configure_multiplayer(count, {0, 0, 0}, 0));
        CHECK(seed_save(app));
        const auto before = bytes(file);
        CHECK(!before.empty());
        app.progress.character = 0;
        app.progress.difficulty = 4;
        CHECK(app.start_game(7));
        app.pending_destination = -1;
        app.game_random.reseed_restore(8719);
        app.visual.reseed_restore(8719);
        CHECK(app.begin_run() && app.complete_loading());
        GameInput input;
        for (unsigned frame = 0; frame < 8; ++frame) {
            input.update(1);
            CHECK(app.update(input, false) && app.commit(60.f) && app.present());
        }
        CHECK(app.return_to_title() && app.present());
        CHECK_EQ(bytes(file), before);
    }
}

TEST(mp_save_isolation_never_falls_back_to_single_player_replay_slots) {
    multiplayer::session.Stop();
    for (unsigned count : {2u, 3u}) {
        App app;
        progress(app);
        CHECK(app.configure_multiplayer(count, {0, 0, 0}, 0));
        const auto before1 = bytes("th13_01.rpy");
        const auto before2 = bytes("th13_02.rpy");
        CHECK(!app.can_save_replay());
        CHECK(!app.save_replay_slot(1, "NOSP"));
        CHECK_EQ(bytes("th13_01.rpy"), before1);
        unsigned calls = 0;
        app.multiplayer_replay_available = [] { return true; };
        app.multiplayer_replay_save = [&](i32 slot, const std::string& name) {
            ++calls;
            return slot == 2 && name == "MPONLY";
        };
        CHECK(app.can_save_replay());
        CHECK(app.save_replay_slot(2, "MPONLY") && calls == 1);
        CHECK_EQ(bytes("th13_02.rpy"), before2);
    }
}

int __attribute__((weak)) main() { return run_all("MP single-player save isolation"); }
