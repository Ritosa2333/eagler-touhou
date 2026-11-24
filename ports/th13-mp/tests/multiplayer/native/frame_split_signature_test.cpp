#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "sdl/ApplicationState.hpp"
#include "state_digest.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <type_traits>
#include <vector>

namespace {

using th13::u8;
using th13::u16;
using th13::i32;
using th13::u32;
using App = th13::sdl::ApplicationState;
using th13::sdl::Destination;

constexpr u16 kBootSeed = 0x1B2C;
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
    bool ok = false;
};

bool drive_arm(Arm arm, App& app, th13::GameInput& input, u32 held, ArmFrame& out) {
    input.update(held);
    out.ok = false;
    if (!app.update(input, false)) return false;
    if (!app.commit(kFps)) return false;
    out.after_commit = mpdigest::capture(app);
    const bool barrier = app.pending_destination >= 0;
    const int presents = barrier ? 1 : (arm == Arm::A ? 0 : arm == Arm::B ? 1 : 2);
    for (int i = 0; i < presents; ++i)
        if (!app.present()) return false;
    out.presents = presents;
    out.final = mpdigest::capture(app);
    out.ok = app.failure.empty();
    return out.ok;
}

void report_mismatch(const char* window, int frame, const char* point,
                     const mpdigest::FrameDigest& lhs, const mpdigest::FrameDigest& rhs) {
    std::printf("  失败 digest %s f%d %s: differs in [%s]\n", window, frame, point,
                lhs.diff_against(rhs).c_str());
}

struct DrawProbe {
    th13::FrameCallback callback{};
    App* app = nullptr;
    th13::FrameScheduler* attached_scheduler = nullptr;
    int fires = 0;
    bool observed_ok = false;
    static i32 run(void* p) {
        auto& probe = *static_cast<DrawProbe*>(p);
        ++probe.fires;
        probe.observed_ok = true;
        probe.callback.enabled = false;
        return i32(th13::FrameAction::Continue);
    }
    bool attach(App& target) {
        app = &target;
        callback.owner = this;
        callback.enabled = true;
        callback.run = &DrawProbe::run;
        if (target.scheduler.add(callback, th13::FramePass::Draw, 73) < 0) return false;
        attached_scheduler = &target.scheduler;
        return true;
    }
    void detach() {
        if (attached_scheduler) attached_scheduler->remove(callback);
        attached_scheduler = nullptr;
        app = nullptr;
    }
    ~DrawProbe() { detach(); }
};

}

TEST(split_segments_update_failure_short_circuit) {
    {
        App fresh;
        th13::GameInput input;
        input.update(0);
        CHECK_EQ(fresh.initialized, false);
        CHECK(!fresh.update(input, false));
        CHECK(!fresh.step(input, kFps, false));
        CHECK_EQ(fresh.frames, 0u);
        CHECK(fresh.failure.empty());
    }
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    DrawProbe probe;
    App app;
    CHECK(boot_replay_battle(app, rpy, "t2sig-shortcircuit-save.bin"));
    if (!app.failure.empty() || !app.run) return;
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    CHECK_EQ(app.frames, 1u);
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    CHECK_EQ(app.frames, 2u);
    const auto stats_before = app.audio_device.statistics();
    CHECK(probe.attach(app));
    app.failure = "t2 signature short-circuit injection";
    input.update(0);
    CHECK(!app.update(input, false));
    CHECK(!app.step(input, kFps, false));
    CHECK_EQ(app.frames, 2u);
    CHECK_EQ(probe.fires, 0);
    CHECK_EQ(app.failure, std::string("t2 signature short-circuit injection"));
    const auto stats_after = app.audio_device.statistics();
    CHECK_EQ(stats_after.requests, stats_before.requests);
    CHECK_EQ(stats_after.started, stats_before.started);
    probe.detach();
}

