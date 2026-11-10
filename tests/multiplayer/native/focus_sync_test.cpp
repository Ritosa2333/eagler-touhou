// m1.focus_sync —— M1/T7 固定 case：T6 双会话扩三态（focused / 失焦 MP /
// 失焦 SP）+ 失焦期输入沿单帧一次采样（workflow T7；BrowserRuntime.cpp:219-222
// 的宿主采样面在 native 的等价观测=失焦不改变每帧恰一次的输入沿流）。
//
// 被测生产面（全部 public，零生产钩子）：
//   - ApplicationState::focus_pause_enabled（ApplicationState.hpp 运行时
//     flag）与 ApplicationState::update 暂停入口（Application.cpp:1403-1408）：
//     focus_pause = lost_focus && focus_pause_enabled——SP 默认 true 逐位不变；
//     MP 会话（M7 Runtime 置位 false）下失焦不开 PauseEntrance::Pause、不入
//     pause->controls；显式 Esc（pressed&0x100）不受门影响。
//   - do/while restart 循环（Application.cpp:1410-1418）：Replay 快进重启
//     多次 update_scene_pass 时物理输入不被重采样——Esc 沿在一帧内只消费
//     一次（开门恰一次 / 不随 pass 重放）。
//   - live-run 录制链（record_input → PendingReplayRecorder 挂起槽 → commit
//     冲账）：失焦帧的输入沿逐帧恰一条、边沿与专注臂逐位全等——「失焦期
//     输入沿单帧一次采样」的字节级证据。
//
// 断言命题：
//   1) 三态回放对拍 90 帧：focused 无暂停；MP 失焦臂（flag=false，周期失焦）
//      无暂停且与 focused 臂逐帧全 FrameDigest 全等；SP 失焦臂（默认 flag）
//      首个失焦帧自动暂停、对局冻结、帧体照常 commit。
//   2) live 三臂录制对拍 61 帧：focused 与 MP 失焦臂的录像帧数/逐帧
//      state/pressed/released 逐位全等（沿单帧一次、不丢不重）；SP 失焦臂
//      暂停冻结后录像停在对局冻结帧（无幻影帧）。
//   3) 快进 restart 循环下的失焦沿：MP 臂失焦+Esc 沿不开暂停（跨多 pass）、
//      沿不被 pass 重放；SP 臂同拍 Esc 沿恰一次开门（age 单调、无二次 open）。
#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/ReplayFile.hpp"
#include "sdl/ApplicationState.hpp"
#include "state_digest.hpp"

#include <algorithm>
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
using mpdigest::FrameDigest;

constexpr u16 kBootSeed = 0x1B2C;
constexpr float kFps = 60.0f;
constexpr u32 kZ = 0x01;
constexpr u32 kRight = 0x80;
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

// 测试存档落到 runner 的 case 运行目录（TH13_NATIVE_RUN_DIR，由
// tests/multiplayer/lib/native-runner.mjs 注入）；未注入时保持相对 cwd。
std::string run_output_path(const char* name) {
    const char* dir = std::getenv("TH13_NATIVE_RUN_DIR");
    if (!dir || !*dir) return name;
    return std::string(dir) + "/" + name;
}

// 确定性 boot（套件同款拍）。
bool pin_boot(App& app, const char* save_name) {
    ::setenv("TH13_SAVE_PATH", run_output_path(save_name).c_str(), 1);
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

// 一次 update+commit+present（失焦经参数注入生产消费点）。
bool drive_frame(App& app, GameInput& input, u32 held, bool lost_focus) {
    input.update(held);
    return app.update(input, lost_focus) && app.commit(kFps) && app.present();
}

bool encode_pinned(const App& app, std::vector<u8>& out) {
    th13::ReplaySaveMeta meta;
    meta.date = 1;
    meta.slow_numerator = 0.0;
    meta.slow_denominator = 1099511627776.0;
    return app.recorder.encode(out, "t7fs", 0, meta);
}

bool bytes_equal(const std::vector<u8>& a, const std::vector<u8>& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}

// 合成输入沿模式（回放臂零输入；live 臂消费）。
u32 live_pattern(u32 f) {
    u32 held = 0;
    if (f % 12 < 7) held |= kZ;
    if (f % 19 >= 11 && f % 19 < 15) held |= kRight;
    return held;
}

// live 臂 battle 帧号 → 失焦（f2 起失焦用于 SP 臂自动暂停选点）。
bool lost_at(u32 battle_frame) { return battle_frame >= 2; }

}  // namespace

