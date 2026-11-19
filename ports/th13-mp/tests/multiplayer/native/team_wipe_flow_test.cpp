// R-G: the production scene must end an exhausted team after 180 active ticks.
#include "../../harness/test_framework.hpp"
#include "sdl/ApplicationState.hpp"
#include "multiplayer/Session.hpp"
#include "multiplayer/TerminalHandoff.hpp"
#include "multiplayer/WorldState.hpp"

#include <cstdlib>

using namespace th13;
using namespace th13test;

namespace {
using App = sdl::ApplicationState;

bool frame(App& app, GameInput& input, u32 held = 0) {
    input.update(held);
    return app.update(input, false) && app.commit(60.f) && app.present() && app.failure.empty();
}

bool boot(App& app, i32 seats, const char* name) {
    multiplayer::session.Stop();
    const char* assets = std::getenv("TH13_NATIVE_ASSET_ROOT");
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    if (!assets || !root) return false;
    ::setenv("TH13_SAVE_PATH", name, 1);
    if (!app.audio_device.initialize_for_test(assets,
            std::string(root) + "/games/th13/thbgmogg.dat") || !app.initialize(false))
        return false;
    if (!app.configure_multiplayer(seats, {0, 1, 3}, 0) || !app.start_game(1)) return false;
    app.pending_destination = -1;
    app.frames = 0;
    app.game_random.reseed_restore(1234);
    app.visual.reseed_restore(1234);
    if (!app.begin_run()) return false;
    GameInput input;
    for (unsigned i = 0; i < 35; ++i) if (!frame(app, input)) return false;
    return !app.pause->active() && app.progress.stage_frame > 0;
}

void exhaust(App& app) {
    for (i32 seat = 0; seat < app.run->battle.seat_count; ++seat) {
        auto& player = app.run->battle.seats[size_t(seat)];
        player.ghost = true;
        player.dead = true;
        app.run->battle.seat_huds[size_t(seat)].extra_lives = -1;
    }
}

bool world(App& app, multiplayer::WorldState& state) {
    return state.capture(app.run->battle, app.visual, nullptr,
        app.run->presentation.get(), app.run.get(), app.run_transition.handle());
}
}

TEST(team_wipe_production_180) {
    for (i32 seats : {2, 3}) {
        App app;
        CHECK(boot(app, seats, seats == 2 ? "team-wipe-dual.bin" : "team-wipe-triple.bin"));
        if (!app.run) return;
        exhaust(app);
        CHECK(app.run->battle.team_wiped());
        // The final seat enters ghost state in StageBattle's normal dying
        // update; that same logical tick starts the scene's wait interval.
        auto& last = app.run->battle.seats[size_t(seats - 1)];
        last.ghost = false;
        last.dead = false;
        last.phase = StagePlayerState::Phase::Dying;
        last.phase_frame = 30;
        const i32 initial_frame = app.progress.stage_frame;
        GameInput input;
        for (unsigned tick = 1; tick < 180; ++tick) {
            CHECK(frame(app, input));
            CHECK(!app.pause->active());
            CHECK_EQ(app.progress.stage_frame, initial_frame + i32(tick));
            CHECK(app.run->battle.team_wiped());
            CHECK_EQ(app.run->capture_rollback().team_wipe_frames, tick);
        }
        CHECK(frame(app, input));
        CHECK_EQ(app.progress.stage_frame, initial_frame + 180);
        CHECK(app.pause->active());
        CHECK(app.pause->state.screen == PauseScreen::BGameOver);
        CHECK(app.game_over_music.empty());
        CHECK_EQ(app.game_over_music_cursor, 0u);
        CHECK_EQ(app.run->capture_rollback().team_wipe_frames, 180u);
        CHECK(app.run->capture_rollback().team_wipe_game_over);
        for (unsigned i = 0; i < 12; ++i) CHECK(frame(app, input));
        CHECK_EQ(app.pause->state.phase, 6);
        CHECK_EQ(app.pause->state.menu.count, 3);
        CHECK_EQ(app.progress.stage_frame, initial_frame + 180);
    }
}

