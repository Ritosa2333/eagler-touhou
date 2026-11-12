// m1.update_present_equivalence —— M1/T7 固定 case：三臂（A=update+commit、
// B=+present、C=present×2）在 destination-free 全窗的逐帧全 digest 全等
//（M1-plan §1 等价口径；workflow T7「destination-free 窗（装载挡板
// 0..9/transition 门/暂停开/关/600+1200 帧窗）+ caption 窗 battle f0..10」）。
//
// 被测生产面（全部 public，零生产钩子）：ApplicationState::update/commit/
// present 三段拆分（Application.cpp:1388-1632）与 commit_loading_battle_
// caption（:272-308，T2 解耦(a)——caption tick 门控键=stage_frame，与 present
// 次数无关）。若 present 次数再改变任何权威态（RNG/VM 相位/链序/音频/文件/
// 录像），三臂 digest 在本文件任一窗必假。
//
// 窗口集（全部 destination-free；destination 帧按三臂一致单 present 的屏障
// 规则穿过、不入纯度断言，其序由 side_effect_boundary 探针钉住）：
//   1) replay boot caption 窗 battle f0..f11（装载挡板 0..9 + f10 retire +
//      窗外一帧）；
//   2) live-run 装载挡板两拍（Run destination 帧 pending_load 建立 + 末次
//      装载幕 present=complete_loading→begin_run）+ battle f0..f11 caption 窗；
//   3) live-run NextStage(0xc) transition 门全程（destination 帧→door→
//      start_transition→30 帧门→activate→新关）——scenery 淡入/退场倒计时
//      在固定逻辑帧绘制中按原单趟顺序推进（decomp 0x406d90 / 0x407310），
//      基线段三臂按单 present 纪律一致
//      驱动 + 时序钉，door 帧与淡入完成后的新关帧保持标准三臂差分；
//      另设 present_coupling 块以 0/1/2 present 钉完整状态与原单趟时序；
//   4) replay 对局暂停开/关（Esc 开 →24 暂停帧→Esc 关→24 恢复帧）；
//   5) replay 对局长窗 600+1200 帧连续（同一三臂实例跑满 1200 帧）。
// 比较集 = tests/multiplayer/native/state_digest.hpp 的 FrameDigest 全部分量
//（projection 16 项、双 RNG、AudioDevice 统计、FileStore 计数、双 ANM
// registry 位级、session、录像 encode 字节）——float 按 bits，无容差。
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
constexpr u32 kZ = 0x01;    // 0x4e49f0 键位 bit0 = Z 射击/确认（e2e 同源）
constexpr u32 kEsc = 0x100; // 暂停键（Application.cpp 暂停门 pressed&0x100）

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

// 确定性 boot（套件同款拍）：真实资产 + 双流种子钉。
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

// live-run 直入对局（生产 start_game 链；destination 帧由驱动循环走屏障规则）。
bool boot_live_run(App& app, const char* save_name) {
    if (!pin_boot(app, save_name)) return false;
    if (!app.start_game(1)) {
        std::printf("  失败 boot: start_game: %s\n", app.failure.c_str());
        return false;
    }
    return app.pending_destination == i32(Destination::Run) && app.failure.empty();
}

// ---- 三臂定义（M1-plan §1）----
enum class Arm { A, B, C };

struct ArmFrame {
    mpdigest::FrameDigest after_commit;
    mpdigest::FrameDigest final;
    int presents = 0;
    bool barrier = false;  // update 后 pending_destination>=0（present 尾消费的转换屏障帧）
    bool ok = false;
};

// 一次三臂驱动：同输入沿同帧喂三臂；屏障帧三臂一致单 present，其余帧按臂
// 定义 0/1/2 次 present。任一臂失败即整帧失败（返回 false 由调用方落证）。
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

// 已确认 present 耦合面（scenery 淡入/退场倒计时，见窗 3 头注）的同步驱动拍：
// 一律恰一次 present（生产 1 present/逻辑帧纪律），终态 digest 由调用方跨臂比较。
bool drive_sync(App& app, GameInput& input, u32 held, mpdigest::FrameDigest& out_final) {
    input.update(held);
    if (!app.update(input, false)) return false;
    if (!app.commit(kFps)) return false;
    if (!app.present()) return false;
    out_final = mpdigest::capture(app);
    return app.failure.empty();
}