// ---- 1) 三态回放对拍 90 帧（focused / 失焦 MP / 失焦 SP）----
TEST(focus_sync_three_states_replay) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App focused, mp_lost, sp_lost;
    CHECK(boot_replay_battle(focused, rpy, "t7fs-focus-save.bin"));
    CHECK(boot_replay_battle(mp_lost, rpy, "t7fs-mp-save.bin"));
    CHECK(boot_replay_battle(sp_lost, rpy, "t7fs-sp-save.bin"));
    if (!focused.failure.empty() || !mp_lost.failure.empty() || !sp_lost.failure.empty() ||
        !focused.run || !mp_lost.run || !sp_lost.run)
        return;
    CHECK(focused.focus_pause_enabled == true);   // SP 默认逐位不变
    CHECK(mp_lost.focus_pause_enabled == true);
    CHECK(sp_lost.focus_pause_enabled == true);
    mp_lost.focus_pause_enabled = false;          // M7 Runtime 置位点（公开运行时 flag）
    CHECK(mp_lost.focus_pause_enabled == false);

    GameInput in_focus, in_mp, in_sp;
    bool mp_pause_ever = false;
    for (int frame = 0; frame < 90; ++frame) {
        const bool lost = (frame % 3) != 0;  // 周期失焦，含 f1（f0 专注基线）
        CHECK(drive_frame(focused, in_focus, 0, false));
        CHECK(drive_frame(mp_lost, in_mp, 0, lost));
        CHECK(drive_frame(sp_lost, in_sp, 0, lost));
        CHECK(focused.failure.empty());
        CHECK(mp_lost.failure.empty());
        CHECK(sp_lost.failure.empty());
        // MP 臂：失焦不开暂停、不置冻结位、对局照常推进。
        if (mp_lost.pause->active()) mp_pause_ever = true;
        CHECK(!mp_pause_ever);
        CHECK(mp_lost.environment.paused == false);
        CHECK_EQ(mp_lost.progress.scene_flags & 0x10u, 0u);
        CHECK_EQ(mp_lost.progress.stage_frame, focused.progress.stage_frame);
        // MP 臂与 focused 臂逐帧全 digest 全等（失焦零模拟分歧）。
        const FrameDigest df = mpdigest::capture(focused);
        const FrameDigest dm = mpdigest::capture(mp_lost);
        if (!(df == dm)) {
            std::printf("  失败 frame %d: lost_focus=%d digest diff: %s\n",
                        frame, lost ? 1 : 0, df.diff_against(dm).c_str());
            CHECK(false);
        } else {
            CHECK(true);
        }
        // SP 臂：首个失焦帧自动暂停（PauseEntrance::Pause），此后冻结。
        if (lost && frame >= 1) {
            CHECK(sp_lost.pause->active());
            CHECK(sp_lost.environment.paused == true);
            CHECK_EQ(sp_lost.progress.scene_flags & 0x10u, 0x10u);
        }
    }
    // 三态收口不变量。
    CHECK(!focused.pause->active());
    CHECK(!mp_lost.pause->active());
    CHECK(sp_lost.pause->active());
    CHECK(focused.frames == 90u);
    CHECK(mp_lost.frames == 90u);                 // MP 失焦不停等
    CHECK(sp_lost.frames == 90u);                 // SP 帧体照常 commit
    CHECK(focused.progress.stage_frame > 0);
    CHECK_EQ(mp_lost.progress.stage_frame, focused.progress.stage_frame);
    // SP 首失焦帧（f1）已开门冻结：对局停在 battle f0（stage_frame==1）。
    CHECK_EQ(sp_lost.progress.stage_frame, 1);
    CHECK(mp_lost.focus_pause_enabled == false);  // flag 不被 update 路径翻转
    CHECK(sp_lost.focus_pause_enabled == true);
}

