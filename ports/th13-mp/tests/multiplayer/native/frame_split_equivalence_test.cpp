// m1.unit_frame_split_equivalence —— M1/T2 三臂等价回归（M1-plan §1 等价口径
// 的 T2 单元窗）。三臂：A=update+commit（M8 重算形态）、B=+present（生产单
// present）、C=present×2（present 次数不敏感形态）；destination 屏障帧不入臂
// 差分（三臂一致走生产单 present，序由 frame_split_order_probes 钉住）。
//
// 两个窗（均为真实生产入口）：
//   1) replay boot（th13_debug_replay 生产序：draw_loading(false)+3 装载帧+
//      begin_replay_run(th13_01.rpy,0)）直入 destination-free 窗 battle
//      f0..f31——含装载 caption 窗 f0..10（r5 起 caption tick 已在 commit，
//      此窗纳入三臂）与窗外稳态帧；
//   2) live-run 窗（start_game(1)→合成输入 Z）：destination 屏障帧 + battle
//      f0..f12（caption 窗 + 窗外帧）。
//
// 逐帧断言：三臂 after_commit / final 全 FrameDigest 逐分量全等（M1-plan §1
// 逐帧比较集：projection 16 项、双 RNG seed+index、AudioDevice statistics、
// FileStore 计数、双 ANM registry 全链序+每 VM 位级、回放游标/屏障旗标、
// 钉 meta 的 recorder encode 字节）；B/C 臂内 after_commit==final（present
// 纯度——「draw-only 派生缓存可变化；VM/RNG/几何/链/池权威态不随 present
// 次数变化」的直接反例探针）。
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

// 一帧三段驱动：destination 屏障帧三臂一致单 present；destination-free 帧
// 按臂定义（A=0、B=1、C=2 次 present）。失败即整体 false（arm 内 failure 面）。
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

// 三臂逐分量相等断言（差异分量名进 stdout 证据；返回值供 CHECK 汇总）。
// expect_purity=false 用于 destination 屏障帧：present 尾正当构建装载挡板
// （draw_loading(false)+pending_load，可确认屏障），不作为 present 纯度反例面。
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

}  // namespace

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
    // battle f0..f31：caption 窗（f0..10，含 f10 retire）+ 窗外稳态 20 帧。
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
        CHECK_EQ(app_a.pending_destination, -1);  // 窗内 destination-free
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
    // step0 = destination 屏障帧（三臂一致单 present；装载挡板两拍原样帧尾）；
    // step1..13 = battle f0..f12（合成输入 Z：battle f0 按下沿并连射）。
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
            CHECK(app_b.recorder.ready());  // live 臂真录制（digest 消费钉 meta encode）
        }
        CHECK(check_arms_equal("live", step, fa, fb, fc, step >= 1));
    }
}

int main() { return th13test::run_all("m1.unit_frame_split_equivalence"); }