// 三臂逐帧断言：present 次数按臂、commit 后三臂全等、终态三臂全等；
// 非屏障帧加 present 纯度（臂内 commit 后 == present 后）。
// 返回 false = 出现失败（调用方应停止推进避免污染后续断言面）。
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
        // present 纯度（臂内）：屏障帧的 present 尾正当消费 destination，不入纯度面。
        const bool pure_b = fb.after_commit == fb.final;
        const bool pure_c = fc.after_commit == fc.final;
        if (!pure_b) report_digest_mismatch(window, frame, "purity B", fb.after_commit, fb.final);
        if (!pure_c) report_digest_mismatch(window, frame, "purity C", fc.after_commit, fc.final);
        CHECK(pure_b);
        CHECK(pure_c);
    }
    return equal;
}

}  // namespace

// ---- 窗 1：replay boot caption 窗 battle f0..f11（装载挡板 0..9 + f10 retire）----
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
        CHECK_EQ(app_a.progress.stage_frame, 0);  // boot 不变量（restore 后对局 f0 未跑）
        CHECK(app_a.loading_prayer != 0);          // 装载树随 draw_loading(false) 建立
        GameInput in_a, in_b, in_c;
        for (int F = 0; F <= 11; ++F) {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            // 本窗无 destination（回放对局开局窗）；屏障规则不得触发。
            CHECK(!fa.barrier);
            if (fa.barrier) return;
            if (!assert_arms_equal("caption", F, fa, fb, fc)) return;
            // caption 分量单列（T2 解耦(a) 的直钉面：tick 在 commit、present 仅覆写）。
            CHECK_EQ(fa.after_commit.loading, fb.after_commit.loading);
            CHECK_EQ(fb.after_commit.loading, fc.after_commit.loading);
            CHECK_EQ(fa.final.loading, fb.final.loading);
            CHECK_EQ(fb.final.loading, fc.final.loading);
        }
    }
}

// ---- 窗 2：live-run 装载挡板两拍 + battle f0..f11 caption 窗 ----
TEST(update_present_equivalence_run_baffle_and_caption_live) {
    App app_a, app_b, app_c;
    CHECK(boot_live_run(app_a, "t7upe-liveA-save.bin"));
    CHECK(boot_live_run(app_b, "t7upe-liveB-save.bin"));
    CHECK(boot_live_run(app_c, "t7upe-liveC-save.bin"));
    if (app_a.failure.empty() && app_b.failure.empty() && app_c.failure.empty()) {
        GameInput in_a, in_b, in_c;
        // step0 = Run destination 屏障帧（present 尾 draw_loading(false)+pending_load=1）。
        {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK(fa.barrier);
            CHECK_EQ(app_b.pending_load, 1u);        // present 尾置位
            CHECK_EQ(app_b.loading_frames, 0u);
            CHECK(app_b.loading_prayer != 0);         // 装载幕已建
            CHECK_EQ(app_b.pending_destination, i32(Destination::Run));  // Run 分支不消费
            if (!assert_arms_equal("baffle", 0, fa, fb, fc)) return;
        }
        // step1 = 末次装载幕 present 帧（update 头 complete_loading→begin_run，
        // battle f0 起）；step1..12 = battle f0..f11 caption 窗 + 窗外一帧。
        // Z 常按（battle f0 起按下沿 + 连射——game/visual 流真实推进）。
        for (int step = 1; step <= 12; ++step) {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, kZ, fa) &&
                            drive_arm(Arm::B, app_b, in_b, kZ, fb) &&
                            drive_arm(Arm::C, app_c, in_c, kZ, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK_EQ(app_b.progress.stage_frame, step);  // battle f0 起步
            CHECK_EQ(app_b.pending_load, 0u);
            CHECK(!fa.barrier);  // 换关延续未排队：本窗全 destination-free
            if (fa.barrier) return;
            if (!assert_arms_equal("baffle-live", step, fa, fb, fc)) return;
            if (step <= 11) {
                // caption 分量：battle f0..f10 的装载挡板窗（f10 retire）。
                CHECK_EQ(fa.final.loading, fb.final.loading);
                CHECK_EQ(fb.final.loading, fc.final.loading);
            }
        }
        CHECK(app_b.replay_active == false);  // live 臂真录制
        CHECK(app_b.recorder.ready());
    }
}

