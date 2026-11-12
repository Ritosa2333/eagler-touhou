// A retained production engine must start the same world after each room return.
// Capture objects deliberately survive their old scene and its ANM resources.
#include "../../harness/test_framework.hpp"
#include "sdl/ApplicationState.hpp"
#include "multiplayer/Session.hpp"
#include "multiplayer/WorldState.hpp"
#include <chrono>
#include <cstdlib>

using namespace th13;
using namespace th13test;
namespace {
using App = sdl::ApplicationState;

bool boot(App& app, i32 client) {
    const std::string save = "scene-epoch-" + std::to_string(client) + "-" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".bin";
    ::setenv("TH13_SAVE_PATH", save.c_str(), 1);
    const char* assets = std::getenv("TH13_NATIVE_ASSET_ROOT");
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return assets && root && app.audio_device.initialize_for_test(
        assets, std::string(root) + "/games/th13/thbgmogg.dat") && app.initialize(false);
}

bool title_frame(App& app) {
    return app.update(GameInput{}, false) && app.commit(60.f) && app.present() && app.failure.empty();
}
} // namespace

TEST(retained_engine_three_runs_replace_scene_epoch_snapshots) {
    using namespace multiplayer;
    std::array<std::array<u64, 71>, 3> expected{};
    for (i32 client = 0; client < 2; ++client) {
        session.Stop();
        App app;
        CHECK(boot(app, client));
        WorldState reused;
        StagePresentation::SnapshotData reused_presentation;
        for (i32 round = 0; round < 3; ++round) {
            // Unequal title and previous-run lifetimes must not enter a new world.
            for (i32 i = 0; i < client * 37 + round * 13; ++i) CHECK(title_frame(app));
            CHECK(app.configure_multiplayer(2, {0, 0, 0}, client));
            SessionSetup setup;
            setup.version = 2; setup.mode = 2; setup.seed = 1234;
            setup.session_id = 42 + round; setup.build[0] = 1;
            setup.local_player = client; setup.automatic = false;
            const auto words = EncodeSessionSetup(setup);
            CHECK(session.Configure(words.data(), words.size()));
            CHECK(session.BeginLocalSimulation());
            app.progress.run_mode_flags = 0; app.progress.practice_mode = 0;
            app.progress.selected_spell_id = -1;
            app.progress.character = 0; app.progress.difficulty = 1;
            CHECK(app.start_game(1)); app.pending_destination = -1; app.frames = 0;
            app.game_random.reseed_restore(1234); app.visual.reseed_restore(1234);
            CHECK(app.begin_run());
            const auto observe = [&](i32 frame) {
                StagePresentation::SnapshotData fresh_presentation;
                CHECK(app.run->presentation->capture_presentation(reused_presentation));
                CHECK(app.run->presentation->capture_presentation(fresh_presentation));
                CHECK_EQ(reused_presentation.has_player, fresh_presentation.has_player);
                if (!fresh_presentation.has_player) {
                    // Check bindings before digesting, so the regression reports
                    // the dangling old-scene reference without dereferencing it.
                    CHECK(reused_presentation.player.resource == nullptr);
                    CHECK_EQ(reused_presentation.player.source_script, -1);
                    if (reused_presentation.player.resource) return false;
                }
                WorldState fresh;
                CHECK(reused.capture(app.run->battle, app.visual, nullptr,
                    app.run->presentation.get(), app.run.get(), app.run_transition.handle()));
                CHECK(fresh.capture(app.run->battle, app.visual, nullptr,
                    app.run->presentation.get(), app.run.get(), app.run_transition.handle()));
                CHECK_EQ(reused.digest(), fresh.digest());
                CHECK_EQ(reused.digest_core(), fresh.digest_core());
                char a[4096]{}, b[4096]{};
                reused.digest_domains(a, sizeof(a)); fresh.digest_domains(b, sizeof(b));
                CHECK_EQ(std::string(a), std::string(b));
                CHECK_EQ(reused.diagnostic_detail(), fresh.diagnostic_detail());
                if (client == 0) expected[size_t(round)][size_t(frame)] = fresh.digest();
                else CHECK_EQ(fresh.digest(), expected[size_t(round)][size_t(frame)]);
                return true;
            };
            if (!observe(0)) { session.Stop(); return; }
            for (i32 frame = 0; frame < 70 + client * 29; ++frame) {
                CHECK(app.complete_loading());
                CHECK(session.CaptureLocal(0, frame)); session.AdvanceCapturePhase();
                CHECK(session.SubmitRemote(unsigned(1 - client), frame, {}) == Netplay::RemoteInputResult::Accepted);
                CHECK(session.CanSimulate()); CHECK(session.Pump(frame));
                CHECK(app.update(session.MenuInput(), false)); CHECK(app.commit(60.f));
                session.OnFrameSimulated(); CHECK(app.present());
                if (frame < 70 && !observe(frame + 1)) { session.Stop(); return; }
            }
            CHECK(session.FinishWorldSimulation(false)); session.Stop();
            app.mp_seat_count = 0; app.mp_characters = {0, 0, 0}; app.mp_local_seat = 0;
            app.exiting = false; app.pending_destination = -1;
            CHECK(app.begin_title(1)); CHECK(app.title != nullptr); CHECK(app.run == nullptr);
        }
        session.Stop();
    }
}

int __attribute__((weak)) main() { return run_all("MP retained-engine scene epochs"); }
