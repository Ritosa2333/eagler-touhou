// m1.side_effect_boundary —— M1/T7 固定 case：T2 探针 (a)(b)(c)(d) 收全 +
// T3 冲账/录制 + T4 队列收全（workflow T7「Title/Run/Ending/NextStage/
// AfterEnding 五条转换路径『先画后拆/只置 flag』+ 装载幕末次 present +
// 冲账时机 + 录制恰好一次」）。
//
// 被测生产面（全部 public/struct 公开态，零生产钩子）：
//   - present 段（Application.cpp:1618-1632）：pause->draw_overlay →
//     display->draw() → destination 尾。五条路径的消费点：
//       Run(13)/Ending(15) ⇒ draw_loading(false)+pending_load=1（「只置
//       幕」，次帧头 complete_loading→begin_run）；Title(4/2/14)/Ending/
//       AfterEnding(16) ⇒ present 尾 apply_destination 同帧拆建（「先画后
//       拆」）；NextStage(100) ⇒ 只置 stage_transition_loading 不拆场景。
//   - AfterEnding native 合成（Application.cpp:1217-1232）：活动 run 上直接
//       置 pending_destination=AfterEnding(16)；ending==nullptr 前置断言
//       （非空即停用例换证据路径）；apply_destination 跳过 if(ending) 拆卸
//       块，直接 save_score+begin_title(3)（reason 3 专属 AfterEnding）。
//   - ending clear 迁移（T2 解耦(b)）：select_target/clear_target 已从
//       update_scene_pass 移至 present 头——submission_trace 断言 update 段
//       零 screen Clear 项、present 段先 Clear 后场景 Draw。
//   - T3：pending_play_frames_ 镜像（commit 头冲账）+ PendingReplayRecorder
//       挂起槽（record_input 输入沿/update_spell_time 钟值，帧身份幂等、
//       同帧双 commit 不双记）。
//   - T4：PendingFileEvent 八捕获点三档（帧失败/吞错/菜单错误——本件钉
//       捕获序冲账与 update-only 不变性，档位失败语义由 unit 件逐点保真）
//       + PendingVoicePan（捕获拍 tap 恰一次、commit apply-only）+
//       save_replay_slot 确认拍原位（不进待确认队列）。
#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/ReplayFile.hpp"
#include "game/SpellState.hpp"
#include "game/TitleOptionsMenu.hpp"
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
using th13::sdl::PendingFileEvent;
using EventKind = PendingFileEvent::Kind;
using th13::GameInput;
using th13::SpellEvent;
using th13::SpellEventKind;
using th13::kSpellCapture;

constexpr u16 kBootSeed = 0x1B2C;
constexpr float kFps = 60.0f;
constexpr u32 kZ = 0x01;
constexpr u32 kShift = 0x08;

const char* repo_root() {
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    return root && *root ? root : "../../..";
}