TEST(caption_commit_phase_replay_window) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app;
    CHECK(boot_replay_battle(app, rpy, "t2sig-caption-save.bin"));
    if (!app.failure.empty() || !app.run) return;
    CHECK_EQ(app.progress.stage_frame, 0);
    CHECK_EQ(app.replay_consumed, 0u);
    CHECK(app.loading_prayer != 0);
    const uint64_t d_boot = mpdigest::digest_loading_caption(app);
    const th13::AnmVm* fore14_boot = mpdigest::find_fore_vm(app, 14);
    CHECK(fore14_boot != nullptr);
    if (!fore14_boot) return;

    uint64_t d_commit_prev = d_boot;
    for (int F = 0; F <= 11; ++F) {
        th13::GameInput input;
        input.update(0);
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
        CHECK_EQ(app.progress.stage_frame, F + 1);
        const int battle_frame = mpdigest::battle_frame_of(app);
        CHECK_EQ(battle_frame, F);
        const uint64_t d_pre = mpdigest::digest_loading_caption(app);
        const th13::AnmVm* fore14_pre = mpdigest::find_fore_vm(app, 14);
        CHECK(fore14_pre != nullptr);
        if (!fore14_pre) return;
        const i32 timer_pre = fore14_pre->timer.current;
        const u32 idx_pre = app.visual.index();
        const u32 prayer_pre = app.loading_prayer;
        CHECK(app.commit(kFps));
        if (!app.failure.empty()) return;
        const uint64_t d_commit = mpdigest::digest_loading_caption(app);
        const th13::AnmVm* fore14_commit = mpdigest::find_fore_vm(app, 14);
        CHECK(fore14_commit != nullptr);
        if (!fore14_commit) return;
        const i32 timer_commit = fore14_commit->timer.current;
        const u32 idx_commit = app.visual.index();
        const u32 prayer_commit = app.loading_prayer;
        CHECK(app.present());
        if (!app.failure.empty()) return;
        const uint64_t d_final = mpdigest::digest_loading_caption(app);
        const th13::AnmVm* fore14_final = mpdigest::find_fore_vm(app, 14);
        const i32 timer_final = fore14_final ? fore14_final->timer.current : timer_commit;

        if (d_pre != d_commit_prev) {
            std::printf("  失败 caption 相位 f%d: update 段后 loading digest 相对上一 commit 变化（tick 泄漏进 update 段）\n", F);
        }
        CHECK_EQ(d_pre, d_commit_prev);
        if (F == 0) {
            CHECK_EQ(d_commit, d_pre);
            CHECK_EQ(timer_commit, timer_pre);
            CHECK_EQ(prayer_commit, prayer_pre);
            CHECK_EQ(d_final, d_commit);
        } else if (F >= 1 && F <= 9) {
            if (d_commit == d_pre) {
                std::printf("  失败 caption 相位 f%d: commit 未 tick 装载树（tick 未迁入 commit）\n", F);
            }
            CHECK(d_commit != d_pre);
            CHECK_EQ(timer_commit, timer_pre + 1);
            CHECK_EQ(idx_commit, idx_pre);
            if (d_final != d_commit) {
                std::printf("  失败 caption 相位 f%d: present 后 loading digest 变化（tick/retire 留在 Draw72 的直接反例）\n", F);
            }
            CHECK_EQ(d_final, d_commit);
            CHECK_EQ(timer_final, timer_commit);
            CHECK_EQ(prayer_commit, prayer_pre);
        } else if (F == 10) {
            CHECK(prayer_pre != 0);
            CHECK_EQ(prayer_commit, 0u);
            CHECK_EQ(timer_commit, timer_pre);
            CHECK_EQ(idx_commit, idx_pre);
            CHECK((fore14_commit->visual.render_flags & 0x20u) != 0u);
            CHECK_EQ(d_final, d_commit);
        } else {
            CHECK_EQ(prayer_commit, 0u);
            CHECK_EQ(d_commit, d_pre);
            CHECK_EQ(d_final, d_commit);
            CHECK_EQ(timer_final, timer_pre);
        }
        d_commit_prev = d_commit;
    }
}

TEST(caption_window_three_arms_replay_digest_equal) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    constexpr int kFrames = 12;
    App app_a, app_b, app_c;
    CHECK(boot_replay_battle(app_a, rpy, "t2sig-armA-save.bin"));
    CHECK(boot_replay_battle(app_b, rpy, "t2sig-armB-save.bin"));
    CHECK(boot_replay_battle(app_c, rpy, "t2sig-armC-save.bin"));
    if (!app_a.failure.empty() || !app_b.failure.empty() || !app_c.failure.empty()) return;
    th13::GameInput in_a, in_b, in_c;
    for (int F = 0; F < kFrames; ++F) {
        ArmFrame fa, fb, fc;
        const bool ok_a = drive_arm(Arm::A, app_a, in_a, 0, fa);
        const bool ok_b = drive_arm(Arm::B, app_b, in_b, 0, fb);
        const bool ok_c = drive_arm(Arm::C, app_c, in_c, 0, fc);
        CHECK(ok_a);
        CHECK(ok_b);
        CHECK(ok_c);
        if (!ok_a || !ok_b || !ok_c) {
            std::printf("  失败 arm drive f%d: failureA='%s' failureB='%s' failureC='%s'\n", F,
                        app_a.failure.c_str(), app_b.failure.c_str(), app_c.failure.c_str());
            return;
        }
        CHECK_EQ(fb.presents, 1);
        CHECK_EQ(fc.presents, 2);
        CHECK_EQ(fa.presents, 0);
        if (fa.after_commit.loading != fb.after_commit.loading ||
            fa.after_commit.loading != fc.after_commit.loading) {
            std::printf("  失败 caption 三臂 f%d after_commit: A-vs-B[%s] B-vs-C[%s]\n", F,
                        fa.after_commit.diff_against(fb.after_commit).c_str(),
                        fb.after_commit.diff_against(fc.after_commit).c_str());
        }
        CHECK_EQ(fa.after_commit.loading, fb.after_commit.loading);
        CHECK_EQ(fb.after_commit.loading, fc.after_commit.loading);
        CHECK_EQ(fa.final.loading, fb.final.loading);
        CHECK_EQ(fb.final.loading, fc.final.loading);
        if (!(fa.final == fb.final)) report_mismatch("replay", F, "A-vs-B final", fa.final, fb.final);
        if (!(fb.final == fc.final)) report_mismatch("replay", F, "B-vs-C final", fb.final, fc.final);
        CHECK(fa.final == fb.final);
        CHECK(fb.final == fc.final);
        CHECK(fb.after_commit == fb.final);
        CHECK(fc.after_commit == fc.final);
    }
}

