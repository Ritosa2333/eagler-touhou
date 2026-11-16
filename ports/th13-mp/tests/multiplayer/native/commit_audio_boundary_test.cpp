// m1.unit_commit_audio_boundary —— M1/T3 commit 固定冲账序 + 音频边界独立
// 回归（工单步骤④：录制槽→存档队列(T4)→pan 队(T4)→播放帧→
// audio_device.update()→frames++→fps 窗→caption tick；证伪『fail 后无后续
// 消费』即 BLOCKED 不记偏差）。
//
// 被测生产面（全部 public，零生产钩子；实现=Application.cpp:1470-1502 commit
// 固定序、:1425-1446 录制槽冲账、:1453-1462 播放帧冲账、AudioDevice.cpp
// request_observer 只读 tap + statistics()）。
//
// 断言命题：
//   1) update-only 段音频请求只入队不启动不混音（requests/pending_voices 增、
//      started/mixed_frames/stopped 不变）；commit 恰推进一拍（12 通道表排空、
//      started/stopped 恰按通道模型增量、frames 恰 +1）；同帧双 commit 零第二
//      次音频消费；
//   2) 三段复合 step 与 update/commit 分调的 request_observer tap 序列逐事件
//      一致（拆分不重排/不丢/不重复音频请求）；
//   3) commit 固定序截断：播放帧失败（character 越界→生产 record_play_frame
//      返 false→fail("Invalid play time record")）时录制槽已冲账（先于播放
//      帧）、音频/frames++/fps 窗/caption tick 全部不执行、step 短路 present、
//      下一帧入口守卫拦停；录制槽失败（stream 退化）时播放帧也不冲账——两级
//      截断直接可证；
//   4) 对照臂证明被截断的消费在该帧真实会发生（caption tick / 音频排空 /
//      镜像 +N），非空断言。
//
// live-run 走 start_game(1) → 合成输入真实链（Bomb/蓄声触发生产 SE 请求），不
// 建假设备；混音在 mixer-only 诊断态不发生（mix 只由外部泵驱动）——
// mixed_frames 恒定是「update/commit 不混音」的直接钉面。
#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/ReplayFile.hpp"
#include "sdl/ApplicationState.hpp"
#include "state_digest.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <string>
#include <vector>