TEST(team_wipe_pause_and_rescue) {
    App app;
    CHECK(boot(app, 2, "team-wipe-pause-rescue.bin"));
    if (!app.run) return;
    exhaust(app);
    GameInput input;
    for (unsigned i = 0; i < 70; ++i) CHECK(frame(app, input));
    CHECK_EQ(app.run->capture_rollback().team_wipe_frames, 70u);
    CHECK(app.request_pause());
    const i32 paused_frame = app.progress.stage_frame;
    for (unsigned i = 0; i < 40; ++i) {
        CHECK(frame(app, input));
        CHECK_EQ(app.run->capture_rollback().team_wipe_frames, 70u);
        CHECK_EQ(app.progress.stage_frame, paused_frame);
    }
    CHECK(frame(app, input, 0x100));
    for (unsigned i = 0; i < 15; ++i) CHECK(frame(app, input));
    CHECK(!app.pause->active());
    CHECK_EQ(app.run->capture_rollback().team_wipe_frames,
        70u + u32(app.progress.stage_frame - paused_frame));
    const u32 before_transition = app.run->capture_rollback().team_wipe_frames;
    app.run->transition_wait = true;
    for (unsigned i = 0; i < 5; ++i) {
        CHECK(frame(app, input));
        CHECK_EQ(app.run->capture_rollback().team_wipe_frames, before_transition);
    }
    app.run->transition_wait = false;

    // A production revival interrupts the interval; five real Shift edges
    // then rescue a ghost through RunScene -> StageBattle's input path.
    auto& battle = app.run->battle;
    battle.revive_all_on_stage_clear();
    CHECK(frame(app, input));
    CHECK_EQ(app.run->capture_rollback().team_wipe_frames, 0u);
    battle.seats[1].ghost = true;
    battle.seats[1].dead = true;
    battle.seat_huds[0].extra_lives = 2;
    battle.seats[1].x = battle.seats[0].x;
    battle.seats[1].y = battle.seats[0].y;
    battle.seats[1].fixed_x = battle.seats[0].fixed_x;
    battle.seats[1].fixed_y = battle.seats[0].fixed_y;
    for (unsigned tap = 0; tap < 5; ++tap) {
        CHECK(frame(app, input, 8));
        CHECK(frame(app, input));
        CHECK_EQ(app.run->capture_rollback().team_wipe_frames, 0u);
    }
    CHECK(!battle.seats[1].ghost);
    CHECK_EQ(battle.seat_huds[0].extra_lives, 1);
    CHECK_EQ(battle.seats[1].invulnerable, 239);
    exhaust(app);
    for (unsigned i = 1; i < 180; ++i) {
        CHECK(frame(app, input));
        CHECK(!app.pause->active());
        CHECK_EQ(app.run->capture_rollback().team_wipe_frames, i);
    }
    CHECK(frame(app, input));
    CHECK(app.pause->state.screen == PauseScreen::BGameOver);
}

TEST(team_wipe_restore_replay) {
    App app;
    CHECK(boot(app, 3, "team-wipe-restore.bin"));
    if (!app.run) return;
    exhaust(app);
    GameInput input;
    for (unsigned i = 0; i < 175; ++i) CHECK(frame(app, input));
    const auto saved_input = input;
    const auto restore_host = app.capture_multiplayer_host();
    multiplayer::WorldState checkpoint;
    CHECK(world(app, checkpoint));
    CHECK_EQ(app.run->capture_rollback().team_wipe_frames, 175u);

    // Change only each countdown field. Full/core and the scene domain must
    // all detect state that changes the next terminal update.
    const auto original_scene = app.run->capture_rollback();
    char original_domains[4096]{};
    checkpoint.digest_domains(original_domains, sizeof(original_domains));
    for (unsigned mutation = 0; mutation < 2; ++mutation) {
        auto altered_scene = original_scene;
        if (mutation == 0) ++altered_scene.team_wipe_frames;
        else altered_scene.team_wipe_game_over = true;
        std::unordered_map<const AnmVm*, AnmVm*> remapped;
        app.run->restore_rollback(altered_scene, remapped);
        multiplayer::WorldState altered;
        CHECK(world(app, altered));
        CHECK(altered.digest() != checkpoint.digest());
        CHECK(altered.digest_core() != checkpoint.digest_core());
        char altered_domains[4096]{};
        altered.digest_domains(altered_domains, sizeof(altered_domains));
        CHECK(std::string(altered_domains) != original_domains);
        CHECK(checkpoint.restore(app.run->battle, app.visual, nullptr,
            app.run->presentation.get(), app.run.get()));
        CHECK(restore_host());
    }

    std::array<std::array<u64, 2>, 18> expected{};
    std::array<std::string, 18> expected_music;
    for (unsigned i = 0; i < expected.size(); ++i) {
        CHECK(frame(app, input));
        multiplayer::WorldState actual;
        CHECK(world(app, actual));
        expected[i] = {actual.digest(), app.digest_multiplayer_host()};
        expected_music[i] = app.game_over_music;
        CHECK_EQ(app.pause->active(), i >= 4);
    }
    CHECK(app.run->capture_rollback().team_wipe_game_over);
    CHECK(checkpoint.restore(app.run->battle, app.visual, nullptr,
        app.run->presentation.get(), app.run.get()));
    CHECK(restore_host());
    CHECK(!app.pause->active());
    CHECK_EQ(app.run->capture_rollback().team_wipe_frames, 175u);
    CHECK(!app.run->capture_rollback().team_wipe_game_over);
    input = saved_input;
    for (unsigned i = 0; i < expected.size(); ++i) {
        CHECK(frame(app, input));
        multiplayer::WorldState actual;
        CHECK(world(app, actual));
        CHECK_EQ(actual.digest(), expected[i][0]);
        if (app.game_over_music != expected_music[i])
            std::printf("# game-over music mismatch frame %u: expected='%s' actual='%s'\n",
                i, expected_music[i].c_str(), app.game_over_music.c_str());
        CHECK_EQ(app.digest_multiplayer_host(), expected[i][1]);
        CHECK_EQ(app.pause->active(), i >= 4);
    }
    CHECK_EQ(app.run->capture_rollback().team_wipe_frames, 180u);
}

