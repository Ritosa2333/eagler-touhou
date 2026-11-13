// R-G: clearing a stage revives exhausted seats before the next HUD is built.
#include "../../harness/test_framework.hpp"
#include "sdl/ApplicationState.hpp"
#include "multiplayer/Session.hpp"
#include "multiplayer/StageEntry.hpp"

#include <cstdlib>

using namespace th13;
using namespace th13test;

namespace {
bool boot(sdl::ApplicationState& app, const char* name, i32 seats = 3,
          std::array<i32, 3> characters = {0, 1, 3}) {
    multiplayer::session.Stop();
    const char* assets = std::getenv("TH13_NATIVE_ASSET_ROOT");
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    if (!assets || !root) return false;
    ::setenv("TH13_SAVE_PATH", name, 1);
    if (!app.audio_device.initialize_for_test(assets,
            std::string(root) + "/games/th13/thbgmogg.dat") || !app.initialize(false))
        return false;
    app.progress.difficulty = 3;
    app.progress.character = characters[0];
    if (!app.configure_multiplayer(seats, characters, 0) || !app.start_game(1)) return false;
    app.pending_destination = -1;
    app.frames = 0;
    app.game_random.reseed_restore(1234);
    app.visual.reseed_restore(1234);
    return app.begin_run();
}

void cross_stage(unsigned exhausted_seat, bool ghost, const char* name, bool rewarded = false) {
    sdl::ApplicationState app;
    CHECK(boot(app, name));
    if (!app.run) return;
    auto& old = app.run->battle;
    auto& exhausted = old.seats[exhausted_seat];
    exhausted.ghost = ghost;
    exhausted.dead = ghost;
    exhausted.phase = StagePlayerState::Phase::Dying;
    exhausted.phase_frame = ghost ? 31 : 17;
    exhausted.invulnerable = 143;
    exhausted.final_death = rewarded;
    auto& hud = old.seat_huds[exhausted_seat];
    const i32 stock = rewarded ? 1 : -1;
    // CHECK_EQ binds references; materialize max() to avoid retaining its
    // reference to a temporary zero when stock is negative.
    const i32 expected_stock = std::max(0, stock);
    hud.extra_lives = stock;
    hud.life_pieces = 2;
    hud.life_piece_threshold_index = 4;
    hud.bombs = 0;
    hud.bomb_pieces = 7;
    hud.power = 115;
    hud.deaths = 5;
    const unsigned survivor = (exhausted_seat + 1) % 3;
    old.seat_huds[survivor].extra_lives = 4;
    old.seat_huds[survivor].bombs = 3;
    old.seat_huds[survivor].power = 315;
    old.seat_huds[survivor].life_piece_threshold_index = 2;
    // Raw chapter snapshots must preserve ghosts and the in-flight death;
    // only the explicit stage-clear activation is allowed to revive them.
    multiplayer::StageEntry raw;
    raw.Capture(old, app.progress, app.selection_score, 1234, 0, 1234, 0);
    CHECK_EQ(raw.seats[exhausted_seat].extra_lives, stock);
    CHECK_EQ(raw.seats[exhausted_seat].ghost, ghost);
    const auto bytes = raw.Encode();
    multiplayer::StageEntry decoded;
    CHECK(multiplayer::StageEntry::Decode(bytes.data(), bytes.size(), decoded));
    CHECK_EQ(decoded.seats[exhausted_seat].extra_lives, stock);
    CHECK_EQ(decoded.seats[exhausted_seat].ghost, ghost);
    decoded.RestoreBattleCarry(old);
    CHECK_EQ(old.seat_huds[exhausted_seat].extra_lives, stock);
    CHECK_EQ(old.seats[exhausted_seat].ghost, ghost);

    app.progress.stage = 2;
    CHECK(app.prepare_next_stage());
    const bool activated = app.activate_next_stage();
    if (!activated) std::printf("# stage activation failure: %s\n", app.failure.c_str());
    CHECK(activated);
    if (!activated) return;
    const auto& next = app.run->battle;
    CHECK_EQ(app.progress.stage, 2);
    CHECK_EQ(next.seat_count, 3);
    CHECK(!next.seats[exhausted_seat].ghost);
    CHECK(!next.seats[exhausted_seat].dead);
    CHECK(next.seats[exhausted_seat].phase == StagePlayerState::Phase::Active);
    CHECK_EQ(next.seats[exhausted_seat].phase_frame, 0);
    CHECK_EQ(next.seats[exhausted_seat].invulnerable, 120);
    CHECK(!next.seats[exhausted_seat].final_death);
    CHECK_EQ(next.seat_huds[exhausted_seat].extra_lives, expected_stock);
    CHECK_EQ(next.seat_huds[exhausted_seat].bombs, 2);
    CHECK_EQ(next.seat_huds[exhausted_seat].bomb_pieces, 0);
    CHECK_EQ(next.seat_huds[exhausted_seat].power, next.seat_huds[exhausted_seat].max_power / 2);
    CHECK_EQ(next.seat_huds[exhausted_seat].life_pieces, 2);
    CHECK_EQ(next.seat_huds[exhausted_seat].life_piece_threshold_index, 4);
    CHECK_EQ(next.seat_huds[exhausted_seat].deaths, 5);
    CHECK_EQ(next.seat_huds[survivor].extra_lives, 4);
    CHECK_EQ(next.seat_huds[survivor].bombs, 3);
    CHECK_EQ(next.seat_huds[survivor].power, 315);
    CHECK_EQ(next.seat_huds[survivor].life_piece_threshold_index, 2);
    CHECK_EQ(app.selection_player.extra_lives, next.seat_huds[0].extra_lives);
    CHECK_EQ(app.selection_player.bombs, next.seat_huds[0].bombs);
    CHECK_EQ(app.selection_player.power, next.seat_huds[0].power);
    CHECK(app.failure.empty());
    GameInput input;
    for (unsigned frame = 0; frame < 35; ++frame) {
        input.update(1);
        CHECK(app.update(input, false) && app.commit(60.f) && app.present());
        CHECK(!next.seats[exhausted_seat].ghost);
        CHECK(!next.seats[exhausted_seat].dead);
        CHECK_EQ(next.seat_huds[exhausted_seat].extra_lives, expected_stock);
    }
}
}

