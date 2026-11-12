// Host control state is compared alongside the world at confirmed frames.
// These negative controls keep the battle and dialogue unchanged while
// changing a host field that can alter the next update or scene handoff.
#include "../../harness/test_framework.hpp"
#include "sdl/ApplicationState.hpp"
#include "multiplayer/HostState.hpp"
#include "multiplayer/Session.hpp"
#include "multiplayer/WorldState.hpp"

#include <array>
#include <chrono>
#include <cstdlib>
#include <functional>

using namespace th13;
using namespace th13test;

namespace {
using App = sdl::ApplicationState;
using multiplayer::HostState;
using multiplayer::WorldState;

bool boot(App& app, const char* name) {
    multiplayer::session.Stop();
    const char* assets = std::getenv("TH13_NATIVE_ASSET_ROOT");
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    if (!assets || !root) return false;
    ::setenv("TH13_SAVE_PATH", name, 1);
    if (!app.audio_device.initialize_for_test(assets,
            std::string(root) + "/games/th13/thbgmogg.dat") || !app.initialize(false))
        return false;
    if (!app.configure_multiplayer(2, {0, 1, 0}, 0) || !app.start_game(1)) return false;
    app.pending_destination = -1;
    app.frames = 0;
    app.game_random.reseed_restore(1234);
    app.visual.reseed_restore(1234);
    return app.begin_run();
}

bool frame(App& app, GameInput& input, u32 held) {
    input.update(held);
    return app.update(input, false) && app.commit(60.f) && app.present() && app.failure.empty();
}

bool world(App& app, WorldState& state) {
    return state.capture(app.run->battle, app.visual, nullptr,
        app.run->presentation.get(), app.run.get(), app.run_transition.handle());
}
} // namespace

TEST(host_digest_detects_control_state_mutations) {
    HostState original;
    original.state_fields.loading_registry = std::make_shared<AnmRegistry::SnapshotData>();
    original.state_fields.has_pause = true;
    original.state_fields.fades.push_back({});
    struct Mutation { const char* name; std::function<void(HostState::StateFields&)> apply; };
    const Mutation mutations[] = {
        {"frame identity", [](auto& s) { ++s.frames; }},
        {"destination", [](auto& s) { s.pending_destination = 12; }},
        {"loading gate", [](auto& s) { ++s.pending_load; }},
        {"recorder edge", [](auto& s) { s.record_edge_reset_pending = true; }},
        {"spell stage", [](auto& s) { ++s.spell_time_stage; }},
        {"spell latch", [](auto& s) { s.spell_clock_active = true; }},
        {"spell index", [](auto& s) { ++s.spell_clock_index; }},
        {"transition loader", [](auto& s) { s.stage_transition_loading = true; }},
        {"transition activation", [](auto& s) { s.stage_transition_active = true; }},
        {"startup age", [](auto& s) { ++s.stage_startup_age; }},
        {"transition gate", [](auto& s) { s.transition_gate_active = true; }},
        {"transition clock", [](auto& s) { ++s.transition_gate_frames; }},
        {"selection resources", [](auto& s) { ++s.selection_player.extra_lives; }},
        {"selection threshold", [](auto& s) { ++s.selection_player.life_piece_threshold_index; }},
        {"game over music", [](auto& s) { s.game_over_music = "th13_01.ogg"; }},
        {"loader animation rate", [](auto& s) { s.loading_animation_rate = .5f; }},
        {"loader RNG", [](auto& s) { s.loading_environment.game_rng.next32(); }},
        {"loader camera", [](auto& s) { s.loading_environment.camera_origin.z = 12.f; }},
        {"loader pause", [](auto& s) { s.loading_environment.paused = true; }},
        {"pause existence", [](auto& s) { s.has_pause = false; }},
        {"pause screen", [](auto& s) { s.pause.screen = PauseScreen::Pause; }},
        {"pause previous", [](auto& s) { s.pause.previous = PauseScreen::GameOver; }},
        {"pause phase", [](auto& s) { ++s.pause.phase; }},
        {"pause age", [](auto& s) { s.pause.age.tick(nullptr); }},
        {"pause selection age", [](auto& s) { s.pause.selection_age.tick(nullptr); }},
        {"pause flags", [](auto& s) { s.pause.flags = 1; }},
        {"pause front resource", [](auto& s) { ++s.pause.front_bank; }},
        {"pause practice", [](auto& s) { s.pause.practice = true; }},
        {"pause continue", [](auto& s) { s.pause.can_continue = true; }},
        {"pause replay", [](auto& s) { s.pause.can_save_replay = true; }},
        {"pause options", [](auto& s) { s.pause.can_open_options = true; }},
        {"pause action", [](auto& s) { ++s.pause.selected_action; }},
        {"pause slot", [](auto& s) { ++s.pause.replay_slot; }},
        {"pause name cursor", [](auto& s) { ++s.pause.name_cursor; }},
        {"pause option cursor", [](auto& s) { ++s.pause.option_row; }},
        {"pause name", [](auto& s) { s.pause.replay_name = "test"; }},
        {"pause name mode", [](auto& s) { s.pause.name_is_result = true; }},
        {"pause result editor", [](auto& s) { s.pause.result_needs_name = true; }},
        {"pause saved speed", [](auto& s) { s.pause.saved_rate = .5f; }},
        {"menu cursor", [](auto& s) { ++s.pause.menu.cursor; }},
        {"menu previous cursor", [](auto& s) { ++s.pause.menu.previous; }},
        {"menu count", [](auto& s) { ++s.pause.menu.count; }},
        {"menu cursor stack", [](auto& s) { ++s.pause.menu.history_cursor[1]; }},
        {"menu count stack", [](auto& s) { ++s.pause.menu.history_count[1]; }},
        {"menu disabled entry", [](auto& s) { ++s.pause.menu.disabled[1]; }},
        {"menu stack depth", [](auto& s) { ++s.pause.menu.depth; }},
        {"menu wrapping", [](auto& s) { s.pause.menu.wrapping = true; }},
        {"menu disabled count", [](auto& s) { ++s.pause.menu.disabled_count; }},
        {"pause input press", [](auto& s) { s.pause_frame.input.pressed = 0x100; }},
        {"pause input repeat", [](auto& s) { s.pause_frame.input.repeated = 0x20; }},
        {"pause input focus", [](auto& s) { s.pause_frame.input.lost_focus = true; }},
        {"pause callback", [](auto& s) { s.pause_frame.enabled = true; }},
        {"fade kind", [](auto& s) { s.fades[0].kind = 5; }},
        {"fade duration", [](auto& s) { ++s.fades[0].duration; }},
        {"fade time", [](auto& s) { ++s.fades[0].time; }},
        {"fade alpha", [](auto& s) { ++s.fades[0].alpha; }},
        {"fade clipping", [](auto& s) { s.fades[0].playfield_only = true; }},
    };
    const u64 expected = WorldState::digest_host(original);
    for (const auto& mutation : mutations) {
        HostState altered = original;
        mutation.apply(altered.state_fields);
        const bool detected = WorldState::digest_host(altered) != expected;
        if (!detected) std::printf("  undetected host mutation: %s\n", mutation.name);
        CHECK(detected);
    }
    HostState dialogue = original;
    ++dialogue.dialogue_digest;
    CHECK(WorldState::digest_host(dialogue) != expected);
}

