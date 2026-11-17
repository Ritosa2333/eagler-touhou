// m1.unit_recorder_pending_once —— M1/T3 可恢复录制槽独立回归（工单步骤
// ②③：record_input 捕获进挂起槽 input/slow/edge 随槽 + commit 按帧身份幂等
// 冲账；update_spell_time 钟值随槽；slow 字节源=捕获拍上一 fps 窗）。
//
// 被测生产面（全部 public，零生产钩子；实现=Application.cpp:1393 捕获段头
// capture_frame 同步、:860-875 record_input 回调捕获、:1425-1446 冲账、
// ApplicationState.hpp:183-230 PendingReplayRecorder 同 API 包装、
// ApplicationGame.cpp:93-96 record_spell_time 拦截）。
//
// 工单反例（任一出现即机制不成立，断言逐条钉死）：
//   - 同帧双/三 commit 产生第二条录像记录（encode 逐字节一致 + decode
//     frame_count 不变）；
//   - slow 字节不在捕获拍随槽（源改为恒 60 / last_fps_ / commit 重算都会
//     在 fps 窗变化后的事件字节错位）；
//   - D-fix 关首边沿复位不在冲账拍应用（帧 0 pressed != 全量 held）或
//     复位标志不随槽（捕获拍已消费、冲账拍无据可依）；
//   - 捕获拍同步校验丢失（!ready() 时槽被推入条目、失败消息/拍漂移）。
//
// live-run 录制测试走 start_game(1) → 合成输入真实链（GameInput 边沿 +
// RunScene Update 20 record_input 回调），不建假 recorder、不伪造游戏。
#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/ReplayFile.hpp"
#include "sdl/ApplicationState.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

namespace {

using th13::u8;
using th13::u16;
using th13::u32;
using th13::i32;
using App = th13::sdl::ApplicationState;
using th13::sdl::Destination;

constexpr u16 kBootSeed = 0x1B2C;  // 确定性 boot 钉种子（T2 签名测试同款）
constexpr float kFps = 60.0f;
constexpr u32 kZ = 0x01;      // 0x4e49f0 键位 bit0 = Z 射击/确认
constexpr u32 kShift = 0x08;  // bit3 = Shift 低速（标题不消费，安全预持）
constexpr u32 kRight = 0x80;  // bit7 = 右
constexpr u32 kDown = 0x20;   // bit5 = 下

const char* repo_root() {
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return root && *root ? root : "../../..";
}

// 确定性 boot（T2 frame_split_signature_test 同款拍：真实资产 + 双流种子钉）。
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

// live-run 直入对局（生产 start_game 链；destination 帧由驱动循环统一走屏障）。
bool boot_live_run(App& app, const char* save_name) {
    if (!pin_boot(app, save_name)) return false;
    if (!app.start_game(1)) {
        std::printf("  失败 boot: start_game: %s\n", app.failure.c_str());
        return false;
    }
    return app.pending_destination == i32(Destination::Run) && app.failure.empty();
}

// encode 字节（钉保存时刻实时量 meta：date/den 钉死 → 跨时点逐字节可比）。
bool encode_pinned(const App& app, std::vector<u8>& out) {
    th13::ReplaySaveMeta meta;
    meta.date = 1;
    meta.slow_numerator = 0.0;
    meta.slow_denominator = 1099511627776.0;  // 2^40 → slow=0%
    return app.recorder.encode(out, "t3rec", 0, meta);
}

i32 total_frames(const App& app) {
    return app.files.ordinary_records[size_t(app.progress.character)].total_frames;
}

bool bytes_equal(const std::vector<u8>& a, const std::vector<u8>& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}

// 捕获拍 slow 字节（record_input 回调 :866 同式读法——源=上一 fps 窗）。
u8 expected_slow(const App& app) {
    return u8(std::min(255.0f, std::max(0.0f, app.slow_window_fps_ + 0.5f)));
}

// 合成输入沿模式（Z/方向/低速 周期性按放，覆盖 pressed/released 双沿）。
u32 pattern(u32 f) {
    u32 held = 0;
    if (f % 9 < 5) held |= kZ;
    if (f % 17 >= 10 && f % 17 < 14) held |= kRight;
    if (f % 23 == 20) held |= kDown;
    if (f % 31 >= 25) held |= kShift;
    return held;
}

// 单帧捕获日志（decode 期望的来源；只在 stage_frame 推进帧记录）。
struct CaptureLog {
    th13::GameInput input{};  // 捕获拍原始输入（D-fix 复位在冲账拍应用）
    u8 slow = 0;              // 捕获拍随槽 slow 字节
    bool edge_reset = false;  // D-fix 关首复位标志随槽
    u32 frame = 0;            // 捕获拍逻辑帧身份
};

}  // namespace

