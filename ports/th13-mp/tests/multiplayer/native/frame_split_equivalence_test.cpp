#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "sdl/ApplicationState.hpp"
#include "state_digest.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

using th13::u8;
using th13::u32;
using th13::i32;
using App = th13::sdl::ApplicationState;
using th13::sdl::Destination;

constexpr th13::u16 kBootSeed = 0x1B2C;
constexpr float kFps = 60.0f;
constexpr u32 kZ = 0x01;

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
        std::printf("  失败 boot: audio initialize_for_test: %s\n",
                    app.audio_device.error.c_str());
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
};

bool drive_arm(Arm arm, App& app, th13::GameInput& input, u32 held, ArmFrame& out) {
    input.update(held);
    if (!app.update(input, false)) return false;
    if (!app.commit(kFps)) return false;
    out.after_commit = mpdigest::capture(app);
    const bool barrier = app.pending_destination >= 0;
    const int presents = barrier ? 1 : (arm == Arm::A ? 0 : arm == Arm::B ? 1 : 2);
    for (int i = 0; i < presents; ++i)
        if (!app.present()) return false;
    out.presents = presents;
    out.final = mpdigest::capture(app);
    return app.failure.empty();
}

bool check_arms_equal(const char* window, int frame, const ArmFrame& fa,
                      const ArmFrame& fb, const ArmFrame& fc, bool expect_purity) {
    bool equal = true;
    const auto note = [&](const char* pair, const mpdigest::FrameDigest& lhs,
                          const mpdigest::FrameDigest& rhs) {
        if (!(lhs == rhs)) {
            equal = false;
            std::printf("  失败 %s f%d %s: differs in [%s]\n", window, frame, pair,
                        lhs.diff_against(rhs).c_str());
        }
    };
    note("commit A-vs-B", fa.after_commit, fb.after_commit);
    note("commit B-vs-C", fb.after_commit, fc.after_commit);
    note("final A-vs-B", fa.final, fb.final);
    note("final B-vs-C", fb.final, fc.final);
    if (expect_purity) {
        note("purity B", fb.final, fb.after_commit);
        note("purity C", fc.final, fc.after_commit);
    }
    return equal;
}

}

TEST(equivalence_replay_boot_three_arms_full_digest) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app_a, app_b, app_c;
    CHECK(boot_replay_battle(app_a, rpy, "t2eq-armA-save.bin"));
    CHECK(boot_replay_battle(app_b, rpy, "t2eq-armB-save.bin"));
    CHECK(boot_replay_battle(app_c, rpy, "t2eq-armC-save.bin"));
    if (!app_a.failure.empty() || !app_b.failure.empty() || !app_c.failure.empty() ||
        !app_a.run || !app_b.run || !app_c.run) {
        std::printf("  失败 boot: A='%s' B='%s' C='%s'\n", app_a.failure.c_str(),
                    app_b.failure.c_str(), app_c.failure.c_str());
        return;
    }
    CHECK_EQ(app_a.progress.stage_frame, 0);
    th13::GameInput in_a, in_b, in_c;
    constexpr int kFrames = 32;
    for (int F = 0; F < kFrames; ++F) {
        ArmFrame fa, fb, fc;
        const bool ok_a = drive_arm(Arm::A, app_a, in_a, 0, fa);
        const bool ok_b = drive_arm(Arm::B, app_b, in_b, 0, fb);
        const bool ok_c = drive_arm(Arm::C, app_c, in_c, 0, fc);
        CHECK(ok_a);
        CHECK(ok_b);
        CHECK(ok_c);
        if (!ok_a || !ok_b || !ok_c) {
            std::printf("  失败 arm drive f%d: A='%s' B='%s' C='%s'\n", F,
                        app_a.failure.c_str(), app_b.failure.c_str(), app_c.failure.c_str());
            return;
        }
        CHECK_EQ(app_a.progress.stage_frame, F + 1);
        CHECK_EQ(fa.presents, 0);
        CHECK_EQ(fb.presents, 1);
        CHECK_EQ(fc.presents, 2);
        CHECK_EQ(app_a.pending_destination, -1);
        CHECK_EQ(app_a.pending_load, 0u);
        CHECK(check_arms_equal("replay", F, fa, fb, fc, true));
    }
}

TEST(equivalence_live_run_caption_window_three_arms) {
    App app_a, app_b, app_c;
    CHECK(boot_live_run(app_a, "t2eq-liveA-save.bin"));
    CHECK(boot_live_run(app_b, "t2eq-liveB-save.bin"));
    CHECK(boot_live_run(app_c, "t2eq-liveC-save.bin"));
    if (!app_a.failure.empty() || !app_b.failure.empty() || !app_c.failure.empty()) {
        std::printf("  失败 boot: A='%s' B='%s' C='%s'\n", app_a.failure.c_str(),
                    app_b.failure.c_str(), app_c.failure.c_str());
        return;
    }
    th13::GameInput in_a, in_b, in_c;
    for (int step = 0; step <= 13; ++step) {
        const u32 held = step >= 1 ? kZ : 0u;
        ArmFrame fa, fb, fc;
        const bool ok_a = drive_arm(Arm::A, app_a, in_a, held, fa);
        const bool ok_b = drive_arm(Arm::B, app_b, in_b, held, fb);
        const bool ok_c = drive_arm(Arm::C, app_c, in_c, held, fc);
        CHECK(ok_a);
        CHECK(ok_b);
        CHECK(ok_c);
        if (!ok_a || !ok_b || !ok_c) {
            std::printf("  失败 live arm drive step%d: A='%s' B='%s' C='%s'\n", step,
                        app_a.failure.c_str(), app_b.failure.c_str(), app_c.failure.c_str());
            return;
        }
        if (step == 0) {
            CHECK_EQ(app_a.pending_load, 1u);
            CHECK_EQ(fa.presents, 1);
            CHECK_EQ(fb.presents, 1);
            CHECK_EQ(fc.presents, 1);
            CHECK_EQ(app_b.loading_frames, 0u);
        } else {
            CHECK_EQ(app_b.progress.stage_frame, step);
            CHECK_EQ(fb.presents, 1);
            CHECK_EQ(fc.presents, 2);
            CHECK_EQ(fa.presents, 0);
            CHECK_EQ(app_b.replay_active, false);
            CHECK(app_b.recorder.ready());
        }
        CHECK(check_arms_equal("live", step, fa, fb, fc, step >= 1));
    }
}

int main() { return th13test::run_all("m1.unit_frame_split_equivalence"); }