TEST(host_digest_covers_application_capture_and_restore) {
    App app;
    CHECK(boot(app, "host-control-mutation.bin"));
    if (!app.run) return;
    const auto restore = app.capture_multiplayer_host();
    const auto digest = app.digest_multiplayer_host();
    const auto dialogue = app.digest_dialogue_rollback();
    WorldState before;
    CHECK(world(app, before));
    app.pending_destination = i32(sdl::Destination::NextStage);
    app.pending_load = 1;
    ++app.frames;
    app.stage_startup_age = 41;
    app.transition_gate_active = true;
    app.transition_gate_frames = 27;
    app.selection_player.bombs = 1;
    app.pause->state.phase = 7;
    app.pause->frame.controls({0x100, 0x20, false});
    WorldState altered;
    CHECK(world(app, altered));
    CHECK_EQ(altered.digest(), before.digest());
    CHECK_EQ(app.digest_dialogue_rollback(), dialogue);
    CHECK(app.digest_multiplayer_host() != digest);
    CHECK(restore());
    CHECK_EQ(app.digest_multiplayer_host(), digest);
    CHECK_EQ(app.pending_destination, -1);
    CHECK_EQ(app.pending_load, 0u);
    CHECK_EQ(app.pause->frame.capture_rollback().input.pressed, 0u);
}