// ---- 1) 关首 D-fix 边沿复位随槽 + 槽随行量（帧身份/slow/edge）----
// 反例钉死：battle f0 按 Shift 预持 + Z 新按——活沿 pressed=0x1、held=0x9；
// D-fix（0x448ec0 关首输入沿复位）要求落盘 pressed=全量 held=0x9、released=0。
// 若复位决定不随槽（捕获拍消费后冲账拍无据），落盘退化为活沿 0x1——直接可辨。
TEST(recorder_pending_dfix_first_frame_slot) {
    App app;
    CHECK(boot_live_run(app, "t3rec-dfix-save.bin"));
    if (!app.failure.empty()) return;
    th13::GameInput input;
    // step0 = destination 屏障帧（present 尾置 pending_load；Shift 预持——标题
    // 不消费 Shift，battle f0 时 held 已含 0x8 且无活沿）。
    input.update(kShift);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    CHECK_EQ(app.pending_load, 1u);
    CHECK_EQ(app.frames, 1u);
    // battle f0 = update-only 帧（begin_run 在本帧 complete_loading 内建真实
    // 录像流；Update 20 record_input 首捕获落挂起槽）。
    input.update(kShift | kZ);
    const u32 frames_pre = app.frames;
    CHECK(app.update(input, false));
    CHECK(app.recorder.ready());
    CHECK_EQ(app.record_edge_reset_pending_, false);  // 复位决定在捕获拍消费一次
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(1));
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
    if (app.recorder.pending_inputs.empty()) return;
    const auto& slot = app.recorder.pending_inputs.front();
    CHECK(slot.edge_reset);                       // D-fix 复位标志随槽
    CHECK_EQ(slot.frame, frames_pre);             // 帧身份 = update 段头同步的 frames
    CHECK_EQ(slot.frame, app.recorder.capture_frame);
    CHECK_EQ(u32(slot.input.held), kShift | kZ);  // 原始捕获输入
    CHECK_EQ(u32(slot.input.pressed), kZ);        // 活沿只有 Z（Shift 无新沿）
    CHECK_EQ(u32(slot.input.released), 0u);
    CHECK_EQ(slot.slow, expected_slow(app));      // 捕获拍 slow = 上一 fps 窗（60）
    CHECK_EQ(slot.slow, u8(60));
    // update-only 段流不变（捕获只落槽）。
    std::vector<u8> before;
    CHECK(encode_pinned(app, before));
    CHECK_EQ(total_frames(app), 0);
    CHECK_EQ(app.frames, frames_pre);  // frames 只在 commit 推进

    CHECK(app.commit(kFps));
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(0));  // 冲账即清槽
    CHECK_EQ(app.frames, frames_pre + 1u);
    std::vector<u8> after;
    CHECK(encode_pinned(app, after));
    CHECK(!bytes_equal(after, before));  // 记录落流
    // 冲账拍应用 D-fix 变换：帧 0 落盘 pressed=全量 held(0x9)、released=0——
    // 与活沿 pressed=0x1 可辨（反例：复位不随槽/不落盘时此处必假）。
    th13::ReplayFile decoded;
    CHECK(decoded.decode(after.data(), after.size()));
    CHECK_EQ(decoded.stages.size(), size_t(1));
    if (decoded.stages.empty()) return;
    const th13::ReplayStage& stage = decoded.stages.front();
    CHECK_EQ(stage.frame_count, 1u);
    CHECK_EQ(stage.number, u16(1));
    const th13::ReplayFrame f0 = stage.frame(0);
    CHECK_EQ(u32(f0.state), kShift | kZ);
    CHECK_EQ(u32(f0.pressed), kShift | kZ);  // D-fix：pressed = 全量 held
    CHECK_EQ(u32(f0.released), 0u);
    CHECK_EQ(stage.events.size(), size_t(1));
    if (!stage.events.empty()) CHECK_EQ(stage.events[0], u8(60));  // 捕获拍窗值
}

