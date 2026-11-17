// m1.unit_focus_policy_sp / m1.unit_focus_policy_mp —— M1/T6 失焦策略运行时
// flag（ApplicationState::focus_pause_enabled）独立回归与反例件（workflow
// m1.focus_sync 的本层 unit 件）。断言全部读自当前生产源码，不依赖实现者
// 摘要；观测面全部 public（ApplicationState 是 struct）。
//
// 生产语义（本件依据的源码事实）：
//   - sdl/ApplicationState.hpp focus_pause_enabled：public bool 默认 true
//     （SP 逐位默认）；注释声明 M7 Runtime 置位点=MP 会话建立时置 false。
//   - sdl/Application.cpp ApplicationState::update 暂停入口（M1/T2 拆分后
//     现位，原 :1369-1372）：focus_pause = lost_focus && focus_pause_enabled；
//     未暂停时开门 (input.pressed & 0x100u) || focus_pause →
//     pause->open(PauseEntrance::Pause)；已暂停时转发同门
//     pause->controls({pressed, repeated, focus_pause})。显式 Esc 不受门影响。
//   - game/RunPause.cpp RunPause::open（:186 起）：PauseEntrance::Pause →
//     PauseScreen::Pause（RunPause.cpp:207-211）+ environment.paused=true
//     （:270）+ progress.scene_flags|=0x10（:212）+ suspend_music
//     （:269 → ApplicationGame.cpp:284 audio_device.pause_music(true)）。
//   - game/RunScene.cpp RunScene::update（:659）environment.paused 早退 ⇒
//     暂停冻结对局（progress.stage_frame 唯一推进点 :699 不再执行）；
//     ApplicationState::frames 在 commit 段每帧 +1（Application.cpp:1392 注
//     与实现），不受暂停影响。
//   - game/PauseFrame.cpp PauseFrame::update（:33-40）只消费 pressed/
//     repeated；PauseFrameInput::lost_focus 全树无消费者（grep 实证）——
//     转发值当前无可观测面，开门行为由本件逐位钉住。
//   - sdl/BrowserRuntime.cpp:221 回放臂 focus 恒 false 属宿主采样面，不在
//     本件触达面：本件以 update(input, lost_focus) 直驱生产消费点
//     （ApplicationState::update 暂停入口）。
//
// 反例方向（workflow T6 风险清单；任一出现即红）：
//   R1 SP 臂不再失焦自动暂停（focus_policy_sp）；
//   R2 MP 臂（flag=false）失焦帧开 PauseEntrance::Pause 或置 environment.
//      paused（focus_policy_mp 持续失焦窗 + 双会话逐帧 digest 对拍）；
//   R3 门误伤显式 Esc（pressed&0x100 在 flag=false 且失焦帧下仍必须开暂停）；
//   R4 失焦在 MP 臂产生任何模拟分歧（同 replay/seed 双臂 120 帧全 FrameDigest
//      逐帧全等，tests/multiplayer/native/state_digest.hpp 只读复用）。
//
// boot 拍 = 套件同款（frame_split/scheduler 同款）：真实资产 + 双流种子钉 +
// draw_loading + 3 装载帧 + begin_replay_run(games/replays/th13_01.rpy, 0)。
// 驱动 = update+commit（不 present），与 M1 等价口径一致。
#include "../../harness/test_framework.hpp"
#include "state_digest.hpp"

#include "game/GameInput.hpp"
#include "sdl/ApplicationState.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

using th13::i32;
using th13::u8;
using th13::u16;
using th13::u32;
using App = th13::sdl::ApplicationState;
using th13::GameInput;
using mpdigest::FrameDigest;

constexpr u16 kBootSeed = 0x1B2C;  // GameRandom::reseed(uint16_t) / reseed_restore(uint16_t)
constexpr float kFps = 60.0f;

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

// 确定性 boot（套件同款拍：真实资产 + 双流种子钉）。
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

// 一次 update+commit（不 present；M1 等价口径的帧边界驱动）。
bool drive_frame(App& app, GameInput& input, u32 held, bool lost_focus) {
    input.update(held);
    return app.update(input, lost_focus) && app.commit(kFps);
}

}  // namespace

