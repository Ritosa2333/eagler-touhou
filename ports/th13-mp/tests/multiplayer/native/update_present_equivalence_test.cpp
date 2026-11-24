#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "sdl/ApplicationState.hpp"
#include "state_digest.hpp"
#include "multiplayer/WorldState.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

using th13::u8;
using th13::u16;
using th13::u32;
using th13::i32;
using App = th13::sdl::ApplicationState;
using th13::sdl::Destination;
using th13::GameInput;

constexpr u16 kBootSeed = 0x1B2C;
constexpr float kFps = 60.0f;
constexpr u32 kZ = 0x01;
constexpr u32 kEsc = 0x100;

const char* repo_root() {
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return root && *root ? root : "../../..";
}

bool read_repo_file(const std::string& relative, std::vector<u8>& out) {
    std::FILE* file = std::fopen((std::string(repo_root()) + "/" + relative).c_str(), "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END);
    const long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    out.resize(size_t(size < 0 ? 0 : size));
    const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
    std::fclose(file);
    return got == out.size();
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

bool boot_replay_battle(App& app, const std::vector<u8>& rpy, const char* save_name) {
    if (!pin_boot(app, save_name)) return false;
    if (!app.draw_loading(false)) {
        std::printf("  失败 boot: draw_loading: %s\n", app.failure.c_str());
        return false;
    }
    for (unsigned frame = 0; frame < App::kDebugLoadingFrames; ++frame)
        if (!app.advance_loading_caption()) {
            std::printf("  失败 boot: advance_loading_caption: %s\n", app.failure.c_str());
            return false;
        }
    if (!app.begin_replay_run(rpy.data(), rpy.size(), 0)) {
        std::printf("  失败 boot: begin_replay_run: %s\n", app.failure.c_str());
        return false;
    }
    return app.run != nullptr && app.failure.empty();
}

bool boot_live_run(App& app, const char* save_name) {
    if (!pin_boot(app, save_name)) return false;
    if (!app.start_game(1)) {
        std::printf("  失败 boot: start_game: %s\n", app.failure.c_str());
        return false;
    }
    return app.pending_destination == i32(Destination::Run) && app.failure.empty();
}

enum class Arm { A, B, C };

struct ArmFrame {
    mpdigest::FrameDigest after_commit;
    mpdigest::FrameDigest final;
    int presents = 0;
    bool barrier = false;
    bool ok = false;
};

bool drive_arm(Arm arm, App& app, GameInput& input, u32 held, ArmFrame& out) {
    input.update(held);
    out.ok = false;
    out.barrier = false;
    if (!app.update(input, false)) return false;
    if (!app.commit(kFps)) return false;
    out.after_commit = mpdigest::capture(app);
    out.barrier = app.pending_destination >= 0;
    const int presents = out.barrier ? 1 : (arm == Arm::A ? 0 : arm == Arm::B ? 1 : 2);
    for (int i = 0; i < presents; ++i)
        if (!app.present()) return false;
    out.presents = presents;
    out.final = mpdigest::capture(app);
    out.ok = app.failure.empty();
    return out.ok;
}

void report_digest_mismatch(const char* window, int frame, const char* point,
                            const mpdigest::FrameDigest& lhs, const mpdigest::FrameDigest& rhs) {
    std::printf("  失败 digest %s f%d %s: differs in [%s]\n", window, frame, point,
                lhs.diff_against(rhs).c_str());
}

bool drive_sync(App& app, GameInput& input, u32 held, mpdigest::FrameDigest& out_final) {
    input.update(held);
    if (!app.update(input, false)) return false;
    if (!app.commit(kFps)) return false;
    if (!app.present()) return false;
    out_final = mpdigest::capture(app);
    return app.failure.empty();
}

bool assert_arms_equal(const char* window, int frame, const ArmFrame& fa,
                       const ArmFrame& fb, const ArmFrame& fc) {
    if (!fa.ok || !fb.ok || !fc.ok) {
        std::printf("  失败 %s f%d: arm drive ok=(%d,%d,%d)\n", window, frame,
                    fa.ok ? 1 : 0, fb.ok ? 1 : 0, fc.ok ? 1 : 0);
        return false;
    }
    CHECK(fa.barrier || (!fa.barrier && fa.presents == 0));
    CHECK(fb.barrier || fb.presents == 1);
    CHECK(fc.barrier || fc.presents == 2);
    if (fa.barrier) {
        CHECK_EQ(fa.presents, 1);
        CHECK_EQ(fb.presents, 1);
        CHECK_EQ(fc.presents, 1);
    }
    bool equal = true;
    if (!(fa.after_commit == fb.after_commit)) {
        report_digest_mismatch(window, frame, "after_commit A-vs-B", fa.after_commit, fb.after_commit);
        equal = false;
    }
    if (!(fb.after_commit == fc.after_commit)) {
        report_digest_mismatch(window, frame, "after_commit B-vs-C", fb.after_commit, fc.after_commit);
        equal = false;
    }
    if (!(fa.final == fb.final)) {
        report_digest_mismatch(window, frame, "final A-vs-B", fa.final, fb.final);
        equal = false;
    }
    if (!(fb.final == fc.final)) {
        report_digest_mismatch(window, frame, "final B-vs-C", fb.final, fc.final);
        equal = false;
    }
    CHECK(equal);
    if (!fa.barrier) {
        const bool pure_b = fb.after_commit == fb.final;
        const bool pure_c = fc.after_commit == fc.final;
        if (!pure_b) report_digest_mismatch(window, frame, "purity B", fb.after_commit, fb.final);
        if (!pure_c) report_digest_mismatch(window, frame, "purity C", fc.after_commit, fc.final);
        CHECK(pure_b);
        CHECK(pure_c);
    }
    return equal;
}

}

TEST(update_present_equivalence_caption_window_f0_10) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app_a, app_b, app_c;
    CHECK(boot_replay_battle(app_a, rpy, "t7upe-capA-save.bin"));
    CHECK(boot_replay_battle(app_b, rpy, "t7upe-capB-save.bin"));
    CHECK(boot_replay_battle(app_c, rpy, "t7upe-capC-save.bin"));
    if (app_a.failure.empty() && app_b.failure.empty() && app_c.failure.empty() &&
        app_a.run && app_b.run && app_c.run) {
        CHECK_EQ(app_a.progress.stage_frame, 0);
        CHECK(app_a.loading_prayer != 0);
        GameInput in_a, in_b, in_c;
        for (int F = 0; F <= 11; ++F) {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK(!fa.barrier);
            if (fa.barrier) return;
            if (!assert_arms_equal("caption", F, fa, fb, fc)) return;
            CHECK_EQ(fa.after_commit.loading, fb.after_commit.loading);
            CHECK_EQ(fb.after_commit.loading, fc.after_commit.loading);
            CHECK_EQ(fa.final.loading, fb.final.loading);
            CHECK_EQ(fb.final.loading, fc.final.loading);
        }
    }
}