TEST(team_wipe_single_seat_unchanged) {
    App app;
    CHECK(boot(app, 0, "team-wipe-single.bin"));
    if (!app.run) return;
    GameInput input;
    for (unsigned i = 0; i < 181; ++i) CHECK(frame(app, input));
    CHECK(!app.pause->active());
    CHECK_EQ(app.run->capture_rollback().team_wipe_frames, 0u);
    CHECK(!app.run->capture_rollback().team_wipe_game_over);
    app.selection_player.extra_lives = -1;
    app.run->battle.player_dead = false;
    app.run->battle.player_phase = StagePlayerState::Phase::Dying;
    app.run->battle.player_phase_frame = 30;
    CHECK(frame(app, input));
    CHECK(app.pause->state.screen == PauseScreen::GameOver);
    CHECK(!app.game_over_music.empty());
}

TEST(team_wipe_terminal_delayed_confirmation_requires_handoff) {
    using namespace th13::multiplayer;
    for (const auto screen : {PauseScreen::BGameOver, PauseScreen::GameOver,
                              PauseScreen::ExtraEnd}) {
    for (const bool pending_title : {false, true}) {
    for (const bool fail_audio : {false, true}) {
    App app;
    const bool booted = boot(app, 3, "team-wipe-delayed-terminal.bin");
    CHECK(booted); if (!booted || !app.run) return;
    exhaust(app);
    GameInput input;
    for (unsigned i = 0; i < 179; ++i) {
        const bool ok = frame(app, input); CHECK(ok); if (!ok) return;
    }
    CHECK(!app.pause->active());
    SessionSetup setup; setup.version = 2; setup.mode = 2; setup.automatic = false;
    setup.session_id = 42; setup.build[0] = 1; setup.player_count = 3;
    const auto words = EncodeSessionSetup(setup);
    const bool configured = session.Configure(words.data(), words.size());
    CHECK(configured); if (!configured) return;
    session.BindWorld(&app.run->battle, &app.visual, app.run->presentation.get(),
                      app.run.get(), &app.mp_transition_handle_);
    session.capture_host = [&] { return app.capture_multiplayer_host(); };
    app.run->playback_input = [] { return session.LocalDelayedInput(); };
    const bool started = session.BeginLocalSimulation();
    CHECK(started); if (!started) { session.Stop(); return; }
    app.wire_multiplayer_audio();
    // No terminal overlay: even an empty record ledger must not end gameplay.
    CHECK(session.AllFramesCommitted());
    CHECK(complete_terminal_handoff(app, session) == TerminalHandoff::NotReady);
    CHECK(session.Simulating());
    CHECK(session.CaptureLocal(0, 0)); session.AdvanceCapturePhase();
    const bool granted = session.CanSimulate(); CHECK(granted);
    if (!granted) { session.Stop(); return; }
    CHECK(app.update(session.MenuInput(), false)); CHECK(app.commit(60.f));
    // Final-frame requests are deferred until the remote inputs arrive.
    // Use the real device observer/validation and capture all applied events.
    CHECK(app.audio_device.stop(-1));
    CHECK(app.audio_device.observe_voice_pan(55, 192.f));
    CHECK(app.audio_device.queue_music(2, -1, "th128_08.wav"));
    session.OnFrameSimulated();
    CHECK(app.pause->state.screen == PauseScreen::BGameOver);
    // Each terminal screen uses the same existing present-tail handoff.
    // The team-wipe fixture reaches BGameOver; the other two screens are
    // selected here solely to exercise that common handoff boundary.
    app.pause->state.screen = screen;
    CHECK(!session.AllFramesCommitted());
    CHECK(complete_terminal_handoff(app, session) == TerminalHandoff::NotReady);
    CHECK(app.present()); CHECK(session.Simulating());
    for (unsigned seat = 1; seat < 3; ++seat) {
        const auto result = session.SubmitRemote(seat, 0, {});
        CHECK(result == Netplay::RemoteInputResult::Accepted ||
              result == Netplay::RemoteInputResult::PredictionCorrect);
        if (seat == 1) {
            CHECK(session.Pump(1));
            CHECK(!session.AllFramesCommitted());
            CHECK(complete_terminal_handoff(app, session) == TerminalHandoff::NotReady);
            CHECK(session.Simulating());
        }
    }
    CHECK(session.Pump(2));
    CHECK(session.AllFramesCommitted()); CHECK(session.Simulating());
    using Kind = sdl::AudioDevice::RequestObservation::Kind;
    // The production game-over update queued its own music before our three
    // requests; it must survive and stay first in the confirmed sequence.
    const std::array<Kind, 4> audio_kinds{Kind::Music, Kind::Stop, Kind::Pan, Kind::Music};
    CHECK_EQ(session.AudioOut().size(), audio_kinds.size());
    const auto applied_before = session.AppliedAudio().size();
    const auto stopped_before = app.audio_device.statistics().stopped;
    const auto music_before = session.MusicApplied();
    const auto before_frame = app.progress.stage_frame;
    const auto before_app_frames = app.frames;
    const auto before_phase = app.pause->state.phase;
    multiplayer::WorldState before; CHECK(world(app, before));
    if (pending_title) app.pending_destination = i32(sdl::Destination::TitleFromGameOver);
    if (fail_audio) {
        // A device failure must propagate without presenting or tearing down
        // the world; confirmed requests remain queued for the failed frame.
        CHECK(!app.audio_device.stop(99999));
        CHECK(complete_terminal_handoff(app, session) == TerminalHandoff::Failed);
        CHECK(!app.failure.empty()); CHECK(session.Simulating());
        CHECK_EQ(session.AudioOut().size(), audio_kinds.size());
        CHECK_EQ(session.AppliedAudio().size(), applied_before);
        CHECK_EQ(app.frames, before_app_frames);
        CHECK(app.run != nullptr);
        if (pending_title) CHECK_EQ(app.pending_destination, i32(sdl::Destination::TitleFromGameOver));
        session.Stop();
        continue;
    }
    // This is the terminal handoff required after a receive-only callback;
    // no next gameplay input is available or needed.
    CHECK(complete_terminal_handoff(app, session) == TerminalHandoff::Finished);
    CHECK(!session.Simulating());
    CHECK(session.AudioOut().empty());
    CHECK_EQ(session.AppliedAudio().size(), applied_before + audio_kinds.size());
    if (session.AppliedAudio().size() == applied_before + audio_kinds.size()) {
        for (size_t i = 0; i < audio_kinds.size(); ++i) {
            const auto& applied = session.AppliedAudio()[applied_before + i];
            CHECK(applied.observation.kind == audio_kinds[i]);
            CHECK_EQ(applied.frame, 0u);
        }
    }
    // Returning to title also performs its own immediate stop-all.
    CHECK_EQ(app.audio_device.statistics().stopped, stopped_before + (pending_title ? 2 : 1));
    CHECK_EQ(session.MusicApplied(), music_before + 2);
    CHECK_EQ(session.WorldCompleted(), screen == PauseScreen::ExtraEnd);
    CHECK_EQ(app.frames, before_app_frames);
    if (pending_title) {
        CHECK(app.run == nullptr); CHECK(app.title != nullptr);
        CHECK_EQ(app.pending_destination, -1);
    } else {
        CHECK_EQ(app.progress.stage_frame, before_frame);
        CHECK_EQ(app.pause->state.phase, before_phase);
        multiplayer::WorldState after; CHECK(world(app, after));
        CHECK_EQ(after.digest(), before.digest());
    }
    CHECK(complete_terminal_handoff(app, session) == TerminalHandoff::NotReady);
    CHECK(session.AudioOut().empty());
    CHECK_EQ(session.AppliedAudio().size(), applied_before + audio_kinds.size());
    CHECK_EQ(app.audio_device.statistics().stopped, stopped_before + (pending_title ? 2 : 1));
    CHECK_EQ(session.MusicApplied(), music_before + 2);
    CHECK(app.present()); CHECK_EQ(app.frames, before_app_frames);
    if (!pending_title) {
        CHECK_EQ(app.pause->state.phase, before_phase);
        multiplayer::WorldState repeated; CHECK(world(app, repeated));
        CHECK_EQ(repeated.digest(), before.digest());
    }
    session.Stop();
    }
    }
    }
}

int __attribute__((weak)) main() { return run_all("MP production team wipe"); }