// ---- SP 臂（R1 反例方向）：focus_pause_enabled 默认 true，语义逐位不变 ----
TEST(focus_policy_sp) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    App app;
    CHECK(boot_replay_battle(app, rpy, "focus-sp-save.bin"));
    CHECK(app.run != nullptr);
    CHECK(app.pause != nullptr);

    // SP 默认：flag 未经任何置位即 true（M7 Runtime 置位前的逐位默认）。
    CHECK(app.focus_pause_enabled == true);

    // boot 后无暂停、无冻结。
    CHECK(!app.pause->active());
    CHECK(app.pause->state.screen == th13::PauseScreen::Inactive);
    CHECK(app.environment.paused == false);

    // 失焦门外不暂停：专注帧（lost_focus=false、无 Esc）不开暂停（R1 的
    // 补集面——自动暂停只由失焦触发，不由专注帧触发）。
    GameInput input;
    for (int i = 0; i < 3; ++i) {
        CHECK(drive_frame(app, input, 0, /*lost_focus=*/false));
        CHECK(app.failure.empty());
        CHECK(!app.pause->active());
        CHECK(app.environment.paused == false);
    }
    const i32 stage_frame_focused = app.progress.stage_frame;
    const u32 frames_focused = app.frames;
    CHECK(stage_frame_focused > 0);  // 专注窗对局确在推进（回放 stage1 逐帧 stage_frame++）

    // 失焦帧自动暂停：无任何 Esc 按下沿（held=0），仅 lost_focus=true。
    CHECK(drive_frame(app, input, 0, /*lost_focus=*/true));
    CHECK(app.failure.empty());
    CHECK(app.pause->active());
    CHECK(app.pause->state.screen == th13::PauseScreen::Pause);
    // 由 Inactive 刚 open（PauseEntrance::Pause 的 select 记录 previous）。
    CHECK(app.pause->state.previous == th13::PauseScreen::Inactive);
    // 真实激活副作用（RunPause::open）：冻结位 + 冻结 + 音乐挂起。
    CHECK(app.environment.paused == true);
    CHECK((app.progress.scene_flags & 0x10u) != 0u);
    CHECK(app.audio_device.statistics().music_paused == true);
    // 开门帧发生在 update_scene_pass 之前：当帧对局已冻结。
    CHECK(app.progress.stage_frame == stage_frame_focused);
    CHECK(app.frames == frames_focused + 1);  // 帧体本身照常 commit

    // 暂停持续（保持失焦）：对局冻结（stage_frame 不动）、帧体推进、
    // 转发同门（focus_pause=true 入 pause->controls）无失败。
    for (int i = 1; i <= 4; ++i) {
        CHECK(drive_frame(app, input, 0, /*lost_focus=*/true));
        CHECK(app.failure.empty());
        CHECK(app.pause->active());
        CHECK(app.pause->state.screen == th13::PauseScreen::Pause);
        CHECK(app.progress.stage_frame == stage_frame_focused);
        CHECK(app.frames == frames_focused + u32(1 + i));
    }

    // 焦点恢复不自动恢复对局（RunPause 无 focus-resume 路径；SP 恢复由
    // 用户菜单驱动）：专注帧后暂停仍在、对局仍冻结。
    for (int i = 0; i < 2; ++i) {
        CHECK(drive_frame(app, input, 0, /*lost_focus=*/false));
        CHECK(app.failure.empty());
        CHECK(app.pause->active());
        CHECK(app.pause->state.screen == th13::PauseScreen::Pause);
        CHECK(app.environment.paused == true);
        CHECK(app.progress.stage_frame == stage_frame_focused);
    }

    // flag 不被 update 路径翻转：SP 默认贯穿全程。
    CHECK(app.focus_pause_enabled == true);
}

