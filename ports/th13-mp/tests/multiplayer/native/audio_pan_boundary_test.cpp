// m1.unit_audio_pan_boundary —— M1/T4 F3 pan 两入口拆分独立回归 + 天然队列
// 语义钉面（request_observer+statistics）。
//
// 被测生产面（全部 public，零生产钩子；实现=AudioDevice.cpp:339-362
// observe_voice_pan/apply_voice_pan/set_voice_pan 合成、ApplicationGame.cpp:258
// set_voice_pan 捕获接线、Application.cpp:1540 commit_pending_voice_pans
// apply-only 冲账、StageBattle.cpp:690-691 蓄声追踪窗真实调用链）。
//
// 断言命题（工单步骤 F3 + 风险反例逐条落点）：
//   1) 观察入口 observe_voice_pan：request_observer(Kind::Pan) 恰一次发射且
//      先于校验（无效 id 也发射）；校验条件与 error 文案逐字同原实现
//      （set-if-empty）；不写 buffer_pan、不触设备、不动 statistics。
//   2) apply-only 入口 apply_voice_pan：无观察器发射（Pan tap 恰一次落在
//      捕获拍——反例钉面：若 commit 拍再发射即观察时点漂移，tap 计数翻倍
//      即红）；buffer_pan=pan_entry(pan) 真实触达（mix 层 pan 律左/右声道
//      能量可辨）；无校验副作用（越界 id 静默 no-op）。
//   3) set_voice_pan = 两者合成：恰一次 tap + 真实应用；宿主捕获点
//      （ApplicationGame.cpp:258）同步校验失败返回 false（StageBattle
//      :690-694 消费面）+ fail(audio_device.error) + 不入挂起队。
//   4) 宿主 commit 冲账：pending_voice_pans_ 清槽、双 commit 幂等、pan 不触
//      statistics（requests 仅 queue_voice :361/:367 递增）。
//   5) live run（妖夢蓄声真实生产链）：逐帧 Pan tap 序列 (帧号, 30, 55) 成对
//      落捕获拍、commit 窗零 Pan tap、capture_frame=该帧逻辑帧身份。
//   6) 天然队列语义：enqueue/stop(id≥0)/stop(-1)/queue_music 的 tap 先于
//      校验、requests/pending_voices/started/stopped/dropped 按通道模型逐项
//      钉住；pan 全程不入 requests。
//
// mixer-only 诊断态（-DTH13_AUDIO_MIXER_ONLY）：immediate_output()=false →
// apply_voice_pan 的 web_sfx::pan immediate 分支不运行（属设备态/T8 浏览器
// 层）；mix 层断言消费 PCM 混音路径的 buffer_pan 律（左=1-pan、右=1+pan）。
#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "sdl/ApplicationState.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

using th13::u16;
using th13::u32;
using th13::u64;
using th13::i32;
using App = th13::sdl::ApplicationState;
using Audio = th13::sdl::AudioDevice;
using Observation = Audio::RequestObservation;

constexpr u16 kBootSeed = 0x1B2C;
constexpr float kFps = 60.0f;
constexpr u32 kZ = 0x01;
constexpr u32 kShift = 0x08;  // 低速/蓄声（StageBattle.cpp:690 追踪窗源）

const char* repo_root() {
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return root && *root ? root : "../../..";
}

const char* asset_root() {
    const char* root = std::getenv("TH13_NATIVE_ASSET_ROOT");
    return root && *root ? root : "";
}

bool init_effects(Audio& audio) {
    if (!*asset_root()) {
        std::printf("  失败 boot: TH13_NATIVE_ASSET_ROOT 未注入\n");
        return false;
    }
    return audio.initialize_effects_for_test(asset_root());
}