TEST(stage_clear_revives_first_seat_ghost_before_hud) {
    cross_stage(0, true, "stage-clear-ghost.bin");
}
TEST(stage_clear_revives_inflight_last_death_before_hud) {
    cross_stage(0, false, "stage-clear-dying.bin");
}
TEST(stage_clear_revives_remote_ghost_preserving_survivors) {
    cross_stage(2, true, "stage-clear-remote.bin");
}
TEST(stage_clear_revives_remote_inflight_last_death) {
    cross_stage(1, false, "stage-clear-remote-dying.bin");
    cross_stage(1, false, "stage-clear-remote-reward.bin", true);
    cross_stage(0, false, "stage-clear-first-reward.bin", true);
}

TEST(marisa_beams_stop_after_stage_clear_reuses_shot_generations) {
    struct Dialogue final : StageDialogHost {
        bool active = false;
        void open_dialog(i32) override {}
        bool dialog_active() const override { return active; }
    };
    for (i32 count : {2, 3}) {
        Dialogue dialogue;
        sdl::ApplicationState app;
        CHECK(boot(app, "stage-clear-marisa.bin", count, {1, 1, 1}));
        if (!app.run) return;
        GameInput input;
        const auto step = [&](u32 held) {
            input.update(held);
            for (i32 seat = 1; seat < count; ++seat)
                app.run->battle.seat_inputs[size_t(seat)].update(held);
            return app.update(input, false) && app.commit(60.f) && app.present();
        };
        const auto frames = [&](u32 held, i32 count) {
            for (i32 frame = 0; frame < count; ++frame)
                if (!step(held)) return false;
            return true;
        };
        const auto set_power = [&] {
            for (i32 seat = 0; seat < count; ++seat)
                app.run->battle.seat_huds[size_t(seat)].power = 400;
        };
        const auto beam_vms = [&](i32 script) {
            i32 total = 0;
            for (u32 i = 0; const u32 handle = app.animations.registry.ordered_handle(false, i); ++i) {
                const auto* vm = app.animations.registry.find(handle);
                if (vm && vm->shot_effect_owner && vm->source_script == script) ++total;
            }
            return total;
        };
        const auto shots = [&] {
            i32 total = 0;
            for (i32 seat = 0; seat < count; ++seat)
                for (const auto& shot : app.run->battle.seats[size_t(seat)].shots)
                    if (shot.behavior == 2) ++total;
            return total;
        };
        set_power();
        CHECK(frames(0, 40));
        CHECK(frames(1, 60));
        CHECK_EQ(beam_vms(13), 4 * count);
        CHECK(frames(9, 30));
        CHECK_EQ(beam_vms(13), 0);

        app.progress.stage = 2;
        CHECK(app.prepare_next_stage());
        const bool activated = app.activate_next_stage();
        CHECK(activated);
        if (!activated) return;
        auto& battle = app.run->battle;
        battle.sim.dialog_host = &dialogue;
        CHECK(frames(0, 40));
        for (i32 cycle = 0; cycle < 3; ++cycle) {
            set_power();
            // Keep stage hazards from interrupting the scripted firing cycles.
            for (i32 seat = 0; seat < count; ++seat)
                battle.seats[size_t(seat)].invulnerable = 180;
            CHECK(frames(1, 60));
            CHECK_EQ(beam_vms(13), 4 * count);
            dialogue.active = cycle == 2;
            CHECK(frames(cycle == 0 ? 9 : cycle == 1 ? 0 : 1, 40));
            CHECK_EQ(beam_vms(13), 0);
            CHECK_EQ(beam_vms(14), 0);
            CHECK_EQ(shots(), 0);
        }
    }
}

int __attribute__((weak)) main() { return run_all("MP stage-clear carry"); }