// 测试产物（存档/ending trace）落到 runner 的 case 运行目录：
// TH13_NATIVE_RUN_DIR 由 tests/multiplayer/lib/native-runner.mjs 注入
// （= <outer>/artifacts/mp-native-run/<case>/）。未注入时保持旧行为
// （相对 cwd），供直接运行二进制时使用。
std::string run_output_path(const char* name) {
    const char* dir = std::getenv("TH13_NATIVE_RUN_DIR");
    if (!dir || !*dir) return name;
    return std::string(dir) + "/" + name;
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

bool boot_live_run(App& app, const char* save_name) {
    if (!pin_boot(app, save_name)) return false;
    if (!app.start_game(1)) {
        std::printf("  失败 boot: start_game: %s\n", app.failure.c_str());
        return false;
    }
    return app.pending_destination == i32(Destination::Run) && app.failure.empty();
}

bool drive(App& app, GameInput& input, u32 held) {
    input.update(held);
    if (!app.update(input, false)) return false;
    if (!app.commit(kFps)) return false;
    return app.present();
}

bool encode_pinned(const App& app, std::vector<u8>& out) {
    th13::ReplaySaveMeta meta;
    meta.date = 1;
    meta.slow_numerator = 0.0;
    meta.slow_denominator = 1099511627776.0;
    return app.recorder.encode(out, "t7se", 0, meta);
}

bool bytes_equal(const std::vector<u8>& a, const std::vector<u8>& b) {
    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
}

SpellEvent begin_event(i32 slot, const char* name) {
    SpellEvent event;
    event.kind = SpellEventKind::Begin;
    event.state.id = slot;
    event.state.name = name;
    return event;
}

i32 total_frames(const App& app) {
    return app.files.ordinary_records[size_t(app.progress.character)].total_frames;
}

// ---- Draw 段探针：闩锁第一次 Draw 拍，按模式断言帧内状态面（先画后拆）。
struct DrawProbe {
    enum class Mode { RunBaffle, BaffleLast, NextStage, AfterEnding, Title, Ending };
    th13::FrameCallback callback{};
    App* app = nullptr;
    th13::FrameScheduler* attached_scheduler = nullptr;  // 登记归属（FrameCallback::scheduler private）
    Mode mode = Mode::RunBaffle;
    int fires = 0;
    bool observed_ok = false;
    static i32 run(void* p) {
        auto& probe = *static_cast<DrawProbe*>(p);
        App& app = *probe.app;
        ++probe.fires;
        switch (probe.mode) {
        case Mode::RunBaffle:
            // (b) Run destination 帧：Draw 拍内 pending_load==0、title 存活。
            probe.observed_ok = app.pending_load == 0 && app.title != nullptr && app.run == nullptr;
            break;
        case Mode::BaffleLast:
            // 装载幕末次 present 帧：update 头已 begin_run，装载 caption 仍存活。
            probe.observed_ok = app.run != nullptr && app.pending_load == 0 &&
                                app.loading_prayer != 0 && app.progress.stage_frame == 1;
            break;
        case Mode::NextStage:
            // (c) NextStage 帧：Draw 拍内 run 存活且 flag 仍 false（只置 flag）。
            probe.observed_ok = app.run != nullptr && !app.stage_transition_loading;
            break;
        case Mode::AfterEnding:
            // AfterEnding 合成帧：Draw 拍内活动 run 存活（先画）。
            probe.observed_ok = app.run != nullptr &&
                                app.pending_destination == i32(Destination::AfterEnding);
            break;
        case Mode::Title:
            // (a) Title destination 帧：Draw 拍内 run 存活且 pause active（先画）。
            probe.observed_ok = app.run != nullptr && app.pause != nullptr && app.pause->active();
            break;
        case Mode::Ending:
            // Ending 帧：Draw 拍内 run 存活（先画后拆）。
            probe.observed_ok = app.run != nullptr &&
                                app.pending_destination == i32(Destination::Ending);
            break;
        }
        probe.callback.enabled = false;  // 闩锁
        return i32(th13::FrameAction::Continue);
    }
    bool attach(App& target, Mode m) {
        app = &target;
        mode = m;
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

// 行级 kind 统计（trace 行内 "kind":"clear"/"draw" 计数与首个位置）。
struct KindSpan {
    int clear = 0, draw = 0;
    int first_clear = -1, first_draw = -1;
};

KindSpan scan_span(const std::string& text, size_t begin, size_t end) {
    KindSpan span;
    size_t pos = begin;
    int index = 0;
    while (pos < end) {
        const size_t line_end = text.find('\n', pos);
        const size_t stop = line_end == std::string::npos || line_end > end ? end : line_end;
        const std::string line = text.substr(pos, stop - pos);
        const bool is_clear = line.find("\"kind\":\"clear\"") != std::string::npos;
        const bool is_draw = line.find("\"kind\":\"draw\"") != std::string::npos;
        if (is_clear) {
            if (span.first_clear < 0) span.first_clear = index;
            ++span.clear;
        }
        if (is_draw) {
            if (span.first_draw < 0) span.first_draw = index;
            ++span.draw;
        }
        ++index;
        if (line_end == std::string::npos || line_end >= end) break;
        pos = line_end + 1;
    }
    return span;
}

}  // namespace

// ---- 1) 五条转换路径探针（(b)→末次 present→(c)→AfterEnding 合成→(a)→Ending）----
TEST(side_effect_transition_probes_five_paths) {
    App app;
    CHECK(boot_live_run(app, "t7se-paths-save.bin"));
    if (app.failure.empty()) {
        GameInput input;

        // ---- (b) Run destination：先画（Draw 拍 pending_load==0）后建幕 ----
        CHECK_EQ(app.pending_destination, i32(Destination::Run));
        {
            DrawProbe probe;
            CHECK(probe.attach(app, DrawProbe::Mode::RunBaffle));
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
            CHECK_EQ(probe.fires, 1);
            CHECK(probe.observed_ok);
            CHECK_EQ(app.pending_load, 1u);        // present 尾置位
            CHECK_EQ(app.loading_frames, 0u);
            CHECK(app.loading_prayer != 0);
            CHECK_EQ(app.pending_destination, i32(Destination::Run));  // Run 分支不消费
            probe.detach();
        }
        // ---- 装载幕末次 present：update 头 complete_loading→begin_run ----
        {
            DrawProbe probe;
            CHECK(probe.attach(app, DrawProbe::Mode::BaffleLast));
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
            CHECK_EQ(probe.fires, 1);
            CHECK(probe.observed_ok);
            CHECK_EQ(app.pending_load, 0u);        // 已消费
            CHECK(app.run != nullptr);
            CHECK_EQ(app.progress.stage_frame, 1);  // battle f0
            probe.detach();
        }
        // 稳态对局 ×2。
        for (int F = 1; F <= 2; ++F) {
            CHECK(drive(app, input, kZ));
            CHECK_EQ(app.progress.stage_frame, F + 1);
        }
        // ---- (c) NextStage：只置 flag 不拆场景；Draw 拍内 flag 仍 false ----
        {
            DrawProbe probe;
            app.pending_destination = i32(Destination::NextStage);
            CHECK(probe.attach(app, DrawProbe::Mode::NextStage));
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
            CHECK_EQ(probe.fires, 1);
            CHECK(probe.observed_ok);
            CHECK(app.stage_transition_loading);          // present 尾置位
            CHECK(app.run != nullptr);                    // 场景不拆
            CHECK_EQ(app.pending_destination, -1);
            CHECK_EQ(app.current_destination, i32(Destination::Run));
            probe.detach();
            app.stage_transition_loading = false;  // 单帧后复位防污染（同 unit 件）
        }
        CHECK(drive(app, input, 0));
        CHECK(!app.stage_transition_loading);

        // ---- AfterEnding 合成：活动 run 上置 16，ending==nullptr 前置断言 ----
        {
            // 前置断言（非空即停用例换证据路径——工单显式条款）。
            CHECK(app.ending == nullptr);
            if (app.ending != nullptr) return;
            DrawProbe probe;
            app.pending_destination = i32(Destination::AfterEnding);
            CHECK(probe.attach(app, DrawProbe::Mode::AfterEnding));
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
            CHECK_EQ(probe.fires, 1);
            CHECK(probe.observed_ok);              // Draw 拍内 run 存活（先画）
            CHECK(app.run == nullptr);             // 后拆（16∉{Run,Ending} 走 apply_destination）
            CHECK(app.ending == nullptr);          // if(ending) 拆卸块被跳过
            CHECK(app.title != nullptr);
            CHECK_EQ(app.title->state.return_reason, 3);  // reason 3 专属 AfterEnding
            CHECK_EQ(app.current_destination, i32(Destination::Title));
            CHECK_EQ(app.pending_destination, -1);
            CHECK_EQ(app.pending_load, 0u);        // 不走装载幕分支
            probe.detach();
        }
        // 战后标题可继续开局（begin_title(3) 链健康）。Run destination 两拍：
        // 第一拍 present 尾建装载幕，第二拍 update 头 begin_run（battle f0）。
        CHECK(app.start_game(1));
        CHECK(drive(app, input, 0));  // 屏障帧（建幕）
        CHECK(drive(app, input, 0));  // complete_loading→begin_run
        CHECK(app.run != nullptr);
        for (int F = 0; F < 2; ++F) CHECK(drive(app, input, kZ));

        // ---- (a) Title destination：先画（run 存活 + pause active）后拆 ----
        {
            CHECK(app.pause != nullptr);
            CHECK(app.pause->open(th13::PauseEntrance::Pause));
            CHECK(app.pause->active());
            CHECK(app.return_to_title());  // RunPause 平台回调（生产链）
            CHECK_EQ(app.pending_destination, i32(Destination::Title));
            DrawProbe probe;
            CHECK(probe.attach(app, DrawProbe::Mode::Title));
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
            CHECK_EQ(probe.fires, 1);
            CHECK(probe.observed_ok);
            CHECK(app.run == nullptr);
            CHECK(app.pause == nullptr);
            CHECK(app.title != nullptr);
            CHECK_EQ(app.pending_destination, -1);
            CHECK_EQ(app.current_destination, i32(Destination::Title));
            probe.detach();
        }
        // ---- Ending 经装载幕两拍消费（0xf 与 Run 同走 present 尾建幕分支，
        // 拆建在次帧头 complete_loading→apply_destination：先画后拆）----
        {
            CHECK(app.start_game(1));
            CHECK(drive(app, input, 0));  // 屏障帧（建幕）
            CHECK(drive(app, input, 0));  // complete_loading→begin_run → battle f0
            CHECK(app.run != nullptr);
            CHECK(drive(app, input, kZ));
            // th13_debug_ending 同款字段（BrowserRuntime.cpp:581-597 语义），
            // 但 destination 走 present 尾消费（真实 0x42c550 帧尾拍）。
            app.progress.character = 0;
            app.progress.difficulty = 1;
            app.progress.continues = 0;
            app.progress.practice_mode = 0;
            app.progress.stage = 6;
            DrawProbe probe;
            app.pending_destination = i32(Destination::Ending);
            CHECK(probe.attach(app, DrawProbe::Mode::Ending));
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
            CHECK_EQ(probe.fires, 1);
            CHECK(probe.observed_ok);              // Draw 拍内 run 存活（先画）
            CHECK(app.run != nullptr);             // 幕分支不拆场景（同 Run）
            CHECK_EQ(app.pending_load, 1u);        // 装载幕建立（Ending∈幕分支）
            CHECK_EQ(app.pending_destination, i32(Destination::Ending));
            probe.detach();
            // 次帧头 complete_loading→apply_destination→Ending 分支：拆 run、建结局。
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
            CHECK(app.run == nullptr);             // 后拆（release_run）
            CHECK(app.ending != nullptr);          // Ending 场景已建
            CHECK_EQ(app.current_destination, i32(Destination::Ending));
            CHECK_EQ(app.pending_destination, -1);
            CHECK_EQ(app.pending_load, 0u);
            // ending 稳态帧可跑。
            CHECK(drive(app, input, 0));
            CHECK(app.failure.empty());
        }
    }
}

// ---- 2) (d) ending clear 迁移：update 段零 Clear、present 段先 Clear 后 Draw ----
TEST(side_effect_ending_clear_migration_trace) {
    App app;
    CHECK(boot_live_run(app, "t7se-trace-save.bin"));
    if (app.failure.empty()) {
        GameInput input;
        CHECK(drive(app, input, 0));  // 屏障帧
        CHECK(drive(app, input, kZ));
        app.progress.character = 0;
        app.progress.difficulty = 1;
        app.progress.continues = 0;
        app.progress.practice_mode = 0;
        app.progress.stage = 6;
        app.title.reset();
        app.pending_destination = i32(Destination::Ending);
        CHECK(app.apply_destination());
        CHECK(app.failure.empty());
        CHECK(app.ending != nullptr);
        CHECK(app.run == nullptr);
        CHECK_EQ(app.pending_load, 0u);  // 稳态 ending 帧（无屏障帧混入）

        const std::string trace_path = run_output_path("t7se-ending-trace.jsonl");
        CHECK(app.graphics.submission_trace.open(trace_path.c_str()));
        if (!app.graphics.submission_trace.active()) return;
        constexpr int kTracedFrames = 60;
        std::vector<long> update_ends;
        bool drive_failed = false;
        for (int frame = 0; frame < kTracedFrames && !drive_failed; ++frame) {
            input.update(0);
            if (!app.update(input, false) || !app.failure.empty()) {
                std::printf("  失败 ending update f%d: %s\n", frame, app.failure.c_str());
                drive_failed = true;
                break;
            }
            const long mark = app.graphics.submission_trace.output_position();
            CHECK(mark >= 0);
            if (mark < 0) { drive_failed = true; break; }
            update_ends.push_back(mark);
            if (!app.commit(kFps) || !app.present() || !app.failure.empty()) {
                std::printf("  失败 ending present f%d: %s\n", frame, app.failure.c_str());
                drive_failed = true;
            }
        }
        CHECK(app.graphics.submission_trace.close());
        CHECK(app.graphics.submission_trace.good());
        if (drive_failed || update_ends.size() != size_t(kTracedFrames)) return;

        std::FILE* trace = std::fopen(trace_path.c_str(), "rb");
        CHECK(trace != nullptr);
        if (!trace) return;
        std::string text;
        char buffer[4096];
        size_t got = 0;
        while ((got = std::fread(buffer, 1, sizeof buffer, trace)) > 0)
            text.append(buffer, got);
        std::fclose(trace);

        size_t boundary = 0;
        int total_present_draws = 0;
        int unordered_present_spans = 0;
        int update_clear_total = 0, present_clear_total = 0, draw_frames = 0;
        for (int frame = 0; frame < kTracedFrames; ++frame) {
            const size_t update_end = size_t(update_ends[size_t(frame)]);
            CHECK(update_end <= text.size());
            const KindSpan update_span = scan_span(text, boundary, update_end);
            const size_t next_boundary =
                frame + 1 < kTracedFrames ? size_t(update_ends[size_t(frame) + 1]) : text.size();
            const KindSpan present_span = scan_span(text, update_end, next_boundary);
            boundary = next_boundary;
            update_clear_total += update_span.clear;
            present_clear_total += present_span.clear;
            total_present_draws += present_span.draw;
            if (present_span.draw > 0) ++draw_frames;
            // update 段零 screen Clear（select_target/clear_target 已迁出模拟段）。
            if (update_span.clear != 0)
                std::printf("  失败 trace f%d: update 段含 %d 个 Clear 项（clear 未迁出模拟段）\n",
                            frame, update_span.clear);
            CHECK_EQ(update_span.clear, 0);
            CHECK(present_span.clear >= 1);
            if (present_span.draw > 0) {
                CHECK(present_span.first_clear >= 0);
                CHECK(present_span.first_draw >= 0);
                if (present_span.first_clear >= present_span.first_draw) {
                    ++unordered_present_spans;
                    std::printf("  失败 trace f%d: present 段首 Clear(%d) 不先于首 Draw(%d)\n",
                                frame, present_span.first_clear, present_span.first_draw);
                }
            }
        }
        std::printf("# trace: %d 帧 update 段 Clear 合计=%d，present 段 Clear 合计=%d、"
                    "Draw 合计=%d（含图元帧=%d，乱序帧=%d）\n",
                    kTracedFrames, update_clear_total, present_clear_total,
                    total_present_draws, draw_frames, unordered_present_spans);
        CHECK_EQ(update_clear_total, 0);
        CHECK(present_clear_total >= kTracedFrames);
        CHECK(total_present_draws >= 1);
        CHECK_EQ(unordered_present_spans, 0);
    }
}

// ---- 3) T3 冲账/录制：镜像只在 commit、录制恰一次、双 commit 幂等、钟值随槽 ----
TEST(side_effect_reconcile_record_once) {
    constexpr int kFrames = 60;
    App app;
    CHECK(boot_live_run(app, "t7se-rec-save.bin"));
    if (app.failure.empty()) {
        GameInput input;
        // 屏障帧（Shift 预持；present 尾置 pending_load）。
        input.update(kShift);
        CHECK(drive(app, input, 0));
        CHECK_EQ(app.pending_load, 1u);
        CHECK_EQ(total_frames(app), 0);
        CHECK_EQ(app.ordinary_records_dirty, false);
        CHECK_EQ(app.pending_play_frames_, 0u);

        auto pattern = [](u32 f) {
            u32 held = 0;
            if (f % 12 < 7) held |= kZ;
            if (f % 19 >= 11 && f % 19 < 15) held |= 0x80u;  // 右移
            return held;
        };
        int advanced_total = 0;
        for (int f = 0; f < kFrames; ++f) {
            input.update(f == 0 ? (kShift | kZ) : pattern(u32(f)));
            const i32 stage_pre = app.progress.stage_frame;
            const u32 frames_pre = app.frames;
            const i32 mirror_pre = total_frames(app);
            const bool dirty_pre = app.ordinary_records_dirty;
            const u32 pending_pre = app.pending_play_frames_;
            CHECK(app.update(input, false));
            CHECK(app.failure.empty());
            const bool advanced = app.progress.stage_frame != stage_pre;
            // update-only 段：镜像/dirty/frames 不变；槽随推进帧恰 +1。
            CHECK_EQ(total_frames(app), mirror_pre);
            CHECK_EQ(app.ordinary_records_dirty, dirty_pre);
            CHECK_EQ(app.frames, frames_pre);
            CHECK_EQ(app.pending_play_frames_, pending_pre + (advanced ? 1u : 0u));
            if (advanced) {
                ++advanced_total;
                CHECK(!app.recorder.pending_inputs.empty());
                if (app.recorder.pending_inputs.empty()) return;
                const auto& slot = app.recorder.pending_inputs.back();
                CHECK_EQ(slot.frame, frames_pre);   // 帧身份 = update 段头 frames
                CHECK_EQ(slot.slow, u8(60));        // 恒 60fps 窗
                CHECK_EQ(slot.edge_reset, f == 0);  // D-fix 仅关首帧（begin_run 拍置位）
            }
            int commits = 1;
            if (f >= 20 && f <= 22) commits = 0;    // update-only 三帧窗
            if (f == 40) commits = 2;               // 同帧双 commit 反例位
            for (int c = 0; c < commits; ++c) {
                CHECK(app.commit(kFps));
                CHECK(app.failure.empty());
                if (c == 0) {
                    CHECK_EQ(app.recorder.pending_inputs.size(), size_t(0));
                    CHECK_EQ(app.pending_play_frames_, 0u);   // 冲账即清
                    CHECK_EQ(total_frames(app), advanced_total);  // 恰一次
                    CHECK_EQ(app.ordinary_records_dirty, true);
                    CHECK_EQ(app.frames, frames_pre + 1u);
                } else {
                    CHECK_EQ(total_frames(app), advanced_total);  // 双 commit 不双记
                    CHECK_EQ(app.pending_play_frames_, 0u);
                    CHECK_EQ(app.frames, frames_pre + 2u);
                }
            }
            if (commits == 0) CHECK_EQ(app.pending_play_frames_, u32(f - 19));  // 窗内累计
            CHECK(app.present());
            CHECK(app.failure.empty());
        }
        CHECK_EQ(advanced_total, kFrames);
        CHECK_EQ(total_frames(app), kFrames);
        // 录制恰一次：decode 帧数 == 推进帧数 == 逐帧输入沿对账。
        std::vector<u8> enc;
        CHECK(encode_pinned(app, enc));
        th13::ReplayFile decoded;
        CHECK(decoded.decode(enc.data(), enc.size()));
        CHECK_EQ(decoded.stages.size(), size_t(1));
        if (decoded.stages.empty()) return;
        const th13::ReplayStage& stage = decoded.stages.front();
        CHECK_EQ(stage.frame_count, u32(kFrames));
        for (u32 i = 0; i < stage.frame_count; ++i) {
            const th13::ReplayFrame frame = stage.frame(i);
            const u32 f = i == 0 ? (kShift | kZ) : pattern(i);
            // 关首 D-fix 帧（i==0）pressed=全量 held；此后 pressed=新增位。
            // 逐帧 state 必须等于合成 held（输入沿恰一次、无重复/丢帧）。
            CHECK_EQ(u32(frame.state), f);
            if (u32(frame.state) != f) break;
        }
        // update_spell_time 槽：Begin→End 成对捕获、commit 冲账、双 commit 幂等。
        CHECK(app.recorder.ready());
        CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));
        app.update_spell_time(true);
        app.update_spell_time(false);
        CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(1));
        if (app.recorder.pending_spell_times.size() == 1) {
            const auto& entry = app.recorder.pending_spell_times.front();
            CHECK_EQ(entry.index, 0u);
            CHECK_EQ(entry.packed, app.spell_time_packed);  // 钟值捕获一次随槽
            std::vector<u8> before;
            CHECK(encode_pinned(app, before));
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            CHECK(app.commit(kFps));
            CHECK_EQ(app.recorder.pending_spell_times.size(), size_t(0));  // 冲账清槽
            CHECK_EQ(app.spell_time_packed, entry.packed);  // commit 不重算钟值
            std::vector<u8> after;
            CHECK(encode_pinned(app, after));
            CHECK(!bytes_equal(after, before));
            th13::ReplayFile decoded2;
            CHECK(decoded2.decode(after.data(), after.size()));
            CHECK_EQ(decoded2.stages.size(), size_t(1));
            if (!decoded2.stages.empty()) {
                const th13::ReplayStage& st = decoded2.stages.front();
                CHECK(st.header.size() >= size_t(0xC4));
                const u32 stored = u32(st.header[0x70]) | u32(st.header[0x71]) << 8 |
                                   u32(st.header[0x72]) << 16 | u32(st.header[0x73]) << 24;
                CHECK_EQ(i32(stored), entry.packed);  // 落流 = 捕获拍钟值
            }
        }
    }
}