// ---- 窗 3：live-run NextStage(0xc) transition 门全程 ----
// 已确认生产接口（decomp 实证，2026-11-09 复核）：scenery 入场/退场生命周期
// 倒计时在原版即为 Draw 侧逐 present 推进——0x406d90（背景 draw 趟）
// artifacts/decomp/jp/functions/00406d90.c:98-141（timer<0x1e → 淡入 overlay
// 分配 + +0x4054 计时器置 1 + 几何位翻转）与 0x407310（前景 draw 趟）
// functions/00407310.c:87-94（FUN_004732c0 逐 present 递减 + 归零翻转
// flags|=8 retired / flags&=~6）。port 的 StageScenery::finish_draw_pass
// （RunScene::draw_scenery 的 Draw 回调内调用）为忠实复现——此窗的
// scenery 淡入/退场时间线属「present 耦合的已确认面」，与 destination
// 屏障同类（M1-plan §1「destination 帧不进三臂」同则）。适配（不降断言）：
//   - 淡入/退场耦合段（start_transition 起 ~31 present）：三臂按已确认的
//     单 present/帧纪律一致驱动，逐帧全 digest 跨臂全等 + 已确认时序钉
//     （几何位 timeline：begin_enter 帧 true → 倒计时 false → 完成帧起 true）；
//   - 其余帧（door 帧与淡入完成后的新关帧）保持标准三臂 A/B/C 全 digest
//     全等 + present 纯度；
//   - 另设 update_present_equivalence_transition_gate_present_coupling 块，
//     以 1 present/帧 vs 2 present/帧 双臂钉「逐 present 倒计时」的已确认
//     语义（分叉起点随 present 速率精确减半）——若未来有人把倒计时搬去
//     update 侧而不修本口径，该块即红。
TEST(update_present_equivalence_transition_gate) {
    App app_a, app_b, app_c;
    CHECK(boot_live_run(app_a, "t7upe-gateA-save.bin"));
    CHECK(boot_live_run(app_b, "t7upe-gateB-save.bin"));
    CHECK(boot_live_run(app_c, "t7upe-gateC-save.bin"));
    if (app_a.failure.empty() && app_b.failure.empty() && app_c.failure.empty()) {
        GameInput in_a, in_b, in_c;
        // 屏障帧（destination 消费）+ battle f0..f5 稳态（present-pure，差分臂）。
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
        // 排队 0xc（生产 request_destination(0xc) 同值拍：run 活跃、无开局排队）。
        app_a.pending_destination = i32(Destination::NextStage);
        app_b.pending_destination = i32(Destination::NextStage);
        app_c.pending_destination = i32(Destination::NextStage);
        // destination 帧：present 尾 apply_destination 只置 flag 不拆场景。
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
        // door 帧（i=0，present-pure：新 scenery 未启动、旧 scenery 计时器 0
        // 无倒计时——prepare_next_stage 于 update 头、无任何 Draw 侧状态写入）。
        {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK(!fa.barrier);
            CHECK(app_b.stage_transition_active);   // door 已入（prepare_next_stage）
            if (!assert_arms_equal("gate-door", 0, fa, fb, fc)) return;
        }
        // 淡入/退场耦合段 i=1..32（start_transition 于 i=1 update 触发
        // begin_exit(30)/begin_enter(60)；激活于 i=31 update；1 present/帧下
        // 新 scenery 的零穿越发生在 i=32 的 present——pass2 首见 timer<30 后
        // 置几何位并 reset(1)、pass5 归零；旧 scenery 于 i=30 的 present
        // retired、i=31 的 @13 卸载）。三臂一致单 present 纪律。
        for (int i = 1; i <= 32; ++i) {
            mpdigest::FrameDigest fa, fb, fc;
            const bool ok = drive_sync(app_a, in_a, 0, fa) &&
                            drive_sync(app_b, in_b, 0, fb) &&
                            drive_sync(app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK(app_b.run != nullptr);
            CHECK(!app_b.stage_transition_loading);
            // 跨臂全 digest 全等（一致驱动下的确定性等价）。
            if (!(fa == fb) || !(fb == fc)) {
                std::printf("  失败 gate-fade i%d: A-vs-B[%s] B-vs-C[%s]\n", i,
                            fa.diff_against(fb).c_str(), fb.diff_against(fc).c_str());
                CHECK(false);
                return;
            }
            CHECK(true);
            // 已确认时序钉（新 scenery 几何位 timeline，1 present/帧）：
            // begin_enter 帧 bit0 仍置位；首 present 后进入倒计时（几何关）；
            // 第 31 present（i=31）完成零穿越后恢复。
            const bool geometry = app_b.run->scenery->draw_geometry_enabled();
            // post-present 观测：首 present（i=1）即清几何位（timer 60>=30 走
            // else 分支）；i=32 的 present 完成 reset(1)+零穿越并恢复几何位。
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
                CHECK(!app_b.stage_transition_active);  // 激活于本帧 update（age==30）
                CHECK(!app_b.transition_gate_active);
            }
        }
        // 淡入完成后的新关帧 i=33..41（present-pure，恢复标准差分三臂；
        // fast 臂零穿越在 i=30、slow 在 i=32，i=33 起差分对两臂均安全）。
        for (int i = 33; i <= 41; ++i) {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok) return;
            CHECK(!fa.barrier);
            if (fa.barrier) return;
            CHECK(app_b.run->scenery->draw_geometry_enabled());  // 倒计时已完
            if (!assert_arms_equal("gate-post", i, fa, fb, fc)) return;
        }
        CHECK(app_b.progress.stage_frame >= 1);  // 新关对局推进
        CHECK(app_b.run != nullptr);
    }
}