namespace {

using th13::u8;
using th13::u16;
using th13::u32;
using th13::i32;
using App = th13::sdl::ApplicationState;
using Audio = th13::sdl::AudioDevice;
using th13::sdl::Destination;

constexpr u16 kBootSeed = 0x1B2C;
constexpr float kFps = 60.0f;
constexpr u32 kZ = 0x01;
constexpr u32 kX = 0x02;      // Bomb（生产启动 SE：44 定位 + 49）
constexpr u32 kShift = 0x08;  // 低速（妖夢蓄声 30/55 pan 的生产请求源）

const char* repo_root() {
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return root && *root ? root : "../../..";
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

bool boot_live_run(App& app, const char* save_name) {
    if (!pin_boot(app, save_name)) return false;
    if (!app.start_game(1)) {
        std::printf("  失败 boot: start_game: %s\n", app.failure.c_str());
        return false;
    }
    return app.pending_destination == i32(Destination::Run) && app.failure.empty();
}

bool obs_equal(const Audio::RequestObservation& a, const Audio::RequestObservation& b) {
    return a.kind == b.kind && a.id == b.id && a.value == b.value &&
           a.positional == b.positional && a.x == b.x && a.text == b.text;
}

// 请求 tap 日志（observer 只读；跨臂逐事件对账用）。
struct TapLog {
    std::vector<Audio::RequestObservation> items;
    void attach(App& app) {
        app.audio_device.request_observer = [this](const Audio::RequestObservation& obs) {
            items.push_back(obs);
        };
    }
    size_t mark() const { return items.size(); }
};

// 12 通道帧表结算模型（AudioDevice.cpp:349-431 排队 / :510-542 排空的公开
// 可观测语义）：窗口 = 两次 commit 之间的 Voice/Stop 请求。
//   pending_voices（update 后）= Voice∪Stop 的非负 id 数（各占一通道槽；同窗
//   口同 id 的 Stop 赢、Start 被丢）；started（commit 后）= 仅 Voice 窗的 id
//   数；stop(id<0)（release_run 全停，:406）当场 stopped++ 且全部在播声立停，
//   使同窗后续 marker 排空不再 ++stopped；stopped(marker) = 无 stop(-1) 时
//   「Stop 窗且该 id 声音在播」的 id 数。mixer-only 诊断态 mix 不运行、非循
//   环声不自然退役 → 「在播」= 已启动未停止，模型精确。
struct ChannelModel {
    std::set<i32> playing;
    struct Expect {
        size_t pending = 0, started = 0, marker_stopped = 0;
        size_t immediate_stopped = 0;  // stop(id<0) 在 tap 拍当场生效
        size_t voice_requests = 0;
        size_t stopped_total() const { return immediate_stopped + marker_stopped; }
    };
    Expect observe(const Audio::RequestObservation* taps, size_t count) const {
        std::set<i32> voice_ids, stop_ids;
        Expect e;
        bool all_stop = false;
        for (size_t i = 0; i < count; ++i) {
            const Audio::RequestObservation& obs = taps[i];
            if (obs.kind == Audio::RequestObservation::Kind::Voice) {
                ++e.voice_requests;  // queue_voice 对任意 id（含负）++requests
                if (obs.id >= 0) voice_ids.insert(obs.id);
            } else if (obs.kind == Audio::RequestObservation::Kind::Stop) {
                if (obs.id < 0) {
                    ++e.immediate_stopped;  // :406 当场 ++stopped
                    all_stop = true;        // 全部在播声立停
                } else {
                    stop_ids.insert(obs.id);
                }
            }
        }
        std::set<i32> touched(voice_ids);
        touched.insert(stop_ids.begin(), stop_ids.end());
        e.pending = touched.size();
        for (i32 id : voice_ids)
            if (!stop_ids.count(id)) ++e.started;
        if (!all_stop)
            for (i32 id : stop_ids)
                if (playing.count(id)) ++e.marker_stopped;
        return e;
    }
    void drain(const Audio::RequestObservation* taps, size_t count) {
        std::set<i32> voice_ids, stop_ids;
        bool all_stop = false;
        for (size_t i = 0; i < count; ++i) {
            const Audio::RequestObservation& obs = taps[i];
            if (obs.kind == Audio::RequestObservation::Kind::Voice && obs.id >= 0)
                voice_ids.insert(obs.id);
            else if (obs.kind == Audio::RequestObservation::Kind::Stop) {
                if (obs.id < 0) all_stop = true;
                else stop_ids.insert(obs.id);
            }
        }
        if (all_stop) playing.clear();  // stop(-1) 已停全部在播声
        for (i32 id : voice_ids)
            if (!stop_ids.count(id)) playing.insert(id);  // 排空拍新启动
        for (i32 id : stop_ids) playing.erase(id);
    }
};

i32 total_frames_at(const App& app, i32 character) {
    return app.files.ordinary_records[size_t(character)].total_frames;
}

bool bytes_equal(const std::vector<u8>& a, const std::vector<u8>& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}

bool encode_pinned(const App& app, std::vector<u8>& out) {
    th13::ReplaySaveMeta meta;
    meta.date = 1;
    meta.slow_numerator = 0.0;
    meta.slow_denominator = 1099511627776.0;
    return app.recorder.encode(out, "t3ab", 0, meta);
}

// Draw 段探针（签名测试同款；优先级 73 空闲）。
struct DrawProbe {
    th13::FrameCallback callback{};
    App* app = nullptr;
    th13::FrameScheduler* attached_scheduler = nullptr;
    int fires = 0;
    static i32 run(void* p) {
        auto& probe = *static_cast<DrawProbe*>(p);
        ++probe.fires;
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

}  // namespace

// ---- 1) update-only 只入队不启动不混音；commit 恰推进一拍（逐帧通道模型对账）----
TEST(commit_audio_boundary_update_only_queue_commit_drain) {
    App app;
    CHECK(boot_live_run(app, "t3ab-drain-save.bin"));
    if (!app.failure.empty()) return;
    app.progress.character = 3;  // 妖夢：蓄声(30/55 pan)与 Bomb(44/49/27) 生产请求源
    TapLog log;
    log.attach(app);
    th13::GameInput input;
    // step0 = destination 屏障帧。
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    CHECK_EQ(app.pending_load, 1u);
    ChannelModel model;
    const Audio::Statistics boot_stats = app.audio_device.statistics();
    size_t window_begin = log.mark();
    int windows_with_pending = 0, windows_with_starts = 0;
    size_t max_pending = 0;
    for (int f = 0; f < 30; ++f) {
        // battle f10 按 X 开 Bomb（44/49 启动请求）；Shift+Z 自 f10 常按
        // （startup>19 后 f20 起蓄声 30/55 pan）。
        const u32 held = (f == 10 ? (kShift | kZ | kX) : (f >= 10 ? (kShift | kZ) : kZ));
        input.update(held);
        const u32 frames_pre = app.frames;
        const Audio::Statistics pre = app.audio_device.statistics();
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
        const size_t window_end = log.mark();
        const ChannelModel::Expect e =
            model.observe(log.items.data() + window_begin, window_end - window_begin);
        // update-only 段：请求已入队（requests/pending_voices 增），但零启动、
        // 零混音；stopped 只因 stop(id<0) 在 tap 拍当场生效（release_run 全停）。
        const Audio::Statistics mid = app.audio_device.statistics();
        CHECK_EQ(mid.requests, pre.requests + e.voice_requests);
        CHECK_EQ(u32(mid.pending_voices), u32(e.pending));
        CHECK_EQ(mid.started, pre.started);
        CHECK_EQ(mid.stopped, pre.stopped + e.immediate_stopped);
        CHECK_EQ(mid.mixed_frames, pre.mixed_frames);
        CHECK_EQ(mid.dropped_requests, pre.dropped_requests);
        if (e.pending > 0) ++windows_with_pending;
        if (e.started > 0) ++windows_with_starts;
        max_pending = std::max(max_pending, e.pending);
        CHECK(app.commit(kFps));
        if (!app.failure.empty()) return;
        // commit 恰推进一拍：通道表排空、started/stopped 恰按模型增量、
        // frames 恰 +1、混音仍为零。
        const Audio::Statistics post = app.audio_device.statistics();
        CHECK_EQ(u32(post.pending_voices), 0u);
        CHECK_EQ(post.started, pre.started + e.started);
        CHECK_EQ(post.stopped, pre.stopped + e.stopped_total());
        CHECK_EQ(post.mixed_frames, pre.mixed_frames);
        CHECK_EQ(post.dropped_requests, pre.dropped_requests);
        CHECK_EQ(app.frames, frames_pre + 1u);
        model.drain(log.items.data() + window_begin, window_end - window_begin);
        CHECK(app.present());
        if (!app.failure.empty()) return;
        window_begin = log.mark();
    }
    // 非空断言：测量窗确有真实请求/启动/多通道窗口（防恒真）。
    CHECK(windows_with_pending >= 1);
    CHECK(windows_with_starts >= 1);
    CHECK(max_pending >= 2);  // Bomb 启动帧 44+49 两 id 同窗
    CHECK_EQ(app.audio_device.statistics().mixed_frames, boot_stats.mixed_frames);
}

// ---- 2) 同帧双 commit：音频恰一拍消费（第二拍零消费）----
TEST(commit_audio_boundary_double_commit_single_beat) {
    App app;
    CHECK(boot_live_run(app, "t3ab-double-save.bin"));
    if (!app.failure.empty()) return;
    app.progress.character = 3;
    TapLog log;
    log.attach(app);
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    ChannelModel model;
    size_t window_begin = log.mark();
    int guarded_at = -1;
    for (int f = 0; f < 30 && guarded_at < 0; ++f) {
        const u32 held = (f == 10 ? (kShift | kZ | kX) : (f >= 10 ? (kShift | kZ) : kZ));
        input.update(held);
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
        const size_t window_end = log.mark();
        const ChannelModel::Expect e =
            model.observe(log.items.data() + window_begin, window_end - window_begin);
        CHECK(app.commit(kFps));
        if (!app.failure.empty()) return;
        model.drain(log.items.data() + window_begin, window_end - window_begin);
        CHECK(app.present());
        window_begin = log.mark();
        if (e.started >= 2) {
            // 已提交拍之后再补一拍（同帧双 commit 反例位）：通道表已空 →
            // started/stopped/requests/dropped/mixed 全部零变化。
            const Audio::Statistics after_first = app.audio_device.statistics();
            const u32 frames_after_first = app.frames;
            CHECK(app.commit(kFps));
            const Audio::Statistics after_second = app.audio_device.statistics();
            CHECK_EQ(after_second.started, after_first.started);
            CHECK_EQ(after_second.stopped, after_first.stopped);
            CHECK_EQ(u32(after_second.pending_voices), 0u);
            CHECK_EQ(after_second.requests, after_first.requests);
            CHECK_EQ(after_second.dropped_requests, after_first.dropped_requests);
            CHECK_EQ(after_second.mixed_frames, after_first.mixed_frames);
            CHECK_EQ(app.frames, frames_after_first + 1u);  // 每拍 frames 恰 +1
            guarded_at = f;
        }
    }
    CHECK(guarded_at >= 0);  // 30 帧窗内确有多启动窗（防恒真）
}

// ---- 3) 三段复合 step 与 update/commit 分调的 tap 序列逐事件一致 ----
TEST(commit_audio_boundary_split_step_tap_sequence_identical) {
    constexpr int kFrames = 30;
    App arm_s, arm_d;
    CHECK(boot_live_run(arm_s, "t3ab-taps-save.bin"));
    CHECK(boot_live_run(arm_d, "t3ab-tapd-save.bin"));
    if (!arm_s.failure.empty() || !arm_d.failure.empty()) return;
    arm_s.progress.character = 3;
    arm_d.progress.character = 3;
    TapLog log_s, log_d;
    log_s.attach(arm_s);
    log_d.attach(arm_d);
    th13::GameInput in_s, in_d;
    in_s.update(0);
    in_d.update(0);
    CHECK(arm_s.update(in_s, false));
    CHECK(arm_d.update(in_d, false));
    CHECK(arm_s.commit(kFps));
    CHECK(arm_d.commit(kFps));
    CHECK(arm_s.present());
    CHECK(arm_d.present());
    for (int f = 0; f < kFrames; ++f) {
        const u32 held = (f == 10 ? (kShift | kZ | kX) : (f >= 10 ? (kShift | kZ) : kZ));
        in_s.update(held);
        in_d.update(held);
        CHECK(arm_s.step(in_s, kFps, false));  // 复合：捕获/冲账/音频拍同序背靠背
        if (!arm_s.failure.empty()) {
            std::printf("  失败臂S f%d: %s\n", f, arm_s.failure.c_str());
            return;
        }
        CHECK(arm_d.update(in_d, false));      // 分调：f8..f9 update-only 延迟冲账
        if (!arm_d.failure.empty()) {
            std::printf("  失败臂D f%d: %s\n", f, arm_d.failure.c_str());
            return;
        }
        if (f != 8 && f != 9) {
            CHECK(arm_d.commit(kFps));  // f10 排空窗内全部请求（8/9/10 三帧）
            CHECK(arm_d.present());
        }
        if (!arm_d.failure.empty()) return;
    }
    // tap 逐事件一致（kind/id/value/positional/x/text 全字段、同序同数）。
    if (log_s.items.size() != log_d.items.size()) {
        std::printf("  失败 tap 数量 S=%zu D=%zu\n", log_s.items.size(), log_d.items.size());
    }
    CHECK_EQ(log_s.items.size(), log_d.items.size());
    for (size_t i = 0; i < log_s.items.size() && i < log_d.items.size(); ++i) {
        if (!obs_equal(log_s.items[i], log_d.items[i])) {
            std::printf("  失败 tap[%zu] 不一致\n", i);
        }
        CHECK(obs_equal(log_s.items[i], log_d.items[i]));
    }
    // 终态统计一致（排空拍数不同不影响总量）。
    const Audio::Statistics s = arm_s.audio_device.statistics();
    const Audio::Statistics d = arm_d.audio_device.statistics();
    CHECK_EQ(s.started, d.started);
    CHECK_EQ(s.requests, d.requests);
    CHECK_EQ(s.dropped_requests, d.dropped_requests);
    CHECK_EQ(s.stopped, d.stopped);
    // 录像流同样逐字节一致（录制槽与音频共用同一 commit 冲账序）。
    std::vector<u8> enc_s, enc_d;
    CHECK(encode_pinned(arm_s, enc_s));
    CHECK(encode_pinned(arm_d, enc_d));
    CHECK(bytes_equal(enc_s, enc_d));
}

// ---- 4) commit 固定序截断：播放帧失败 → 录制槽已冲账、后续全不执行 ----
// 反例证伪面：若『fail 后无后续消费』不成立（冲账失败仍 audio_device.update/
// frames++/fps 窗/caption tick/step 继续），下列任一断言为假。
// 截断位取 battle f10（stage_frame=11）：该帧 commit 的 caption 段是 live boot
// 装载树唯一的可观测消费点（battle_frame==10 → retire，loading_prayer→0；
// live boot 无 3 装载帧预热带，副链空、f1..f9 tick 无可观测变化——T2 签名
// 回放臂钉 tick 相位，本臂用 retire 位做截断对照）。妖夢 Bomb 于 f9 启动
// （启动帧 SE 44/49 + 生成帧 44/27 同窗）；f10 窗的音频工作 = startup 期
// stop(30/55) 位已入队——真实 audio_device.update() 必排空（pending 2→0），
// 截断后保持非零，音频跳过可辨。
TEST(commit_audio_boundary_commit_order_truncation_play_frames) {
    // 对照臂：同一帧正常 commit 的真实消费（证明被截断面非空）。
    {
        App control;
        CHECK(boot_live_run(control, "t3ab-ordctl-save.bin"));
        if (!control.failure.empty()) return;
        control.progress.character = 3;  // 妖夢：Bomb 每帧生成声 44/27
        TapLog log;
        log.attach(control);
        th13::GameInput input;
        input.update(0);
        CHECK(control.update(input, false));
        CHECK(control.commit(kFps));
        CHECK(control.present());
        ChannelModel model;
        size_t window_begin = log.mark();
        for (int f = 0; f <= 10; ++f) {
            input.update(f == 9 ? (kZ | kX) : kZ);  // f9 按 X 开 Bomb（启动帧 SE 44/49 + 生成 44/27）
            const Audio::Statistics pre_update = control.audio_device.statistics();
            CHECK(control.update(input, false));
            if (!control.failure.empty()) return;
            const size_t window_end = log.mark();
            const ChannelModel::Expect e =
                model.observe(log.items.data() + window_begin, window_end - window_begin);
            const Audio::Statistics pre_commit = control.audio_device.statistics();
            CHECK_EQ(control.progress.stage_frame, f + 1);
            CHECK_EQ(control.pending_play_frames_, 1u);
            CHECK_EQ(u32(pre_commit.pending_voices), u32(e.pending));
            CHECK_EQ(pre_commit.started, pre_update.started);         // update 段零启动
            CHECK_EQ(pre_commit.stopped,
                     pre_update.stopped + e.immediate_stopped);       // stop(-1) 当场生效
            CHECK(control.commit(kFps));
            if (!control.failure.empty()) return;
            const Audio::Statistics post = control.audio_device.statistics();
            CHECK_EQ(u32(post.pending_voices), 0u);              // 音频排空
            CHECK_EQ(post.started, pre_update.started + e.started);     // 恰一拍启动
            CHECK_EQ(post.stopped, pre_update.stopped + e.stopped_total());
            CHECK_EQ(post.mixed_frames, pre_update.mixed_frames);
            CHECK_EQ(total_frames_at(control, 3), f + 1);        // 镜像恰 +1（妖夢席位）
            CHECK_EQ(control.ordinary_records_dirty, true);
            CHECK_EQ(control.frames, u32(f) + 2u);               // 屏障帧 +1
            if (f == 10) {
                // battle_frame==10：caption retire 消费点——prayer 清零、树标记。
                CHECK(control.loading_prayer == 0);
            }
            model.drain(log.items.data() + window_begin, window_end - window_begin);
            window_begin = log.mark();
            CHECK(control.present());
        }
        CHECK_EQ(control.frames, 12u);  // 屏障帧 + 11 对局帧
    }
    // 截断臂：battle f10 commit 前注入越界 character（生产 record_play_frame
    // 返 false → fail("Invalid play time record")——真实生产失败路径，非伪造
    // 设备）。该帧待冲账面：录制槽 1 条、播放帧 1 帧、Bomb 生成声排空、
    // caption retire——四项全部真实可消费。
    App app;
    CHECK(boot_live_run(app, "t3ab-ordfail-save.bin"));
    if (!app.failure.empty()) return;
    app.progress.character = 3;
    TapLog log;
    log.attach(app);
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    ChannelModel model;
    size_t window_begin = log.mark();
    for (int f = 0; f <= 10; ++f) {
        input.update(f == 9 ? (kZ | kX) : kZ);
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
        if (f == 10) break;  // 截断位：本帧不 commit
        const size_t window_end = log.mark();
        CHECK(app.commit(kFps));
        if (!app.failure.empty()) return;
        model.drain(log.items.data() + window_begin, window_end - window_begin);
        window_begin = log.mark();
        CHECK(app.present());
    }
    // battle f10 update 后（commit 前）基线。
    const size_t window_end = log.mark();
    const ChannelModel::Expect e =
        model.observe(log.items.data() + window_begin, window_end - window_begin);
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(1));
    CHECK_EQ(app.pending_play_frames_, 1u);
    CHECK_EQ(app.progress.stage_frame, 11);
    CHECK_EQ(app.loading_prayer != 0, true);   // caption retire 待消费
    CHECK(e.pending >= 1);                     // 音频排空工作非空（stop 位已入队）
    const Audio::Statistics pre_stats = app.audio_device.statistics();
    CHECK_EQ(u32(pre_stats.pending_voices), u32(e.pending));
    const u32 pre_frames = app.frames;
    const double pre_window_time = app.slow_window_time_;
    const u32 pre_window_frames = app.slow_window_frames_;
    const uint64_t pre_caption = mpdigest::digest_loading_caption(app);
    const i32 pre_mirror = total_frames_at(app, 3);  // 妖夢席位镜像
    const bool pre_dirty = app.ordinary_records_dirty;
    std::vector<u8> enc_pre;
    CHECK(encode_pinned(app, enc_pre));
    app.progress.character = 99;  // 注入：越界席位（公开态变异负控）
    CHECK(!app.commit(kFps));     // commit_pending_play_frames 失败 → 整体失败
    CHECK_EQ(app.failure, std::string("Invalid play time record"));  // 原消息逐字
    // 固定序证据 1：录制槽先于播放帧——已冲账（槽清空、记录落流）。
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(0));
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
    std::vector<u8> enc_post;
    CHECK(encode_pinned(app, enc_post));
    CHECK(!bytes_equal(enc_post, enc_pre));
    // 固定序证据 2：播放帧失败 → 播放帧冲账中止（镜像/dirty 不变；镜像按真席
    // 位 3 读——注入位 99 只作失败触发，不越界读镜像）。
    CHECK_EQ(total_frames_at(app, 3), pre_mirror);
    CHECK_EQ(app.ordinary_records_dirty, pre_dirty);
    CHECK_EQ(app.pending_play_frames_, 0u);  // 计数清零先于冲账（原实现序）
    // 固定序证据 3：audio_device.update() 未执行——已入队请求未排空（真实
    // update 会 pending→0、started += e.started）。
    const Audio::Statistics post_stats = app.audio_device.statistics();
    CHECK_EQ(post_stats.pending_voices, pre_stats.pending_voices);
    CHECK_EQ(post_stats.started, pre_stats.started);
    CHECK_EQ(post_stats.requests, pre_stats.requests);
    CHECK_EQ(post_stats.dropped_requests, pre_stats.dropped_requests);
    CHECK_EQ(post_stats.mixed_frames, pre_stats.mixed_frames);
    // 固定序证据 4：frames++ 未执行；fps 窗未结算；caption retire 未执行
    // （prayer 保持非零、digest 与对照臂 f10 commit 后不同）。
    CHECK_EQ(app.frames, pre_frames);
    CHECK_EQ(app.slow_window_time_, pre_window_time);
    CHECK_EQ(app.slow_window_frames_, pre_window_frames);
    CHECK_EQ(app.loading_prayer != 0, true);
    CHECK_EQ(mpdigest::digest_loading_caption(app), pre_caption);
    // step 短路 present 不执行；下一帧 update 入口守卫拦停泵。
    DrawProbe probe;
    CHECK(probe.attach(app));
    input.update(0);
    CHECK(!app.step(input, kFps, false));
    CHECK_EQ(probe.fires, 0);           // present（含 Draw 拍）未执行
    CHECK_EQ(app.frames, pre_frames);   // 后续帧 commit 不再执行
    CHECK_EQ(app.failure, std::string("Invalid play time record"));  // fail 不被覆盖
    probe.detach();
}

// ---- 5) commit 固定序截断（第一环）：录制槽失败 → 播放帧不冲账 ----
// stream 退化（公开 API ReplayRecorder::clear）使冲账首环失败：播放帧/音频/
// frames++/fps 窗全部不执行、失败消息逐字、槽条目消费至失败点（首条弹出）。
TEST(commit_audio_boundary_commit_order_truncation_recorder_flush) {
    App app;
    CHECK(boot_live_run(app, "t3ab-flushfail-save.bin"));
    if (!app.failure.empty()) return;
    th13::GameInput input;
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    // 三个 update-only 对局帧：录制槽累计 3 条；startup 期 stop(30/55) 请求
    // 已入队（通道表非空——若 audio_device.update() 执行必排空，可辨）。
    for (int f = 0; f < 3; ++f) {
        input.update(0);
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
    }
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(3));
    CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
    const Audio::Statistics pre_stats = app.audio_device.statistics();
    CHECK_EQ(u32(pre_stats.pending_voices), 2u);  // 30/55 stop 位已入队
    const u32 pre_frames = app.frames;
    const i32 pre_mirror = total_frames_at(app, 0);
    const bool pre_dirty = app.ordinary_records_dirty;
    const double pre_window_time = app.slow_window_time_;
    app.recorder.stream.clear();  // 公开 API：流退化 → 冲账首环 stream.record 失败
    CHECK(!app.recorder.ready());
    CHECK(!app.commit(kFps));
    CHECK_EQ(app.failure, std::string("Replay input recording failed"));  // 原消息逐字
    // 首环失败：播放帧未冲账（镜像/dirty/frames/fps 窗不变——第二环未达）。
    CHECK_EQ(total_frames_at(app, 0), pre_mirror);
    CHECK_EQ(app.ordinary_records_dirty, pre_dirty);
    CHECK_EQ(app.frames, pre_frames);
    CHECK_EQ(app.slow_window_time_, pre_window_time);
    // audio_device.update() 未执行：已入队 stop 位未排空（真实 update 会
    // pending→0）。
    const Audio::Statistics post_stats = app.audio_device.statistics();
    CHECK_EQ(post_stats.pending_voices, pre_stats.pending_voices);
    CHECK_EQ(post_stats.started, pre_stats.started);
    // 槽条目消费至失败点：首条已弹出，其余保留（原序证据）。
    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(2));
    // 下一帧入口守卫拦停泵。
    input.update(0);
    CHECK(!app.update(input, false));
    CHECK(!app.step(input, kFps, false));
    CHECK_EQ(app.frames, pre_frames);
}

int main() { return th13test::run_all("m1.unit_commit_audio_boundary"); }