// ---- 2) 拆分前 step 与「update/commit 分调」encode 字节流逐字节一致 ----
// 工单步骤③的等价臂：复合 step（捕获/冲账同拍背靠背）vs 分调臂（update-only
// 窗延迟冲账 + 双/三 commit）——捕获拍随槽机制下录像流必须逐字节一致；
// 同帧重复 commit 不产生第二条记录（工单反例，encode/decode 双面验证）。
TEST(recorder_pending_split_step_byte_identity) {
    constexpr int kFrames = 24;
    App arm_s, arm_d;
    CHECK(boot_live_run(arm_s, "t3rec-arms-save.bin"));
    CHECK(boot_live_run(arm_d, "t3rec-armd-save.bin"));
    if (!arm_s.failure.empty() || !arm_d.failure.empty()) return;
    th13::GameInput in_s, in_d;
    // 各自的 step0 屏障帧（单 present，语义一致）。
    in_s.update(0);
    in_d.update(0);
    CHECK(arm_s.update(in_s, false));
    CHECK(arm_d.update(in_d, false));
    CHECK(arm_s.commit(kFps));
    CHECK(arm_d.commit(kFps));
    CHECK(arm_s.present());
    CHECK(arm_d.present());
    CHECK_EQ(arm_s.pending_load, 1u);
    CHECK_EQ(arm_d.pending_load, 1u);

    std::vector<CaptureLog> log_d;  // 分调臂捕获日志（等价期望来源）
    std::vector<u8> enc_last_commit;  // 上一次 commit 后的流字节（窗内不变基准）
    for (int f = 0; f < kFrames; ++f) {
        const u32 held = pattern(u32(f));
        in_s.update(held);
        in_d.update(held);
        const i32 stage_pre_s = arm_s.progress.stage_frame;
        const i32 stage_pre_d = arm_d.progress.stage_frame;
        // 臂 S：复合 step（原单席入口逐操作同序）。
        CHECK(arm_s.step(in_s, kFps, false));
        if (!arm_s.failure.empty()) {
            std::printf("  失败臂S f%d: %s\n", f, arm_s.failure.c_str());
            return;
        }
        CHECK_EQ(arm_s.progress.stage_frame, stage_pre_s + 1);  // 全窗推进
        // 臂 D：分调。f8..f10 update-only 窗（延迟冲账）；f16 双 commit；
        // f20 三 commit；其余单 commit + 单 present。
        CHECK(arm_d.update(in_d, false));
        if (!arm_d.failure.empty()) {
            std::printf("  失败臂D f%d: %s\n", f, arm_d.failure.c_str());
            return;
        }
        CHECK_EQ(arm_d.progress.stage_frame, stage_pre_d + 1);
        if (arm_d.recorder.pending_inputs.empty()) {
            std::printf("  失败臂D f%d: update 后挂起槽为空（record_input 捕获丢失）\n", f);
        }
        CHECK(!arm_d.recorder.pending_inputs.empty());
        if (!arm_d.recorder.pending_inputs.empty()) {
            const auto& slot = arm_d.recorder.pending_inputs.back();
            CaptureLog entry;
            entry.input = slot.input;
            entry.slow = slot.slow;
            entry.edge_reset = slot.edge_reset;
            entry.frame = slot.frame;
            log_d.push_back(entry);
            CHECK_EQ(slot.frame, arm_d.frames);            // 帧身份随槽
            CHECK_EQ(slot.slow, expected_slow(arm_d));     // 捕获拍窗值随槽
        }
        const i32 mirror_pre = total_frames(arm_d);
        std::vector<u8> stream_pre;
        CHECK(encode_pinned(arm_d, stream_pre));
        if (f >= 8 && f <= 10) {
            // update-only 窗（f0..f7 已各自 commit）：镜像/dirty/frames 全部
            // 保持窗前值（反例：update-only 段 total_frames 已 +1 即机制不成
            // 立——窗内三轮镜像恒 8）。
            CHECK_EQ(total_frames(arm_d), 8);
            CHECK_EQ(mirror_pre, 8);
            CHECK_EQ(arm_d.ordinary_records_dirty, true);  // f0 commit 已置位并保持
            CHECK_EQ(arm_d.pending_play_frames_, u32(f - 7));  // 窗内计数累计
            CHECK_EQ(u32(arm_d.recorder.pending_inputs.size()), u32(f - 7));
            CHECK_EQ(arm_d.frames, 9u);  // frames 不动（commit 才推进）
            std::vector<u8> stream_now;
            CHECK(encode_pinned(arm_d, stream_now));
            CHECK(bytes_equal(stream_now, enc_last_commit));  // 捕获只落槽，流不变
        } else {
            const int commits = f == 16 ? 2 : (f == 20 ? 3 : 1);
            std::vector<u8> after_first;
            for (int c = 0; c < commits; ++c) {
                CHECK(arm_d.commit(kFps));
                if (!arm_d.failure.empty()) return;
                if (c == 0) {
                    CHECK(encode_pinned(arm_d, after_first));
                    CHECK(!bytes_equal(after_first, stream_pre));  // 记录落流
                    CHECK_EQ(arm_d.recorder.pending_inputs.size(), size_t(0));
                    CHECK_EQ(arm_d.pending_play_frames_, 0u);
                    CHECK_EQ(total_frames(arm_d), f + 1);  // 恰把 0..f 全部冲账一次
                    CHECK_EQ(arm_d.ordinary_records_dirty, true);
                    enc_last_commit = after_first;
                } else {
                    // 同帧重复 commit：槽已清 → 不产生第二条录像记录。
                    std::vector<u8> after_again;
                    CHECK(encode_pinned(arm_d, after_again));
                    CHECK(bytes_equal(after_again, after_first));
                    CHECK_EQ(total_frames(arm_d), f + 1);  // 镜像不再 +1
                    CHECK_EQ(arm_d.recorder.pending_inputs.size(), size_t(0));
                    CHECK_EQ(arm_d.pending_play_frames_, 0u);
                }
            }
            CHECK(arm_d.present());
        }
    }
    CHECK_EQ(size_t(log_d.size()), size_t(kFrames));
    // 终态逐字节等价：两臂 encode 一致 + decode 全字段一致。
    std::vector<u8> enc_s, enc_d;
    CHECK(encode_pinned(arm_s, enc_s));
    CHECK(encode_pinned(arm_d, enc_d));
    if (!bytes_equal(enc_s, enc_d)) {
        std::printf("  失败两臂 encode 长度 S=%zu D=%zu\n", enc_s.size(), enc_d.size());
    }
    CHECK(bytes_equal(enc_s, enc_d));
    CHECK_EQ(total_frames(arm_s), kFrames);
    CHECK_EQ(total_frames(arm_d), kFrames);  // 双/三 commit 不重复冲账
    th13::ReplayFile dec_s, dec_d;
    CHECK(dec_s.decode(enc_s.data(), enc_s.size()));
    CHECK(dec_d.decode(enc_d.data(), enc_d.size()));
    CHECK_EQ(dec_d.stages.size(), size_t(1));
    CHECK_EQ(dec_s.stages.size(), size_t(1));
    if (dec_s.stages.empty() || dec_d.stages.empty()) return;
    CHECK_EQ(dec_d.stages.front().frame_count, u32(kFrames));
    CHECK(dec_d.stages.front().input == dec_s.stages.front().input);
    CHECK(dec_d.stages.front().events == dec_s.stages.front().events);
    CHECK(dec_d.stages.front().header == dec_s.stages.front().header);
    // 分调臂 decode 逐帧与捕获日志一致（D-fix 帧变换在冲账拍应用）。
    const th13::ReplayStage& stage = dec_d.stages.front();
    for (size_t i = 0; i < log_d.size(); ++i) {
        const th13::ReplayFrame frame = stage.frame(u32(i));
        u32 pressed = log_d[i].input.pressed;
        u32 released = log_d[i].input.released;
        if (log_d[i].edge_reset) {
            pressed = log_d[i].input.held;
            released = 0;
        }
        if (u32(frame.state) != log_d[i].input.held || u32(frame.pressed) != pressed ||
            u32(frame.released) != released) {
            std::printf("  失败帧%zu: state=%u pressed=%u released=%u（期望 held=%u pressed=%u released=%u edge=%d）\n",
                        i, frame.state, frame.pressed, frame.released,
                        log_d[i].input.held, pressed, released, int(log_d[i].edge_reset));
        }
        CHECK_EQ(u32(frame.state), log_d[i].input.held);
        CHECK_EQ(u32(frame.pressed), pressed);
        CHECK_EQ(u32(frame.released), released);
    }
}

