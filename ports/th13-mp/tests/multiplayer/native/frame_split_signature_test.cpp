// m1.unit_frame_split_signature —— M1/T2 帧体三段拆分的符号与相位签名回归。
//
// 被测生产面（全部 public，零生产钩子；实现=Application.cpp 三段拆分 +
// 解耦(a)(b)，ApplicationState.hpp 三段声明，BrowserRuntime.cpp:225 接线）：
//   ApplicationState::update(input, lost_focus) / commit(presentation_fps) /
//   present() 三段独立可调；step 复合 = update && commit && present 短路序。
//
// 断言命题（M1-plan §2/T2 签名单）：
//   1) update 失败 ⇒ commit/present 不执行（frames 不动、Draw 探针不触发、
//      音频统计不动——失败帧终态不可观测的短路语义直接可证）；
//   2) 装载 caption 窗（replay boot battle f0..10）相位：update 段恰未 tick
//      （跨帧 digest 持平）、commit 恰一 tick（script14 计时器逐帧恰 +1、
//      commit 消费零 visual RNG）、f10 commit 内 retire（loading_prayer 清零、
//      树标 0x20、不 tick）；present 后 caption digest 与 commit 后全等
//      ——若 tick/retire 留在 Draw72，此断言必假（present×N 相位漂移）；
//   3) 三臂 A(update+commit) / B(+present) / C(present×2) 在 caption 窗的
//      caption digest 逐帧全等（r4 语义下 present 次数改变 VM 相位——必假；
//      r5 拆分成立 ⇒ 真），live-run 窗全 FrameDigest 逐帧全等；
//   4) destination-free 帧 present×2 全 digest 不变（present 纯度）。
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

constexpr u16 kBootSeed = 0x1B2C;  // 确定性 boot 钉种子（native SDL_GetTicks 桩=0 之上显式再钉）
constexpr float kFps = 60.0f;
constexpr u32 kZ = 0x01;  // 0x4e49f0 键位 bit0 = Z 射击/确认（e2e 同源）

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

// 确定性 boot（e2e / th13_debug_replay 同款拍）：真实资产初始化 + 双流种子钉。
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

// replay 直入对局（th13_debug_replay 生产序：装载幕 + 3 装载帧 + begin_replay_run）。
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

// live-run 直入对局（生产 start_game 链；destination 帧由驱动循环统一走屏障）。
bool boot_live_run(App& app, const char* save_name) {
    if (!pin_boot(app, save_name)) return false;
    if (!app.start_game(1)) {
        std::printf("  失败 boot: start_game: %s\n", app.failure.c_str());
        return false;
    }
    return app.pending_destination == i32(Destination::Run) && app.failure.empty();
}

// ---- 三臂定义（M1-plan §1）：A=update+commit、B=+present、C=present×2。
// destination 帧（update/commit 期排队、present 尾消费的转换屏障）不入臂差分
// ——三臂一致走生产单次 present（屏障帧不是 present×N 的合法变体面；其序由
// frame_split_order_probes 钉住）。
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

// Draw 段探针（scheduler.add 公开面；优先级 73 空闲——AnmDrawSchedule 占
// 5..71、loading_battle_draw@72、layer28@74、item@75、fps@77、ascii@78）。
struct DrawProbe {
    th13::FrameCallback callback{};
    App* app = nullptr;
    th13::FrameScheduler* attached_scheduler = nullptr;  // 登记归属（FrameCallback::scheduler private）
    int fires = 0;
    bool observed_ok = false;
    static i32 run(void* p) {
        auto& probe = *static_cast<DrawProbe*>(p);
        ++probe.fires;
        probe.observed_ok = true;
        probe.callback.enabled = false;  // 闩锁：只观测第一次 Draw 拍
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
        // FrameScheduler::remove 对未注册回调按 c.scheduler!=this 幂等跳过。
        if (attached_scheduler) attached_scheduler->remove(callback);
        attached_scheduler = nullptr;
        app = nullptr;
    }
    ~DrawProbe() { detach(); }
};

}  // namespace