TEST(update_present_equivalence_run_baffle_and_caption_live) {
    App app_a, app_b, app_c;
    CHECK(boot_live_run(app_a, "t7upe-liveA-save.bin"));
    CHECK(boot_live_run(app_b, "t7upe-liveB-save.bin"));
    CHECK(boot_live_run(app_c, "t7upe-liveC-save.bin"));
    if (app_a.failure.empty() && app_b.failure.empty() && app_c.failure.empty()) {
        GameInput in_a, in_b, in_c;
        {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK(fa.barrier);
            CHECK_EQ(app_b.pending_load, 1u);
            CHECK_EQ(app_b.loading_frames, 0u);
            CHECK(app_b.loading_prayer != 0);
            CHECK_EQ(app_b.pending_destination, i32(Destination::Run));
            if (!assert_arms_equal("baffle", 0, fa, fb, fc)) return;
        }
        for (int step = 1; step <= 12; ++step) {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, kZ, fa) &&
                            drive_arm(Arm::B, app_b, in_b, kZ, fb) &&
                            drive_arm(Arm::C, app_c, in_c, kZ, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK_EQ(app_b.progress.stage_frame, step);
            CHECK_EQ(app_b.pending_load, 0u);
            CHECK(!fa.barrier);
            if (fa.barrier) return;
            if (!assert_arms_equal("baffle-live", step, fa, fb, fc)) return;
            if (step <= 11) {
                CHECK_EQ(fa.final.loading, fb.final.loading);
                CHECK_EQ(fb.final.loading, fc.final.loading);
            }
        }
        CHECK(app_b.replay_active == false);
        CHECK(app_b.recorder.ready());
    }
}