// ---- 2) live 三臂录制对拍：失焦期输入沿单帧一次采样（字节级）----
TEST(focus_sync_unfocused_edges_sampled_once_live) {
    App focused, mp_lost, sp_lost;
    CHECK(boot_live_run(focused, "t7fs-live-focus-save.bin"));
    CHECK(boot_live_run(mp_lost, "t7fs-live-mp-save.bin"));
    CHECK(boot_live_run(sp_lost, "t7fs-live-sp-save.bin"));
    if (!focused.failure.empty() || !mp_lost.failure.empty() || !sp_lost.failure.empty())
        return;
    mp_lost.focus_pause_enabled = false;
    constexpr int kBattleFrames = 61;  // 屏障帧后 battle f0..f60
    GameInput in_focus, in_mp, in_sp;
    // 屏障帧（三臂一致；失焦不触发——run/pause 尚未建立）。
    CHECK(drive_frame(focused, in_focus, 0, false));
    CHECK(drive_frame(mp_lost, in_mp, 0, false));
    CHECK(drive_frame(sp_lost, in_sp, 0, false));
    for (int f = 0; f < kBattleFrames; ++f) {
        const u32 held = live_pattern(u32(f));
        const bool lost = lost_at(u32(f));
        CHECK(drive_frame(focused, in_focus, held, false));
        CHECK(drive_frame(mp_lost, in_mp, held, lost));
        // SP 臂失焦后停送 Z 沿（Z=暂停菜单确认键 0x80001 bit0——持续 Z 沿会
        // 把暂停菜单当确认导航，属测试输入面干扰，非失焦语义面）。
        const u32 sp_held = lost ? 0u : held;
        CHECK(drive_frame(sp_lost, in_sp, sp_held, lost));
        CHECK(focused.failure.empty());
        CHECK(mp_lost.failure.empty());
        CHECK(sp_lost.failure.empty());
        CHECK(!mp_lost.pause->active());  // MP 失焦臂全程无暂停（沿照常驱动对局）
        if (f < 2) CHECK(!sp_lost.pause->active());
        if (f >= 2) CHECK(sp_lost.pause->active());  // SP 首失焦帧自动暂停
        // 对局帧计数：MP 与 focused 全窗同步推进；SP 冻结于 battle f1。
        CHECK_EQ(mp_lost.progress.stage_frame, focused.progress.stage_frame);
        if (f < 2) CHECK_EQ(sp_lost.progress.stage_frame, focused.progress.stage_frame);
    }
    CHECK_EQ(focused.progress.stage_frame, kBattleFrames);  // battle f0..f60 推进
    CHECK_EQ(mp_lost.progress.stage_frame, kBattleFrames);
    CHECK_EQ(sp_lost.progress.stage_frame, 2);  // battle f0/f1 后冻结

    // 录制对账：encode→decode 逐帧边沿。
    std::vector<u8> enc_focus, enc_mp, enc_sp;
    CHECK(encode_pinned(focused, enc_focus));
    CHECK(encode_pinned(mp_lost, enc_mp));
    CHECK(encode_pinned(sp_lost, enc_sp));
    th13::ReplayFile dec_focus, dec_mp, dec_sp;
    CHECK(dec_focus.decode(enc_focus.data(), enc_focus.size()));
    CHECK(dec_mp.decode(enc_mp.data(), enc_mp.size()));
    CHECK(dec_sp.decode(enc_sp.data(), enc_sp.size()));
    CHECK_EQ(dec_focus.stages.size(), size_t(1));
    CHECK_EQ(dec_mp.stages.size(), size_t(1));
    CHECK_EQ(dec_sp.stages.size(), size_t(1));
    if (dec_focus.stages.empty() || dec_mp.stages.empty() || dec_sp.stages.empty()) return;
    const th13::ReplayStage& st_focus = dec_focus.stages.front();
    const th13::ReplayStage& st_mp = dec_mp.stages.front();
    const th13::ReplayStage& st_sp = dec_sp.stages.front();
    // 帧数：失焦臂与专注臂全等（沿不丢帧不重帧）。
    CHECK_EQ(st_mp.frame_count, st_focus.frame_count);
    CHECK_EQ(st_focus.frame_count, u32(kBattleFrames));
    // SP 臂：暂停冻结后录像停在对局冻结帧（battle f0/f1 两条 + 无幻影）。
    CHECK_EQ(st_sp.frame_count, u32(2));
    // 逐帧边沿对账：state/pressed/released 逐位全等（失焦不改变单帧一次采样）。
    for (u32 i = 0; i < st_focus.frame_count && i < st_mp.frame_count; ++i) {
        const th13::ReplayFrame a = st_focus.frame(i);
        const th13::ReplayFrame b = st_mp.frame(i);
        if (a.state != b.state || a.pressed != b.pressed || a.released != b.released) {
            std::printf("  失败 沿对账 f%u: focus(state=%u pressed=%u released=%u) "
                        "mp(state=%u pressed=%u released=%u)\n",
                        i, a.state, a.pressed, a.released, b.state, b.pressed, b.released);
        }
        CHECK_EQ(u32(a.state), u32(b.state));
        CHECK_EQ(u32(a.pressed), u32(b.pressed));
        CHECK_EQ(u32(a.released), u32(b.released));
    }
    // 合成沿对账（含 D-fix 关首帧 pressed=全量 held）。
    for (u32 i = 0; i < st_focus.frame_count; ++i) {
        const th13::ReplayFrame frame = st_focus.frame(i);
        const u32 f = i;
        const u32 held = live_pattern(f);
        u32 pressed = f == 0 ? held : (held & ~live_pattern(f == 0 ? 0 : f - 1));
        u32 released = f == 0 ? 0 : (live_pattern(f - 1) & ~held);
        if (f == 0) pressed = held;  // D-fix：关首帧 pressed=全量 held、released=0
        CHECK_EQ(u32(frame.state), held);
        CHECK_EQ(u32(frame.pressed), pressed);
        CHECK_EQ(u32(frame.released), released);
    }
}