// ---- 1) 三段符号独立可调 + update 失败 ⇒ commit/present 不执行 ----
TEST(split_segments_update_failure_short_circuit) {
    // 未初始化实例：update 段守卫即 false；复合 step 短路 → commit/present
    // 不执行（commit 会 frames++，present 会解引用空 display——都不发生）。
    {
        App fresh;
        th13::GameInput input;
        input.update(0);
        CHECK_EQ(fresh.initialized, false);
        CHECK(!fresh.update(input, false));
        CHECK(!fresh.step(input, kFps, false));
        CHECK_EQ(fresh.frames, 0u);  // commit 未执行
        CHECK(fresh.failure.empty());  // false 来自守卫而非 fail()
    }
    // 已初始化实例：三段符号逐个可调（帧内容与复合 step 同序），随后注入
    // 失败（写 fail() 的同一 public 字段 = 与「先前段失败」等价的公开面注入）
    // 验证复合 step 的短路：frames 不动、Draw 探针不触发、音频统计不动。
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    DrawProbe probe;  // 先于 app 声明（析构序：探针回调须活得比 scheduler 久）
    App app;
    CHECK(boot_replay_battle(app, rpy, "t2sig-shortcircuit-save.bin"));
    if (!app.failure.empty() || !app.run) return;
    th13::GameInput input;
    // 段级独立调用（符号存在性 + 逐操作同序首帧）：
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
    CHECK(!app.update(input, false));  // 守卫读 failure
    CHECK(!app.step(input, kFps, false));  // 复合短路（commit/present 不执行）
    CHECK_EQ(app.frames, 2u);  // commit 未执行
    CHECK_EQ(probe.fires, 0);  // present 未执行
    CHECK_EQ(app.failure, std::string("t2 signature short-circuit injection"));  // fail() 未被调用
    const auto stats_after = app.audio_device.statistics();
    CHECK_EQ(stats_after.requests, stats_before.requests);
    CHECK_EQ(stats_after.started, stats_before.started);
    probe.detach();
}

// ---- 2) 装载 caption 窗相位（replay boot battle f0..11；arm B 逐段捕获）----
TEST(caption_commit_phase_replay_window) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app;
    CHECK(boot_replay_battle(app, rpy, "t2sig-caption-save.bin"));
    if (!app.failure.empty() || !app.run) return;
    CHECK_EQ(app.progress.stage_frame, 0);  // boot 不变量（restore 后对局 f0 未跑）
    CHECK_EQ(app.replay_consumed, 0u);
    CHECK(app.loading_prayer != 0);  // 装载树随 draw_loading(false) 建立
    const uint64_t d_boot = mpdigest::digest_loading_caption(app);
    const th13::AnmVm* fore14_boot = mpdigest::find_fore_vm(app, 14);
    CHECK(fore14_boot != nullptr);  // replay boot 三装载帧后前池（script14）已 spawn
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
        CHECK(fore14_pre != nullptr);  // 前池 script14 窗内存活（fade 30 帧不自退）
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

        // update 段恰未 tick：本帧 update 后 digest == 上一帧 commit 后。
        if (d_pre != d_commit_prev) {
            std::printf("  失败 caption 相位 f%d: update 段后 loading digest 相对上一 commit 变化（tick 泄漏进 update 段）\n", F);
        }
        CHECK_EQ(d_pre, d_commit_prev);
        if (F == 0) {
            // battle f0：装载树保持装载末帧状态（f0 不 tick、不 retire）。
            CHECK_EQ(d_commit, d_pre);
            CHECK_EQ(timer_commit, timer_pre);
            CHECK_EQ(prayer_commit, prayer_pre);
            CHECK_EQ(d_final, d_commit);
        } else if (F >= 1 && F <= 9) {
            // battle f1..f9：commit 恰一 tick——digest 变化、script14 计时器
            // 恰 +1、commit 消费零 visual RNG；present 后全等（Draw72 只剩
            // alpha 覆写 save/restore——若 tick 留在 Draw72 此处必假）。
            if (d_commit == d_pre) {
                std::printf("  失败 caption 相位 f%d: commit 未 tick 装载树（tick 未迁入 commit）\n", F);
            }
            CHECK(d_commit != d_pre);
            CHECK_EQ(timer_commit, timer_pre + 1);
            CHECK_EQ(idx_commit, idx_pre);  // commit 段零 visual RNG（tick 无 random opcode）
            if (d_final != d_commit) {
                std::printf("  失败 caption 相位 f%d: present 后 loading digest 变化（tick/retire 留在 Draw72 的直接反例）\n", F);
            }
            CHECK_EQ(d_final, d_commit);
            CHECK_EQ(timer_final, timer_commit);
            CHECK_EQ(prayer_commit, prayer_pre);
        } else if (F == 10) {
            // battle f10：retire 在 commit——句柄清零、树标 0x20、不 tick。
            CHECK(prayer_pre != 0);
            CHECK_EQ(prayer_commit, 0u);
            CHECK_EQ(timer_commit, timer_pre);  // retire 只标树不 tick
            CHECK_EQ(idx_commit, idx_pre);
            CHECK((fore14_commit->visual.render_flags & 0x20u) != 0u);
            CHECK_EQ(d_final, d_commit);
        } else {
            // battle f11：窗外（battle_frame 11 ∉ [0,10]）——commit/present 全 no-op。
            CHECK_EQ(prayer_commit, 0u);
            CHECK_EQ(d_commit, d_pre);
            CHECK_EQ(d_final, d_commit);
            CHECK_EQ(timer_final, timer_pre);
        }
        d_commit_prev = d_commit;
    }
}