bool init_full(App& app, const char* save_name) {
    ::setenv("TH13_SAVE_PATH", save_name, 1);
    if (!*asset_root()) {
        std::printf("  失败 boot: TH13_NATIVE_ASSET_ROOT 未注入\n");
        return false;
    }
    if (!app.audio_device.initialize_for_test(
            asset_root(), std::string(repo_root()) + "/games/th13/thbgmogg.dat")) {
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
    if (!init_full(app, save_name)) return false;
    if (!app.start_game(1)) {
        std::printf("  失败 boot: start_game: %s\n", app.failure.c_str());
        return false;
    }
    return app.pending_destination == i32(th13::sdl::Destination::Run) && app.failure.empty();
}

// tap 日志（observer 只读；Pan/类目计数与逐事件对账）。
struct TapLog {
    std::vector<Observation> items;
    void attach(Audio& audio) {
        audio.request_observer = [this](const Observation& obs) { items.push_back(obs); };
    }
    size_t mark() const { return items.size(); }
    size_t count_kind(Audio::RequestObservation::Kind kind, size_t begin, size_t end) const {
        size_t n = 0;
        for (size_t i = begin; i < end && i < items.size(); ++i)
            if (items[i].kind == kind) ++n;
        return n;
    }
    size_t count_voice(size_t begin, size_t end) const {
        return count_kind(Observation::Kind::Voice, begin, end);
    }
    size_t count_pan(size_t begin, size_t end) const {
        return count_kind(Observation::Kind::Pan, begin, end);
    }
};

bool pcm_energy(Audio& audio, float& left, float& right) {
    float pcm[441 * 2] = {};
    if (!audio.mix(pcm, 441)) {
        std::printf("  失败 mix: %s\n", audio.error.c_str());
        return false;
    }
    left = right = 0;
    for (int i = 0; i < 441; ++i) {
        left += pcm[i * 2] < 0 ? -pcm[i * 2] : pcm[i * 2];
        right += pcm[i * 2 + 1] < 0 ? -pcm[i * 2 + 1] : pcm[i * 2 + 1];
    }
    return true;
}

// ---- 1) 设备级拆分：observe 观察+校验 / apply 触达 / 合成语义 ----
TEST(audio_pan_boundary_observe_apply_split_device_semantics) {
    // 观察入口：tap 先于校验、恰一次、不动 statistics、error set-if-empty 逐字。
    {
        Audio audio;
        CHECK(init_effects(audio));
        TapLog log;
        log.attach(audio);
        const Audio::Statistics pre = audio.statistics();
        CHECK(audio.observe_voice_pan(55, 192.f));
        CHECK_EQ(log.count_pan(0, log.mark()), size_t(1));
        CHECK_EQ(log.items[0].kind, Observation::Kind::Pan);
        CHECK_EQ(log.items[0].id, 55);
        CHECK(log.items[0].positional);
        CHECK(log.items[0].x == 192.f);
        const Audio::Statistics mid = audio.statistics();
        CHECK_EQ(mid.requests, pre.requests);        // pan 不入 requests
        CHECK_EQ(mid.started, pre.started);
        CHECK_EQ(u32(mid.pending_voices), 0u);       // 不写通道表
        CHECK_EQ(mid.mixed_frames, pre.mixed_frames);
        // 观察先于校验：无效 id 也发射，然后同步 false + error 逐字。
        CHECK(!audio.observe_voice_pan(99, -5.f));
        CHECK_EQ(log.count_pan(0, log.mark()), size_t(2));
        CHECK_EQ(log.items[1].id, 99);
        CHECK_EQ(audio.error, std::string("Invalid or uninitialized TH13 sound pan: 99"));
        // error 已非空：后续观察仍发射、set-if-empty 不覆盖、同步 false。
        CHECK(!audio.observe_voice_pan(30, 0.f));
        CHECK_EQ(log.count_pan(0, log.mark()), size_t(3));
        CHECK_EQ(audio.error, std::string("Invalid or uninitialized TH13 sound pan: 99"));
    }
    // 未初始化设备：同步 false + error 逐字（同 :337-343 校验条件）。
    {
        Audio audio;
        CHECK(!audio.observe_voice_pan(0, 0.f));
        CHECK_EQ(audio.error, std::string("Invalid or uninitialized TH13 sound pan: 0"));
    }
    // apply-only 入口：无观察器发射、mix 层 pan 律可辨、越界 id 静默 no-op。
    {
        Audio audio;
        CHECK(init_effects(audio));
        TapLog log;
        log.attach(audio);
        CHECK(audio.enqueue(55));  // 循环声（flags=1）持续在播
        audio.update();
        float left = 0, right = 0;
        CHECK(pcm_energy(audio, left, right));
        CHECK(left > 0 && right > 0);  // 居中（buffer_pan=0）
        const size_t taps_before = log.mark();
        audio.apply_voice_pan(55, 192.f);
        CHECK_EQ(log.mark(), taps_before);           // 无观察器发射
        CHECK(pcm_energy(audio, left, right));
        CHECK(left == 0.f && right > 0.f);           // pan=+1.0 → 左声道静音
        audio.apply_voice_pan(55, -192.f);
        CHECK_EQ(log.mark(), taps_before);
        CHECK(pcm_energy(audio, left, right));
        CHECK(right == 0.f && left > 0.f);           // pan=-1.0 → 右声道静音
        // 越界/负 id：静默 no-op（无 error、无 tap、无统计变化）。
        const Audio::Statistics pre = audio.statistics();
        audio.apply_voice_pan(99, 5.f);
        audio.apply_voice_pan(-1, 5.f);
        CHECK_EQ(log.mark(), taps_before);
        const Audio::Statistics post = audio.statistics();
        CHECK_EQ(post.requests, pre.requests);
        CHECK_EQ(post.started, pre.started);
        CHECK_EQ(u32(post.pending_voices), u32(pre.pending_voices));
        CHECK(audio.error.empty());
        // 合成入口 = observe + apply：恰一次 tap + 真实应用（回居中）。
        CHECK(audio.set_voice_pan(55, 0.f));
        CHECK_EQ(log.count_pan(0, log.mark()), size_t(1));
        CHECK(pcm_energy(audio, left, right));
        CHECK(left > 0 && right > 0);
    }
    // pan_entry 毫厘盘旋律（0x460c50 trunc(pan*1000/192)）经 positional
    // enqueue 的通道表结算 + voice_pan() 只读访问器钉住。
    {
        Audio audio;
        CHECK(init_effects(audio));
        CHECK(audio.enqueue(55, 1.0f));
        audio.update();
        CHECK_EQ(audio.voice_pan(55), 5);            // trunc(1000/192)
        CHECK(audio.enqueue(55, 192.f));
        audio.update();
        CHECK_EQ(audio.voice_pan(55), 1000);
        CHECK(audio.enqueue(55, -192.f));
        audio.update();
        CHECK_EQ(audio.voice_pan(55), -1000);
        CHECK(audio.enqueue(55, -1.0f));
        audio.update();
        CHECK_EQ(audio.voice_pan(55), -5);
    }
}

// ---- 2) 宿主捕获/冲账拆分：同步校验 + apply-only + 双 commit 幂等 ----
TEST(audio_pan_boundary_host_capture_sync_and_commit_apply) {
    // log 先于 app 声明：析构序 app 先、log 后——AudioDevice::request_observer
    // 持有的 tap 回调在其设备对象整个生命周期内保持有效（析构对合）。
    TapLog log;
    App app;
    CHECK(init_full(app, "t4pan-host.bin"));
    if (!app.failure.empty()) return;
    log.attach(app.audio_device);
    // 排空 boot 标题曲命令并停曲（mix 层只留被测 voice）。
    CHECK(app.audio_device.finish_music_requests());
    CHECK(app.audio_device.queue_music(3, 0, ""));
    CHECK(app.audio_device.finish_music_requests());
    // voice 55 居中在播（经天然队列 + commit 排空启动）。
    CHECK(app.audio_device.enqueue(55));
    const u64 requests_after_enqueue = app.audio_device.statistics().requests;
    CHECK_EQ(requests_after_enqueue, 1ull);  // boot 音频请求后恰 +1
    const u32 frames_boot = app.frames;
    CHECK(app.commit(kFps));
    CHECK_EQ(app.frames, frames_boot + 1u);
    float left = 0, right = 0;
    CHECK(pcm_energy(app.audio_device, left, right));
    CHECK(left > 0 && right > 0);
    // 捕获拍：观察恰一次、挂起队恰一条带帧身份、mix 仍居中（update-only）。
    // 帧身份 = recorder.capture_frame（update 段头同步；本块未经 update，
    // 帧号与挂起条目身份在此都以该值为准——真实对局内帧序断言在 live 块）。
    const u32 capture_frame = app.recorder.capture_frame;
    CHECK(app.set_voice_pan(55, 192.f));  // StageBattle.cpp:690-691 同面
    CHECK_EQ(log.count_pan(0, log.mark()), size_t(1));
    CHECK_EQ(app.pending_voice_pans_.size(), size_t(1));
    CHECK_EQ(app.pending_voice_pans_[0].capture_frame, capture_frame);
    CHECK_EQ(app.pending_voice_pans_[0].id, 55);
    CHECK(app.pending_voice_pans_[0].pan == 192.f);
    CHECK(pcm_energy(app.audio_device, left, right));
    CHECK(left > 0 && right > 0);  // update-only：buffer_pan 未触达
    // commit：apply-only——无第二次 Pan 发射、挂起队清槽、mix 层触达。
    CHECK(app.commit(kFps));
    CHECK_EQ(log.count_pan(0, log.mark()), size_t(1));  // 恰一次（反例钉面）
    CHECK(app.pending_voice_pans_.empty());
    CHECK(pcm_energy(app.audio_device, left, right));
    CHECK(left == 0.f && right > 0.f);
    // 双 commit：无重复冲账、无第二次发射。
    CHECK(app.commit(kFps));
    CHECK_EQ(log.count_pan(0, log.mark()), size_t(1));
    CHECK(app.pending_voice_pans_.empty());
    // pan 全程不入 statistics.requests（仅 enqueue(55) 的 1 次）。
    CHECK_EQ(app.audio_device.statistics().requests, requests_after_enqueue);
    // 宿主同步校验失败（放最后：fail 毒化本实例）：返回 false + failure
    // 逐字 + 不入挂起队；tap 仍恰一次发射（观察先于校验）。
    const size_t taps_before_bad = log.mark();
    CHECK(!app.set_voice_pan(99, 0.f));
    CHECK_EQ(app.failure, std::string("Invalid or uninitialized TH13 sound pan: 99"));
    CHECK_EQ(app.pending_voice_pans_.size(), size_t(0));
    CHECK_EQ(log.count_pan(taps_before_bad, log.mark()), size_t(1));
    CHECK_EQ(log.items[taps_before_bad].id, 99);
}

// ---- 3) live run 真实蓄声链：Pan tap (帧号, 30, 55) 全落捕获拍 ----
TEST(audio_pan_boundary_live_run_charge_pan_capture_not_commit) {
    // log 先于 app 声明：~ApplicationState → release_run → stop(-1) 仍会经
    // request_observer 发射，tap 回调必须存活到设备对象析构之后（析构对合）。
    TapLog log;
    App app;
    CHECK(boot_live_run(app, "t4pan-live.bin"));
    if (!app.failure.empty()) return;
    app.progress.character = 3;  // 妖夢：蓄声追踪窗生产请求源
    log.attach(app.audio_device);
    th13::GameInput input;
    input.update(0);  // 屏障帧
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    CHECK_EQ(app.pending_load, 1u);
    int charge_frames = 0;
    size_t pan_total = 0;
    size_t commit_pan_taps = 0;
    for (int f = 0; f < 40; ++f) {
        const u32 held = (f >= 10 ? (kShift | kZ) : kZ);  // f10 起蓄声窗
        input.update(held);
        const u32 frames_pre = app.frames;
        const size_t mark_pre = log.mark();
        CHECK(app.update(input, false));
        if (!app.failure.empty()) return;
        const size_t mark_mid = log.mark();
        const size_t pan_in_update = log.count_pan(mark_pre, mark_mid);
        if (pan_in_update > 0) {
            // 每蓄声帧恰一对 Pan(30,55)（StageBattle.cpp:690-691 相邻调用）。
            CHECK_EQ(pan_in_update, size_t(2));
            CHECK_EQ(log.items[mark_mid - 2].kind, Observation::Kind::Pan);
            CHECK_EQ(log.items[mark_mid - 2].id, 30);
            CHECK_EQ(log.items[mark_mid - 1].kind, Observation::Kind::Pan);
            CHECK_EQ(log.items[mark_mid - 1].id, 55);
            for (const auto& obs : log.items) {
                if (obs.kind == Observation::Kind::Pan)
                    CHECK(obs.x >= -192.f && obs.x <= 192.f);  // 场心域坐标
            }
            // 挂起队恰两条、帧身份=本帧逻辑帧（capture_frame=frames_pre）。
            CHECK_EQ(app.pending_voice_pans_.size(), size_t(2));
            CHECK_EQ(app.pending_voice_pans_[0].capture_frame, frames_pre);
            CHECK_EQ(app.pending_voice_pans_[1].capture_frame, frames_pre);
            ++charge_frames;
            pan_total += pan_in_update;
        } else {
            CHECK(app.pending_voice_pans_.empty());  // 无观察即无挂起条目
        }
        CHECK(app.commit(kFps));
        if (!app.failure.empty()) return;
        const size_t mark_post = log.mark();
        commit_pan_taps += log.count_pan(mark_mid, mark_post);  // apply-only=0
        CHECK(app.pending_voice_pans_.empty());  // 冲账清槽
        CHECK(app.present());
    }
    // 非空断言（防恒真）：蓄声窗真实发生、tap 全部落在捕获拍。
    CHECK(charge_frames >= 8);
    CHECK(pan_total >= 16);
    CHECK_EQ(commit_pan_taps, size_t(0));
    // requests == Voice tap 数（queue_voice 对每次请求恰 ++requests；Pan/
    // Stop/Music tap 不入 requests——pan 不触统计的全会话钉面）。
    const Audio::Statistics final_stats = app.audio_device.statistics();
    CHECK_EQ(final_stats.requests, u64(log.count_voice(0, log.mark())));
    CHECK(final_stats.requests > 0);  // 蓄声启动/生成声真实发生
}

// ---- 4) 天然队列语义钉面：enqueue/stop/queue_music tap+statistics 模型 ----
TEST(audio_pan_boundary_natural_queue_statistics_pinned) {
    Audio audio;
    CHECK(*asset_root() != 0);
    CHECK(audio.initialize_for_test(asset_root(),
                                    std::string(repo_root()) + "/games/th13/thbgmogg.dat"));
    TapLog log;
    log.attach(audio);
    // enqueue(id)：Voice tap 先于排队；requests/pending 增、started 不变。
    CHECK(audio.enqueue(30));
    CHECK_EQ(log.count_voice(0, log.mark()), size_t(1));
    CHECK_EQ(audio.statistics().requests, 1ull);
    CHECK_EQ(u32(audio.statistics().pending_voices), 1u);
    CHECK_EQ(audio.statistics().started, 0ull);
    audio.update();
    CHECK_EQ(u32(audio.statistics().pending_voices), 0u);  // 通道表排空
    CHECK_EQ(audio.statistics().started, 1ull);
    // 同窗重复请求合并：requests 逐次计、启动恰一次。
    CHECK(audio.enqueue(30));
    CHECK(audio.enqueue(30));
    CHECK_EQ(audio.statistics().requests, 3ull);
    CHECK_EQ(u32(audio.statistics().pending_voices), 1u);
    audio.update();
    CHECK_EQ(audio.statistics().started, 2ull);
    // stop(id≥0)：Stop tap + 停止 marker；排空时在播声 → stopped++。
    CHECK(audio.stop(30));
    CHECK_EQ(log.count_kind(Observation::Kind::Stop, 0, log.mark()), size_t(1));
    CHECK_EQ(u32(audio.statistics().pending_voices), 1u);  // marker 占通道
    audio.update();
    CHECK_EQ(audio.statistics().stopped, 1ull);  // 在播声被停
    CHECK_EQ(u32(audio.statistics().pending_voices), 0u);
    // 非在播声的 stop marker：排空不再 ++stopped。
    CHECK(audio.stop(30));
    audio.update();
    CHECK_EQ(audio.statistics().stopped, 1ull);
    // stop(-1)：tap 后当场 ++stopped（0x460d40 全停即时语义）。
    CHECK(audio.enqueue(21));
    audio.update();
    CHECK_EQ(audio.statistics().started, 3ull);
    CHECK(audio.stop(-1));
    CHECK_EQ(audio.statistics().stopped, 2ull);  // 即时（未 update）
    CHECK_EQ(log.items.back().kind, Observation::Kind::Stop);
    CHECK_EQ(log.items.back().id, -1);
    // 12 通道帧表满载丢弃：12 个不同 id 占满，第 13 个 dropped。
    for (i32 id = 0; id < 12; ++id) CHECK(audio.enqueue(id));
    CHECK_EQ(u32(audio.statistics().pending_voices), 12u);
    const u64 dropped_pre = audio.statistics().dropped_requests;
    const u64 requests_pre = audio.statistics().requests;
    CHECK(audio.enqueue(12));  // tap 已发射、请求已计、通道满 → 丢
    CHECK_EQ(audio.statistics().requests, requests_pre + 1ull);
    CHECK_EQ(audio.statistics().dropped_requests, dropped_pre + 1ull);
    CHECK_EQ(u32(audio.statistics().pending_voices), 12u);
    audio.update();
    CHECK_EQ(u32(audio.statistics().pending_voices), 0u);
    CHECK_EQ(audio.statistics().started, 15ull);  // +12（12 个新 id 启动）
    // stop marker 赢过同帧 start：start 被丢、marker 生效。
    CHECK(audio.enqueue(40));
    audio.update();
    CHECK_EQ(audio.statistics().started, 16ull);  // voice 40 启动
    const u64 dropped_before_marker = audio.statistics().dropped_requests;
    CHECK(audio.stop(40));
    CHECK(audio.enqueue(40));  // 同帧 start → 丢（count<0 拒绝）
    CHECK_EQ(audio.statistics().dropped_requests, dropped_before_marker + 1ull);
    audio.update();
    CHECK_EQ(audio.statistics().started, 16ull);   // 未新增（marker 赢）
    CHECK_EQ(audio.statistics().stopped, 3ull);    // 在播的 40 被停（1+stop(-1)+此）
    // queue_music：Music tap + 命令队列排空到真实曲目位。
    CHECK(audio.queue_music(1, 0, "th13_00.wav"));
    CHECK(audio.queue_music(2, -1, "th13_00.wav"));
    CHECK_EQ(log.count_kind(Observation::Kind::Music, 0, log.mark()), size_t(2));
    CHECK_EQ(log.items[log.items.size() - 2].text, std::string("th13_00.wav"));
    CHECK(audio.finish_music_requests());
    CHECK(audio.statistics().track >= 0);  // th13_00 已选曲
    CHECK(audio.queue_music(3, 0, ""));
    CHECK(audio.finish_music_requests());
    CHECK_EQ(audio.statistics().track, -1);  // 停曲
    // 全程无设备错误（错误会让后续请求全部短路，破坏上述模型）。
    CHECK(audio.error.empty());
    // 观察先于校验（stop 无效 id）：tap 已发射、错误如实（独立设备免毒化）。
    {
        Audio bad;
        CHECK(bad.initialize_for_test(asset_root(),
                                      std::string(repo_root()) + "/games/th13/thbgmogg.dat"));
        TapLog bad_log;
        bad_log.attach(bad);
        CHECK(!bad.stop(99));
        CHECK_EQ(bad_log.count_kind(Observation::Kind::Stop, 0, bad_log.mark()), size_t(1));
        CHECK_EQ(bad.error, std::string("Invalid TH13 sound stop request: 99"));
    }
}

}  // namespace

int main() { return th13test::run_all("m1.unit_audio_pan_boundary"); }