// ---- 3) 快进 restart 循环下的失焦沿：MP 不开暂停、SP 恰一次开门 ----
TEST(focus_sync_edge_single_sample_under_restart) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App mp_lost, sp_lost;
    CHECK(boot_replay_battle(mp_lost, rpy, "t7fs-ff-mp-save.bin"));
    CHECK(boot_replay_battle(sp_lost, rpy, "t7fs-ff-sp-save.bin"));
    if (!mp_lost.failure.empty() || !sp_lost.failure.empty() || !mp_lost.run || !sp_lost.run)
        return;
    mp_lost.focus_pause_enabled = false;  // MP 会话态
    GameInput in_mp, in_sp;
    // f0..f7 零输入基线（consumed 对齐 8 的倍数）。MP 臂全程失焦（flag=false
    // 不开暂停）；SP 臂基线/快进期专注（Esc 沿选点前不得被失焦自动暂停污染）。
    for (int F = 0; F < 8; ++F) {
        CHECK(drive_frame(mp_lost, in_mp, 0, true));
        CHECK(drive_frame(sp_lost, in_sp, 0, false));
    }
    CHECK(!mp_lost.pause->active());  // MP 失焦基线：不开暂停
    CHECK(!sp_lost.pause->active());
    CHECK_EQ(mp_lost.replay_consumed % 8u, 0u);
    // 快进 burst（MP 臂失焦持续）：跨多 pass 重启不开暂停、游标推进恰一帧/pass。
    for (int k = 0; k < 3; ++k) {
        const i32 sf0 = mp_lost.progress.stage_frame;
        const u32 consumed0 = mp_lost.replay_consumed;
        in_mp.update(kZ);
        CHECK(mp_lost.update(in_mp, true));
        CHECK(mp_lost.commit(kFps));
        CHECK(mp_lost.present());
        CHECK(mp_lost.failure.empty());
        const int advanced = mp_lost.progress.stage_frame - sf0;
        CHECK(advanced >= 2);                              // restart 循环真实发生
        CHECK_EQ(mp_lost.replay_consumed, consumed0 + u32(advanced));  // 每恰一帧
        CHECK(!mp_lost.pause->active());                   // MP 失焦 + 快进不开暂停
    }
    // 失焦无沿窗：MP 臂门在 restart 循环后仍不因失焦开暂停（Esc 沿不在本臂
    // 出现——显式 Esc 是 R3 语义面、不受 flag 门控，属 unit_focus_policy_mp；
    // 本臂只钉失焦路径在快进重启后依旧被门拦下、沿不被 pass 重放）。
    {
        // 再一次快进帧（确认门跨 burst 稳定）。
        in_mp.update(kZ);
        CHECK(mp_lost.update(in_mp, true));
        CHECK(mp_lost.commit(kFps));
        CHECK(mp_lost.present());
        CHECK(mp_lost.failure.empty());
        CHECK(!mp_lost.pause->active());
        // 后续 20 个失焦帧无沿：仍不开暂停（失焦门不受 restart 循环影响）。
        for (int f = 0; f < 20; ++f) {
            CHECK(drive_frame(mp_lost, in_mp, 0, true));
            CHECK(!mp_lost.pause->active());
        }
        CHECK(mp_lost.environment.paused == false);
    }
    // SP 臂同拍（同 replay/seed、专注基线 + 快进 burst + Esc 沿）：恰一次开门。
    for (int F = 0; F < 8; ++F) CHECK(drive_frame(sp_lost, in_sp, 0, false));
    CHECK_EQ(sp_lost.replay_consumed % 8u, 0u);
    for (int k = 0; k < 3; ++k) {
        in_sp.update(kZ);
        CHECK(sp_lost.update(in_sp, false));
        CHECK(sp_lost.commit(kFps));
        CHECK(sp_lost.present());
        CHECK(sp_lost.failure.empty());
        CHECK(!sp_lost.pause->active());
    }
    {
        in_sp.update(kZ | kEsc);
        CHECK(sp_lost.update(in_sp, false));
        CHECK(sp_lost.commit(kFps));
        CHECK(sp_lost.present());
        CHECK(sp_lost.failure.empty());
        // Esc 沿恰一次开门：Pause 态、previous=Inactive、非二次 select。
        CHECK(sp_lost.pause->active());
        CHECK(sp_lost.pause->state.screen == th13::PauseScreen::Pause);
        CHECK(sp_lost.pause->state.previous == th13::PauseScreen::Inactive);
        CHECK(sp_lost.environment.paused == true);
        const i32 sf_at_open = sp_lost.progress.stage_frame;
        // 沿不随 pass/帧重放：age 单调 +1（二次 open 会 select 复位 age）。
        i32 age_prev = sp_lost.pause->state.age.current;
        for (int f = 0; f < 8; ++f) {
            CHECK(drive_frame(sp_lost, in_sp, 0, true));  // 暂停持续期失焦转发同门
            CHECK(sp_lost.pause->active());
            CHECK(sp_lost.pause->state.screen == th13::PauseScreen::Pause);
            CHECK(sp_lost.pause->state.age.current == age_prev + 1);  // 单调无复位
            age_prev = sp_lost.pause->state.age.current;
            CHECK_EQ(sp_lost.progress.stage_frame, sf_at_open);  // 冻结
        }
        // 暂停中快进被 paused 拦下：对局仍冻结（快进门 !paused 条件）。
        CHECK(drive_frame(sp_lost, in_sp, kZ, true));
        CHECK_EQ(sp_lost.progress.stage_frame, sf_at_open);
    }
}

int main() { return th13test::run_all("m1.focus_sync"); }