TEST(host_and_world_restore_replay_future_frames_together) {
    App app;
    CHECK(boot(app, "host-forward-replay.bin"));
    if (!app.run) return;
    GameInput input;
    for (unsigned i = 0; i < 35; ++i) CHECK(frame(app, input, 1));
    CHECK(app.open_game_fade(false, 3, true));
    const auto saved_input = input;
    const auto restore_host = app.capture_multiplayer_host();
    const u64 captured_host = app.digest_multiplayer_host();
    WorldState checkpoint;
    CHECK(world(app, checkpoint));
    std::array<std::array<u64, 2>, 18> expected{};
    for (unsigned i = 0; i < expected.size(); ++i) {
        CHECK(frame(app, input, i < 6 ? 1 : 0x40));
        WorldState actual;
        CHECK(world(app, actual));
        expected[i] = {actual.digest(), app.digest_multiplayer_host()};
    }
    CHECK(app.ending_fades.empty());
    CHECK(checkpoint.restore(app.run->battle, app.visual, nullptr,
        app.run->presentation.get(), app.run.get()));
    CHECK(restore_host());
    CHECK_EQ(app.digest_multiplayer_host(), captured_host);
    CHECK_EQ(app.ending_fades.size(), 1u);
    CHECK(app.scheduler.contains(app.ending_fades[0]->update));
    CHECK(app.scheduler.contains(app.ending_fades[0]->draw));
    input = saved_input;
    for (unsigned i = 0; i < expected.size(); ++i) {
        CHECK(frame(app, input, i < 6 ? 1 : 0x40));
        WorldState actual;
        CHECK(world(app, actual));
        CHECK_EQ(actual.digest(), expected[i][0]);
        CHECK_EQ(app.digest_multiplayer_host(), expected[i][1]);
    }
}

TEST(new_session_discards_local_startup_loader_history) {
    using namespace multiplayer;
    u64 expected_host = 0, expected_world = 0;
    for (unsigned client = 0; client < 2; ++client) {
        session.Stop();
        App app;
        const char* assets = std::getenv("TH13_NATIVE_ASSET_ROOT");
        const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
        CHECK(assets && root);
        if (!assets || !root) return;
        const std::string save = "host-startup-history-" + std::to_string(client) + ".bin";
        ::setenv("TH13_SAVE_PATH", save.c_str(), 1);
        CHECK(app.audio_device.initialize_for_test(assets,
            std::string(root) + "/games/th13/thbgmogg.dat"));
        CHECK(app.initialize(false));
        // Browser startup advances this isolated manager before the title;
        // its duration and RNG history belong to the individual device.
        for (unsigned i = 0; i < client * 73; ++i) CHECK(app.advance_loading_caption());
        for (unsigned i = 0; i < 30; ++i) CHECK(app.advance_loading(.1));
        GameInput idle;
        for (unsigned i = 0; i < client * 37; ++i) CHECK(frame(app, idle, 0));
        AnmRegistry::SnapshotData before;
        CHECK(app.loading_animations.registry.capture(before));
        unsigned active = 0;
        for (const auto& entry : before.entries) active += entry.active;
        std::printf("# client %u startup loader entries=%zu active=%u generation=%u\n",
            client, before.entries.size(), active, before.generation);
        CHECK(!before.entries.empty());
        SessionSetup setup;
        setup.version = 2; setup.mode = 2; setup.seed = 1234;
        setup.session_id = 42; setup.build[0] = 1; setup.automatic = false;
        const auto words = EncodeSessionSetup(setup);
        CHECK(session.Configure(words.data(), words.size()));
        CHECK(session.BeginLocalSimulation());
        CHECK(app.configure_multiplayer(2, {0, 1, 0}, client));
        app.progress.character = 0; app.progress.difficulty = 3;
        CHECK(app.start_game(1));
        app.pending_destination = -1; app.frames = 0;
        app.game_random.reseed_restore(1234); app.visual.reseed_restore(1234);
        CHECK(app.begin_run());
        HostState host;
        CHECK(app.capture_multiplayer_host_state(host));
        const auto& loader = *host.state_fields.loading_registry;
        unsigned remaining = 0;
        for (const auto& entry : loader.entries) remaining += entry.active;
        std::printf("# client %u world-entry loader entries=%zu active=%u generation=%u\n",
            client, loader.entries.size(), remaining, loader.generation);
        CHECK(loader.entries.empty());
        CHECK_EQ(loader.generation, 0u);
        CHECK_EQ(app.loading_signature, 0u); CHECK_EQ(app.loading_prayer, 0u);
        CHECK(app.loading_animations.resource(2) != nullptr);
        WorldState state; CHECK(world(app, state));
        const auto actual_host = app.digest_multiplayer_host();
        if (!client) { expected_host = actual_host; expected_world = state.digest(); }
        else { CHECK_EQ(actual_host, expected_host); CHECK_EQ(state.digest(), expected_world); }
        session.Stop();
    }
}

// This suite also registers its cases in the cumulative m6 binary.
int __attribute__((weak)) main() { return run_all("MP host state digest and restore"); }