// ---- 3) slow 字节源 = 捕获拍上一 fps 窗（窗口变化后事件字节随之演进）----
// fps 序：f0..f64@60 → f65..f67@0.5（f65 commit 起结算窗被 2.0s 拍距拉低）→
// f68..f134@60（60 个 @60 commit 后恢复 60）。事件字节（每 30 录制帧一拍）必
// 须反映捕获拍窗值：前 3 拍 60、0.5fps 结算后（R=90/120 拍）≠60、恢复后回到
// 60。反例钉死：源改成恒 60 或 last_fps_（f90 拍 last_fps_=60）都会在
// tick3/tick4 错位；移到 commit 重算同拍同值不构成此差异（flush 先于结算，
// 捕获拍与冲账拍读数同源——该序由 commit 固定序测试钉住）。
TEST(recorder_pending_slow_source_capture_window) {
    constexpr int kFrames = 135;
    App app;
    CHECK(boot_live_run(app, "t3rec-slow-save.bin"));
    if (!app.failure.empty()) return;
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    std::vector<CaptureLog> log;
    std::vector<u8> enc_pre_window;
    bool settled_low = false;
    for (int f = 0; f < kFrames; ++f) {
        const float fps = (f <= 64) ? 60.0f : (f <= 67 ? 0.5f : 60.0f);
        input.update(f % 2 == 0 ? kZ : 0u);  // 简单 Z 沿（无音频断言需求）
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
        CHECK(!app.recorder.pending_inputs.empty());
        if (app.recorder.pending_inputs.empty()) return;
        const auto& slot = app.recorder.pending_inputs.back();
        CaptureLog entry;
        entry.input = slot.input;
        entry.slow = slot.slow;
        entry.edge_reset = slot.edge_reset;
        entry.frame = slot.frame;
        log.push_back(entry);
        // 捕获拍随槽值 = 捕获拍上一 fps 窗（:866 同式）。
        CHECK_EQ(slot.slow, expected_slow(app));
        if (f == 64) CHECK(encode_pinned(app, enc_pre_window));
        CHECK(app.commit(fps));
        if (!app.failure.empty()) return;
        if (f == 65 && app.slow_window_fps_ < 60.0f) settled_low = true;
        CHECK(app.present());
    }
    CHECK(settled_low);                            // 0.5fps commit 结算拉低窗值
    CHECK_NEAR(app.slow_window_fps_, 60.0, 0.01);  // 恢复段已重新结算
    CHECK_EQ(size_t(log.size()), size_t(kFrames));
    std::vector<u8> enc;
    CHECK(encode_pinned(app, enc));
    CHECK(!bytes_equal(enc, enc_pre_window));
    th13::ReplayFile decoded;
    CHECK(decoded.decode(enc.data(), enc.size()));
    CHECK_EQ(decoded.stages.size(), size_t(1));
    if (decoded.stages.empty()) return;
    const th13::ReplayStage& stage = decoded.stages.front();
    CHECK_EQ(stage.frame_count, u32(kFrames));
    // 事件字节 = 对应捕获拍 slow（帧 30t 的随槽值）。
    CHECK_EQ(stage.events.size(), size_t((kFrames + 29) / 30));
    for (size_t t = 0; t < stage.events.size() && size_t(30 * t) < log.size(); ++t)
        CHECK_EQ(stage.events[t], log[size_t(30 * t)].slow);
    // 锚点（对 float 累积累计不敏感的判据）：60fps 段全 60；0.5fps 结算后
    // R=90/120 拍 ≠60（「恒 60」与 last_fps_ 源的反例位）；恢复段末尾回 60。
    CHECK_EQ(stage.events[0], u8(60));
    CHECK_EQ(stage.events[1], u8(60));
    CHECK_EQ(stage.events[2], u8(60));
    CHECK_EQ(log[65].slow, u8(60));  // f65 捕获拍窗尚未结算
    CHECK(log[66].slow < u8(60));    // 结算后捕获拍随槽值演进（2~3）
    CHECK(stage.events[3] != u8(60));   // R=90（f90 捕获拍窗值已拉低）
    CHECK(stage.events[4] != u8(60));   // R=120
    CHECK_EQ(log.back().slow, u8(60));  // 恢复段末尾捕获拍回 60
}