// ---- MP 臂（R2/R3/R4 反例方向）：flag=false，单端失焦不开暂停、模拟照常 ----
TEST(focus_policy_mp) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));

    // 双会话同 replay/seed（MP 身份握手的模拟态：同资产、同 boot 拍、
    // 双流种子钉同值）。
    App focused, lost;
    CHECK(boot_replay_battle(focused, rpy, "focus-mp-focus-save.bin"));
    CHECK(boot_replay_battle(lost, rpy, "focus-mp-lost-save.bin"));
    CHECK(focused.run != nullptr);
    CHECK(lost.run != nullptr);
    CHECK(focused.pause != nullptr && lost.pause != nullptr);

    // 置位前默认仍为 SP 值；M7 Runtime 置位点=本票声明的公开运行时 flag
    // （真实 MP 会话置位接线属 M7 工单）。
    CHECK(focused.focus_pause_enabled == true);
    CHECK(lost.focus_pause_enabled == true);
    focused.focus_pause_enabled = false;
    lost.focus_pause_enabled = false;
    CHECK(focused.focus_pause_enabled == false);
    CHECK(lost.focus_pause_enabled == false);

    const u32 frames_at_start = lost.frames;
    const i32 stage_frame_at_start = lost.progress.stage_frame;

    // 120 帧双臂锁步：专注臂恒专注；失焦臂周期性失焦（含第 0 帧）。
    // 逐帧断言：双臂均不开暂停（R2）、失焦帧不置冻结位、帧体照常推进；
    // 逐帧全 FrameDigest 全等（R4——失焦在 MP 臂不得产生任何模拟分歧）。
    GameInput in_focus, in_lost;  // 双臂各自独立输入沿历史
    u32 prev_frames = 0;
    for (int frame = 0; frame < 120; ++frame) {
        const bool lost_focus = (frame % 7) != 6;  // 周期失焦，含第 0 帧
        CHECK(drive_frame(focused, in_focus, 0, false));
        CHECK(drive_frame(lost, in_lost, 0, lost_focus));
        CHECK(focused.failure.empty());
        CHECK(lost.failure.empty());
        CHECK(!focused.pause->active());
        CHECK(!lost.pause->active());
        CHECK(lost.environment.paused == false);
        // 帧体每帧严格 +1（update+commit 各一次；暂停/失败都会破坏该序列）。
        if (frame == 0) {
            prev_frames = lost.frames;
        } else {
            CHECK(lost.frames == prev_frames + 1);
            prev_frames = lost.frames;
        }
        const FrameDigest df = mpdigest::capture(focused);
        const FrameDigest dl = mpdigest::capture(lost);
        if (df != dl) {
            std::printf("  失败 frame %d: lost_focus=%d digest diff: %s\n",
                        frame, lost_focus ? 1 : 0, df.diff_against(dl).c_str());
            CHECK(false);
        } else {
            CHECK(true);
        }
    }

    // 对拍后不变量：双臂逻辑帧数一致、对局均在推进（无停等）、flag 未被
    // update 路径翻转。
    CHECK(lost.frames == focused.frames);
    CHECK(lost.frames == frames_at_start + 120u);
    CHECK(lost.progress.stage_frame == focused.progress.stage_frame);
    CHECK(lost.progress.stage_frame > stage_frame_at_start);
    CHECK(lost.environment.paused == false);
    CHECK(focused.environment.paused == false);
    CHECK(lost.focus_pause_enabled == false);
    CHECK(focused.focus_pause_enabled == false);

    // R3：显式 Esc 不受门影响——flag=false 且失焦帧下按下沿 0x100 仍开
    // PauseEntrance::Pause（此前失焦从未开过暂停 ⇒ previous 仍 Inactive）。
    CHECK(drive_frame(lost, in_lost, 0x100u, /*lost_focus=*/true));
    CHECK(lost.failure.empty());
    CHECK(lost.pause->active());
    CHECK(lost.pause->state.screen == th13::PauseScreen::Pause);
    CHECK(lost.pause->state.previous == th13::PauseScreen::Inactive);
    CHECK(lost.environment.paused == true);

    // Esc 开启的暂停在失焦帧下不被干扰（转发 focus_pause=false 无副作用、
    // 无自动恢复路径）。
    CHECK(drive_frame(lost, in_lost, 0, /*lost_focus=*/true));
    CHECK(lost.failure.empty());
    CHECK(lost.pause->active());
    CHECK(lost.pause->state.screen == th13::PauseScreen::Pause);
    CHECK(lost.environment.paused == true);
}

int main() { return th13test::run_all("m1.unit_focus_policy_sp/mp"); }