// ---- 3a) caption 窗三臂：A/B/C replay boot 逐帧 digest 全等 ----
TEST(caption_window_three_arms_replay_digest_equal) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    constexpr int kFrames = 12;  // battle f0..f11（含 f10 retire 与窗外一帧）
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
        // caption digest：commit 后三臂全等、全段后三臂全等（r4 缺口 A 的
        // 直接反例探针——tick 留在 Draw72 则 B/C 相位漂移、A 树冻结）。
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
        // 全 FrameDigest 同窗全等（caption 窗自 r5 起纳入三臂等价口径）。
        if (!(fa.final == fb.final)) report_mismatch("replay", F, "A-vs-B final", fa.final, fb.final);
        if (!(fb.final == fc.final)) report_mismatch("replay", F, "B-vs-C final", fb.final, fc.final);
        CHECK(fa.final == fb.final);
        CHECK(fb.final == fc.final);
        // present 纯度（B/C 臂内）。
        CHECK(fb.after_commit == fb.final);
        CHECK(fc.after_commit == fc.final);
    }
}

// ---- 3b) live-run 窗三臂（start_game(1)→合成输入）：全 digest 逐帧全等 ----
TEST(caption_window_three_arms_live_digest_equal) {
    App app_a, app_b, app_c;
    CHECK(boot_live_run(app_a, "t2sig-liveA-save.bin"));
    CHECK(boot_live_run(app_b, "t2sig-liveB-save.bin"));
    CHECK(boot_live_run(app_c, "t2sig-liveC-save.bin"));
    if (!app_a.failure.empty() || !app_b.failure.empty() || !app_c.failure.empty()) return;
    th13::GameInput in_a, in_b, in_c;
    // f0 = destination 屏障帧（三臂一致单 present）；f1..f12 = battle f0..f11，
    // 合成输入 Z 常按（battle f0 起按下沿 + 连射——game RNG/弹 VM 真实推进）。
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
            // 屏障帧：三臂一致单 present；装载挡板两拍（draw_loading+pending_load）
            // 原样在 present 尾（次帧不绘制语义由 order probes 钉住）。
            CHECK_EQ(app_a.pending_load, 1u);
            CHECK_EQ(fb.presents, 1);
            CHECK_EQ(fc.presents, 1);
            CHECK_EQ(fa.presents, 1);
            CHECK_EQ(app_b.loading_frames, 0u);
        } else {
            CHECK_EQ(app_b.progress.stage_frame, step);  // battle f0 起步
            CHECK_EQ(fb.presents, 1);
            CHECK_EQ(fc.presents, 2);
            CHECK_EQ(fa.presents, 0);
            CHECK_EQ(app_b.replay_active, false);  // live 臂真录制
            CHECK(app_b.recorder.ready());
        }
        if (!(fa.final == fb.final)) report_mismatch("live", step, "A-vs-B final", fa.final, fb.final);
        if (!(fb.final == fc.final)) report_mismatch("live", step, "B-vs-C final", fb.final, fc.final);
        CHECK(fa.final == fb.final);
        CHECK(fb.final == fc.final);
        CHECK(fa.after_commit == fb.after_commit);
        CHECK(fb.after_commit == fc.after_commit);
        if (step >= 1) {
            // present 纯度只对 destination-free 帧断言：屏障帧（step0）的
            // present 尾正当构建装载挡板（draw_loading(false) 建 caption 根
            // + 边条 VM），属可确认屏障，不是 present 纯度反例面。
            CHECK(fb.after_commit == fb.final);
            CHECK(fc.after_commit == fc.final);
        }
    }
}

// ---- 4) destination-free 帧 present×2 全 digest 不变 ----
TEST(present_twice_destination_free_digest_unchanged) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app;
    CHECK(boot_replay_battle(app, rpy, "t2sig-present2-save.bin"));
    if (!app.failure.empty() || !app.run) return;
    th13::GameInput input;
    // 推进 12 帧（battle f0..f11）越过 caption 窗与 retire，全部 destination-free。
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