// ---- 4) update_spell_time 钟值随槽、commit 冲账、钟值单次捕获 ----
// 生产宿主方法直调（RunScene Update 20 同一入口；全战局驱动由 e2e 回放等价
// 覆盖）。sleep(20ms) 隔开捕获拍与冲账拍：encode_spell_elapsed_time 以
// 0.0167s 量化——冲账拍若重算钟值会得到不同 quantized 值，随槽冲账则逐位
// 保持捕获拍值（「钟值捕获一次、重算复用」的直接反例探针）。
TEST(recorder_pending_spell_time_slot_captured_once) {
    App app;
    CHECK(boot_live_run(app, "t3rec-spell-save.bin"));
    if (!app.failure.empty()) return;
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    for (int f = 0; f < 2; ++f) {
        input.update(kZ);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());
    }
    CHECK(app.recorder.ready());
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
    std::vector<u8> baseline;
    CHECK(encode_pinned(app, baseline));

    // 生产宿主方法（ApplicationGame.cpp:54-97）：Begin→End 同拍成对恰一次结束。
    app.update_spell_time(true);
    app.update_spell_time(false);
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(1));
    if (app.recorder.pending_spell_times.size() != 1) return;
    const auto& entry = app.recorder.pending_spell_times.front();
    CHECK_EQ(entry.index, 0u);  // stage 头 0x70 + 4·index 首槽
    CHECK(entry.index < 21u);
    CHECK_EQ(entry.packed, app.spell_time_packed);  // 钟值捕获一次随槽
    CHECK(th13::spell_time_packed_valid(entry.packed));
    CHECK_EQ(entry.frame, app.recorder.capture_frame);  // 帧身份盖章
    std::vector<u8> before;
    CHECK(encode_pinned(app, before));
    CHECK(bytes_equal(before, baseline));  // update 段流不变（钟值只落槽）

    // 隔开 20ms 再冲账：随槽冲账保持捕获拍值；重算会漂移（量化不同拍）。
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    CHECK(app.commit(kFps));
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));  // 冲账清槽
    CHECK_EQ(app.spell_time_packed, entry.packed);  // 钟值不被 commit 重算
    std::vector<u8> after;
    CHECK(encode_pinned(app, after));
    CHECK(!bytes_equal(after, baseline));
    th13::ReplayFile decoded;
    CHECK(decoded.decode(after.data(), after.size()));
    CHECK_EQ(decoded.stages.size(), size_t(1));
    if (decoded.stages.empty()) return;
    const th13::ReplayStage& stage = decoded.stages.front();
    CHECK(stage.header.size() >= size_t(0xC4));
    if (stage.header.size() < 0xC4) return;
    const u32 stored = u32(stage.header[0x70]) | u32(stage.header[0x71]) << 8 |
                       u32(stage.header[0x72]) << 16 | u32(stage.header[0x73]) << 24;
    CHECK_EQ(i32(stored), entry.packed);  // 落流 = 捕获拍钟值（非重算值）
    // 第二对 Begin/End：index 推进 + 同帧双 commit 幂等（槽冲账即清）。
    app.update_spell_time(true);
    app.update_spell_time(false);
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(1));
    if (app.recorder.pending_spell_times.size() == 1) {
        CHECK_EQ(app.recorder.pending_spell_times.front().index, 1u);
        CHECK_EQ(app.recorder.pending_spell_times.front().packed,
                 app.spell_time_packed);
    }
    std::vector<u8> after_first;
    CHECK(app.commit(kFps));
    CHECK(encode_pinned(app, after_first));
    CHECK(!bytes_equal(after_first, after));
    CHECK(app.commit(kFps));  // 同帧双 commit
    std::vector<u8> after_double;
    CHECK(encode_pinned(app, after_double));
    CHECK(bytes_equal(after_double, after_first));  // 不产生第二条 spell 记录
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
}