TEST(caption_window_three_arms_live_digest_equal) {
    App app_a, app_b, app_c;
    CHECK(boot_live_run(app_a, "t2sig-liveA-save.bin"));
    CHECK(boot_live_run(app_b, "t2sig-liveB-save.bin"));
    CHECK(boot_live_run(app_c, "t2sig-liveC-save.bin"));
    if (!app_a.failure.empty() || !app_b.failure.empty() || !app_c.failure.empty()) return;
    th13::GameInput in_a, in_b, in_c;
    for (int step = 0; step <= 12; ++step) {
        const u32 held = step >= 1 ? kZ : 0u;
        ArmFrame fa, fb, fc;
        const bool ok_a = drive_arm(Arm::A, app_a, in_a, held, fa);
        const bool ok_b = drive_arm(Arm::B, app_b, in_b, held, fb);
        const bool ok_c = drive_arm(Arm::C, app_c, in_c, held, fc);
        CHECK(ok_a);
        CHECK(ok_b);
        CHECK(ok_c);
        if (!ok_a || !ok_b || !ok_c) {
            std::printf("  失败 live arm drive step%d: failureA='%s' failureB='%s' failureC='%s'\n",
                        step, app_a.failure.c_str(), app_b.failure.c_str(), app_c.failure.c_str());
            return;
        }
        if (step == 0) {
            CHECK_EQ(app_a.pending_load, 1u);
            CHECK_EQ(fb.presents, 1);
            CHECK_EQ(fc.presents, 1);
            CHECK_EQ(fa.presents, 1);
            CHECK_EQ(app_b.loading_frames, 0u);
        } else {
            CHECK_EQ(app_b.progress.stage_frame, step);
            CHECK_EQ(fb.presents, 1);
            CHECK_EQ(fc.presents, 2);
            CHECK_EQ(fa.presents, 0);
            CHECK_EQ(app_b.replay_active, false);
            CHECK(app_b.recorder.ready());
        }
        if (!(fa.final == fb.final)) report_mismatch("live", step, "A-vs-B final", fa.final, fb.final);
        if (!(fb.final == fc.final)) report_mismatch("live", step, "B-vs-C final", fb.final, fc.final);
        CHECK(fa.final == fb.final);
        CHECK(fb.final == fc.final);
        CHECK(fa.after_commit == fb.after_commit);
        CHECK(fb.after_commit == fc.after_commit);
        if (step >= 1) {
            CHECK(fb.after_commit == fb.final);
            CHECK(fc.after_commit == fc.final);
        }
    }
}

TEST(present_twice_destination_free_digest_unchanged) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app;
    CHECK(boot_replay_battle(app, rpy, "t2sig-present2-save.bin"));
    if (!app.failure.empty() || !app.run) return;
    th13::GameInput input;
    for (int F = 0; F < 12; ++F) {
        input.update(0);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());
    }
    CHECK_EQ(app.pending_destination, -1);
    CHECK_EQ(app.pending_load, 0u);
    for (int F = 12; F <= 15; ++F) {
        input.update(0);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        const mpdigest::FrameDigest d0 = mpdigest::capture(app);
        CHECK(app.present());
        const mpdigest::FrameDigest d1 = mpdigest::capture(app);
        CHECK(app.present());
        const mpdigest::FrameDigest d2 = mpdigest::capture(app);
        CHECK(app.present());
        const mpdigest::FrameDigest d3 = mpdigest::capture(app);
        if (!(d0 == d1)) report_mismatch("present2", F, "commit-vs-present1", d0, d1);
        if (!(d1 == d2)) report_mismatch("present2", F, "present1-vs-present2", d1, d2);
        if (!(d2 == d3)) report_mismatch("present2", F, "present2-vs-present3", d2, d3);
        CHECK(d0 == d1);
        CHECK(d1 == d2);
        CHECK(d2 == d3);
    }
}

int main() { return th13test::run_all("m1.unit_frame_split_signature"); }