// ---- 4) T4 队列收全：八捕获点入队序冲账 + pan 挂起队 + 确认拍原位 ----
TEST(side_effect_file_event_queue_full) {
    App app;
    CHECK(pin_boot(app, "t7se-queue-save.bin"));
    if (app.failure.empty()) {
        GameInput input;
        app.progress.character = 0;
        app.progress.selected_spell_id = 0;
        // 捕获序（顺序敏感对：同字段 row0 音量两笔，冲账序可辨）。
        CHECK(app.record_spell_event(begin_event(2, "T7queue")));       // 1 SpellEvent
        CHECK(app.record_spell_practice_score(12345));                  // 2
        CHECK(app.change_pause_option(0, +10));                         // 3（序敏感对 a）
        CHECK(app.change_pause_option(0, -30));                         // 4（序敏感对 b）
        CHECK(app.record_music_unlocked(3));                            // 5
        app.mark_heard(5);                                              // 6（吞错档正控）
        CHECK(app.persist_options(th13::TitleOptionVolumes{40, 60}));   // 7（菜单错误档）
        CHECK(app.record_result_entry("T7QUEUE", 8));                  // 8

        const u32 capture_frame = app.recorder.capture_frame;
        CHECK_EQ(app.pending_file_events_.size(), size_t(8));
        {
            const EventKind expected[] = {
                EventKind::SpellEvent, EventKind::SpellPracticeScore,
                EventKind::PauseOption, EventKind::PauseOption,
                EventKind::MusicUnlocked, EventKind::MarkHeard,
                EventKind::PersistOptions, EventKind::ResultEntry};
            for (size_t i = 0; i < 8 && i < app.pending_file_events_.size(); ++i) {
                CHECK(app.pending_file_events_[i].kind == expected[i]);  // 捕获序
                CHECK_EQ(app.pending_file_events_[i].capture_frame, capture_frame);
            }
        }
        // update-only 段：files 字段/设备音量/存储镜像全部不变。
        const i32 attempts_before =
            app.files.spell_counters(0, 2) ? app.files.spell_counters(0, 2)->attempts[0] : -1;
        const i32 best_before =
            app.files.spell_counters(0, 0) ? app.files.spell_counters(0, 0)->practice_best_score : -1;
        const i32 bgm_before = app.files.bgm_volume;
        const i32 eff_before = app.files.effect_volume;
        const i32 dev_bgm_before = app.audio_device.music_volume;
        const i32 dev_eff_before = app.audio_device.effect_volume;
        const bool heard3_before = app.files.music_heard(3);
        const bool heard5_before = app.files.music_heard(5);
        CHECK(attempts_before >= 0 && best_before >= 0);
        CHECK_EQ(app.files.spell_counters(0, 2)->attempts[0], attempts_before);
        CHECK_EQ(app.files.spell_counters(0, 0)->practice_best_score, best_before);
        CHECK_EQ(app.files.bgm_volume, bgm_before);
        CHECK_EQ(app.files.effect_volume, eff_before);
        CHECK_EQ(app.audio_device.music_volume, dev_bgm_before);
        CHECK_EQ(app.audio_device.effect_volume, dev_eff_before);
        CHECK_EQ(app.files.music_heard(3), heard3_before);
        CHECK_EQ(app.files.music_heard(5), heard5_before);
        CHECK(app.title->option_menu().error.empty());  // 触发面之前 error 空

        // pan 挂起队：捕获拍 tap 恰一次、设备应用延至 commit。
        std::vector<th13::sdl::AudioDevice::RequestObservation> pan_log;
        app.audio_device.request_observer =
            [&pan_log](const th13::sdl::AudioDevice::RequestObservation& obs) {
                pan_log.push_back(obs);
            };
        CHECK(app.set_voice_pan(55, 100.0f));
        CHECK_EQ(pan_log.size(), size_t(1));  // 观察在捕获拍单次发射
        CHECK_EQ(pan_log.front().kind, th13::sdl::AudioDevice::RequestObservation::Kind::Pan);
        CHECK_EQ(app.pending_voice_pans_.size(), size_t(1));
        CHECK_EQ(app.pending_voice_pans_.front().id, 55);

        // 单次 commit：全队列按捕获序冲账恰一次。
        CHECK(app.commit(kFps));
        CHECK(app.failure.empty());
        CHECK_EQ(app.pending_file_events_.size(), size_t(0));
        CHECK_EQ(app.files.spell_counters(0, 2)->attempts[0], attempts_before + 1);
        CHECK(app.files.spell_counters(0, 0)->practice_best_score >= 12340);
        // 序敏感对：+10 → 100 钳位，再 −30 → 70（反序得 80）；设备音量在
        // 该对冲账拍已触达（apply_volumes 随 PauseOption 冲账）——其后
        // PersistOptions 冲账按捕获序覆写 files 字段（40/60）但不再触设备
        // ——files 终值 40 即「persist 晚于暂停选项对冲账」的捕获序证据。
        CHECK_EQ(app.files.bgm_volume, 40);
        CHECK_EQ(app.audio_device.music_volume, 70);
        CHECK_EQ(app.files.effect_volume, 60);
        CHECK_EQ(app.audio_device.effect_volume, eff_before);
        CHECK_EQ(app.files.music_heard(3), true);
        CHECK_EQ(app.files.music_heard(5), true);
        CHECK(app.title->option_menu().error.empty());  // save 成功 → 无补记
        // pan 冲账：apply-only 无第二次 tap、挂起队清空（buffer pan 设备面
        // 由 m1.unit_audio_pan_boundary 经 PCM 钉住——本件钉队列机制面）。
        CHECK_EQ(pan_log.size(), size_t(1));
        CHECK_EQ(app.pending_voice_pans_.size(), size_t(0));

        // 双 commit 幂等：第二次 commit 无任何重复结算。
        const i32 attempts_mid = app.files.spell_counters(0, 2)->attempts[0];
        const i32 bgm_mid = app.files.bgm_volume;
        CHECK(app.commit(kFps));
        CHECK_EQ(app.files.spell_counters(0, 2)->attempts[0], attempts_mid);
        CHECK_EQ(app.files.bgm_volume, bgm_mid);
        CHECK_EQ(app.files.music_heard(3), true);
        CHECK_EQ(pan_log.size(), size_t(1));
        // 观察器卸载（守卫语义）：request_observer 持栈上 vector 引用，
        // release_run 的 stop(-1) 在 app 析构期仍会发射 Stop 观察事件——
        // 悬空引用会写已销毁 vector（heap corruption），必须显式复位。
        app.audio_device.request_observer = nullptr;

        // F2：save_replay_slot 确认拍宿主事件原位——不进待确认队列、同步完成。
        CHECK(app.start_game(1));
        input.update(0);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());   // 屏障帧（建幕）
        input.update(0);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());   // complete_loading→begin_run（recorder ready）
        CHECK(app.recorder.ready());
        CHECK_EQ(app.pending_file_events_.size(), size_t(0));
        CHECK(app.save_replay_slot(1, "T7SE"));  // 同步 IO（确认拍）
        CHECK_EQ(app.pending_file_events_.size(), size_t(0));  // 不入待确认队列
        CHECK(app.replay_slots[0].decoded != nullptr);          // 编号槽 1 → slot0
        CHECK_EQ(app.replay_slots[0].name, std::string("th13_01.rpy"));
    }
}

int main() { return th13test::run_all("m1.side_effect_boundary"); }