// ---- 5) 捕获拍同步校验保留（!ready() 失败在捕获拍、槽不入条目）----
// 反例钉死：ready 校验若被移到冲账拍，捕获拍会推入槽条目（post-update 槽
// 非空）且失败拍漂移到 commit。原语义（拆分前 :843-858 record 直写失败）=
// 消息逐字、失败帧照常完成（fail 不中断当帧 update/commit/present）、下一帧
// 入口守卫拦停。
TEST(recorder_pending_capture_gate_sync_semantics) {
    App app;
    CHECK(boot_live_run(app, "t3rec-gate-save.bin"));
    if (!app.failure.empty()) return;
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    input.update(kZ);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    CHECK(app.recorder.ready());
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(0));

    // 生产公开 API（ReplayRecorder::clear）：流退化为 !ready——捕获拍同步
    // 校验的失败条件（挂起槽宿主注释：!ready() ⟺ stages 空）。
    app.recorder.stream.clear();
    CHECK(!app.recorder.ready());
    input.update(kZ);
    const u32 frames_pre = app.frames;
    const i32 mirror_pre = total_frames(app);
    CHECK(app.update(input, false));  // 失败设 fail 但不中断当帧（原语义）
    CHECK_EQ(app.failure, std::string("Replay input recording failed"));
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(0));  // 槽不入条目
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
    // 当帧 commit 照常（无挂起条目可冲账；播放帧/音频/frames 正常）。
    CHECK(app.commit(kFps));
    CHECK_EQ(app.frames, frames_pre + 1u);
    CHECK_EQ(total_frames(app), mirror_pre + 1);  // 播放帧冲账不受捕获失败影响
    // 下一帧入口守卫拦停泵（fail 终态无后续消费）。
    input.update(0);
    CHECK(!app.update(input, false));
    CHECK(!app.step(input, kFps, false));
    CHECK_EQ(app.frames, frames_pre + 1u);  // 后续帧 commit 不再执行
}

int main() { return th13test::run_all("m1.unit_recorder_pending_once"); }