TEST(update_present_equivalence_transition_gate) {
    App app_a, app_b, app_c;
    CHECK(boot_live_run(app_a, "t7upe-gateA-save.bin"));
    CHECK(boot_live_run(app_b, "t7upe-gateB-save.bin"));
    CHECK(boot_live_run(app_c, "t7upe-gateC-save.bin"));
    if (app_a.failure.empty() && app_b.failure.empty() && app_c.failure.empty()) {
        GameInput in_a, in_b, in_c;
        {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK(fa.barrier);
            if (!assert_arms_equal("gate-pre", 0, fa, fb, fc)) return;
        }
        for (int step = 1; step <= 5; ++step) {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            if (!assert_arms_equal("gate-pre", step, fa, fb, fc)) return;
        }
        app_a.pending_destination = i32(Destination::NextStage);
        app_b.pending_destination = i32(Destination::NextStage);
        app_c.pending_destination = i32(Destination::NextStage);
        {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK(fa.barrier);
            CHECK(app_b.stage_transition_loading);
            CHECK(app_b.run != nullptr);
            CHECK_EQ(app_b.pending_destination, -1);
            if (!assert_arms_equal("gate-dest", -1, fa, fb, fc)) return;
        }
        {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK(!fa.barrier);
            CHECK(app_b.stage_transition_active);
            if (!assert_arms_equal("gate-door", 0, fa, fb, fc)) return;
        }
        for (int i = 1; i <= 32; ++i) {
            mpdigest::FrameDigest fa, fb, fc;
            const bool ok = drive_sync(app_a, in_a, 0, fa) &&
                            drive_sync(app_b, in_b, 0, fb) &&
                            drive_sync(app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK(app_b.run != nullptr);
            CHECK(!app_b.stage_transition_loading);
            if (!(fa == fb) || !(fb == fc)) {
                std::printf("  失败 gate-fade i%d: A-vs-B[%s] B-vs-C[%s]\n", i,
                            fa.diff_against(fb).c_str(), fb.diff_against(fc).c_str());
                CHECK(false);
                return;
            }
            CHECK(true);
            const bool geometry = app_b.run->scenery->draw_geometry_enabled();
            if (i >= 1 && i <= 31) {
                if (geometry) {
                    std::printf("  失败 gate-fade i%d: 倒计时中几何位已恢复（时序漂移）\n", i);
                }
                CHECK(!geometry);
            } else if (i == 32) {
                if (!geometry) {
                    std::printf("  失败 gate-fade i%d: 零穿越后几何位未恢复\n", i);
                }
                CHECK(geometry);
            }
            if (i == 31) {
                CHECK(!app_b.stage_transition_active);
                CHECK(!app_b.transition_gate_active);
            }
        }
        for (int i = 33; i <= 41; ++i) {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK(!fa.barrier);
            if (fa.barrier) return;
            CHECK(app_b.run->scenery->draw_geometry_enabled());
            if (!assert_arms_equal("gate-post", i, fa, fb, fc)) return;
        }
        CHECK(app_b.progress.stage_frame >= 1);
        CHECK(app_b.run != nullptr);
    }
}

TEST(update_present_equivalence_transition_gate_present_coupling) {
    App arms[3];
    GameInput inputs[3];
    for (int arm=0;arm<3;++arm) {
        CHECK(boot_live_run(arms[arm], ("t7upe-present-"+std::to_string(arm)+".bin").c_str()));
        for(int frame=0;frame<=5;++frame) {
            mpdigest::FrameDigest value;
            CHECK(drive_sync(arms[arm],inputs[arm],0,value));
        }
        arms[arm].pending_destination=i32(Destination::NextStage);
        for(int frame=0;frame<2;++frame) {
            mpdigest::FrameDigest value;
            CHECK(drive_sync(arms[arm],inputs[arm],0,value));
        }
        CHECK(arms[arm].stage_transition_active);
    }
    auto full=[](App& app) {
        th13::multiplayer::WorldState value;
        CHECK(app.run!=nullptr);
        CHECK(value.capture(app.run->battle,app.visual,nullptr,
            app.run->presentation.get(),app.run.get(),app.run_transition.handle()));
        return value.digest();
    };
    for(int frame=1;frame<=45;++frame) {
        uint64_t values[3]{};
        for(int arm=0;arm<3;++arm) {
            inputs[arm].update(0);
            CHECK(arms[arm].update(inputs[arm],false));
            CHECK(arms[arm].commit(kFps));
            const auto before=full(arms[arm]);
            for(int present=0;present<arm;++present) CHECK(arms[arm].present());
            values[arm]=full(arms[arm]);
            CHECK_EQ(before,values[arm]);
            CHECK(arms[arm].failure.empty());
            if(frame==1) {
                CHECK(arms[arm].run->scenery!=nullptr);
                CHECK_EQ(arms[arm].run->scenery->capture_extra().lifecycle_timer.current,59);
                CHECK(!arms[arm].run->scenery->draw_geometry_enabled());
            }
            if(frame==30) {
                CHECK(arms[arm].run->previous_scenery!=nullptr);
                CHECK(arms[arm].run->previous_scenery->retired());
            }
            if(frame==31) CHECK(arms[arm].run->previous_scenery==nullptr);
            if(frame==32) CHECK(arms[arm].run->scenery->draw_geometry_enabled());
        }
        CHECK_EQ(values[0],values[1]);CHECK_EQ(values[1],values[2]);
        CHECK(mpdigest::capture(arms[0])==mpdigest::capture(arms[1]));
        CHECK(mpdigest::capture(arms[1])==mpdigest::capture(arms[2]));
    }
}

TEST(update_present_equivalence_pause_open_close) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app_a, app_b, app_c;
    CHECK(boot_replay_battle(app_a, rpy, "t7upe-pauseA-save.bin"));
    CHECK(boot_replay_battle(app_b, rpy, "t7upe-pauseB-save.bin"));
    CHECK(boot_replay_battle(app_c, rpy, "t7upe-pauseC-save.bin"));
    if (app_a.failure.empty() && app_b.failure.empty() && app_c.failure.empty() &&
        app_a.run && app_b.run && app_c.run) {
        GameInput in_a, in_b, in_c;
        for (int F = 0; F < 16; ++F) {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok || !assert_arms_equal("pause-pre", F, fa, fb, fc)) return;
        }
        CHECK(!app_b.pause->active());
        const i32 frozen_at = app_b.progress.stage_frame;
        {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, kEsc, fa) &&
                            drive_arm(Arm::B, app_b, in_b, kEsc, fb) &&
                            drive_arm(Arm::C, app_c, in_c, kEsc, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK(app_b.pause->active());
            CHECK(app_b.pause->state.screen == th13::PauseScreen::Pause);
            CHECK(app_b.environment.paused == true);
            CHECK_EQ(app_b.progress.stage_frame, frozen_at);
            if (!assert_arms_equal("pause-open", 0, fa, fb, fc)) return;
        }
        for (int F = 1; F <= 24; ++F) {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK(app_b.pause->active());
            CHECK(app_b.environment.paused == true);
            CHECK_EQ(app_b.progress.stage_frame, frozen_at);
            if (!assert_arms_equal("pause-hold", F, fa, fb, fc)) return;
        }
        int wait = 0;
        while (app_b.pause->state.phase != 6 && wait < 20) {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok || !assert_arms_equal("pause-wait", wait, fa, fb, fc)) return;
            ++wait;
        }
        CHECK_EQ(app_b.pause->state.phase, 6);
        {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, kEsc, fa) &&
                            drive_arm(Arm::B, app_b, in_b, kEsc, fb) &&
                            drive_arm(Arm::C, app_c, in_c, kEsc, fc);
            CHECK(ok);
            if (!ok) return;
            if (!assert_arms_equal("pause-close", 0, fa, fb, fc)) return;
        }
        bool resumed = app_b.environment.paused == false;
        for (int F = 0; F < 40 && !resumed; ++F) {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok || !assert_arms_equal("pause-close", F + 1, fa, fb, fc)) return;
            resumed = app_b.environment.paused == false;
        }
        CHECK(resumed);
        CHECK(!app_b.pause->active());
        CHECK_EQ(app_b.progress.scene_flags & 0x10u, 0u);
        for (int F = 0; F < 24; ++F) {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok || !assert_arms_equal("pause-post", F, fa, fb, fc)) return;
        }
        CHECK(app_b.progress.stage_frame > frozen_at);
    }
}

TEST(update_present_equivalence_long_windows_600_1200) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app_a, app_b, app_c;
    CHECK(boot_replay_battle(app_a, rpy, "t7upe-longA-save.bin"));
    CHECK(boot_replay_battle(app_b, rpy, "t7upe-longB-save.bin"));
    CHECK(boot_replay_battle(app_c, rpy, "t7upe-longC-save.bin"));
    if (app_a.failure.empty() && app_b.failure.empty() && app_c.failure.empty() &&
        app_a.run && app_b.run && app_c.run) {
        GameInput in_a, in_b, in_c;
        int barriers = 0;
        for (int F = 0; F < 1200; ++F) {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            if (fa.barrier) ++barriers;
            if (!assert_arms_equal("long", F, fa, fb, fc)) return;
            if (F == 299 || F == 599 || F == 1199) {
                CHECK(app_b.progress.stage_frame > 0);
                CHECK(app_b.replay_consumed > 0);
            }
        }
        CHECK_EQ(barriers, 0);
        CHECK_EQ(app_a.frames, 1200u);
        CHECK_EQ(app_b.frames, 1200u);
        CHECK_EQ(app_c.frames, 1200u);
    }
}

int main() { return th13test::run_all("m1.update_present_equivalence"); }