// ---- 窗 3b：换关全状态不受 0/1/2 次呈现影响 ----
// 原版每逻辑帧的一趟 Draw 写回已固定到 commit；继续钉住原单趟
// 相位（old 第30帧退场、第31帧卸载；new 第32帧可见）。
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

// ---- 窗 4：暂停开/关（Esc 生产入口；24 暂停帧 + 24 恢复帧）----
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
        // 稳态 16 帧（越过 caption 窗）。
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
        // Esc 开（按下沿；同一帧 update 段头开门——当帧对局已冻结）。
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
            CHECK_EQ(app_b.progress.stage_frame, frozen_at);  // 开门帧已冻结
            if (!assert_arms_equal("pause-open", 0, fa, fb, fc)) return;
        }
        // 24 暂停帧：对局冻结、帧体照常 commit、三臂全等（present×2 含暂停
        // 覆盖层重复入队——captions 队列在 Update@4 排空，不入 digest）。
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
        // 等菜单 phase 6（Esc 关闭只在普通暂停菜单态生效；select 后 phase=0，
        // age>9 → phase(6)）。
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
        // Esc 关（按下沿；phase16 动画收场后 finish_selection 恢复）。
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
        // 恢复后 24 帧：对局再推进、三臂全等。
        for (int F = 0; F < 24; ++F) {
            ArmFrame fa, fb, fc;
            const bool ok = drive_arm(Arm::A, app_a, in_a, 0, fa) &&
                            drive_arm(Arm::B, app_b, in_b, 0, fb) &&
                            drive_arm(Arm::C, app_c, in_c, 0, fc);
            CHECK(ok);
            if (!ok || !assert_arms_equal("pause-post", F, fa, fb, fc)) return;
        }
        CHECK(app_b.progress.stage_frame > frozen_at);  // 冻结后真实恢复推进
    }
}

// ---- 窗 5：长窗 600+1200 帧连续（同一三臂实例；回放 stage1 f0..f1200）----
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
            if (fa.barrier) ++barriers;  // stage1 窗实测零屏障；出现即窗口选点漂移
            if (!assert_arms_equal("long", F, fa, fb, fc)) return;
            if (F == 299 || F == 599 || F == 1199) {
                // 窗口进度正控：对局真实推进（非空转等价）。
                CHECK(app_b.progress.stage_frame > 0);
                CHECK(app_b.replay_consumed > 0);
            }
        }
        CHECK_EQ(barriers, 0);  // 1200 帧窗 destination-free（th13_01 stage1 实测）
        CHECK_EQ(app_a.frames, 1200u);
        CHECK_EQ(app_b.frames, 1200u);
        CHECK_EQ(app_c.frames, 1200u);
    }
}

int main() { return th13test::run_all("m1.update_present_equivalence"); }
