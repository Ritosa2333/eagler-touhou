// m1.unit_frame_split_order_probes —— M1/T2 转换屏障探针（M1-plan §2/T2 探针
// (a)(b)(c)(d)）。全部经 FrameScheduler::add 公开面挂探针（优先级 73 空闲：
// AnmDrawSchedule 占 5..71、loading_battle_draw@72、layer28@74、item@75、
// fps_text@77、ascii@78；Update@4 被 AsciiFrame text.update() 占用——本票
// 探针只挂 Draw 段，不涉同优先级双回调相对序）。
//
// 探针命题：
//   (a) Title-destination 帧：Draw 拍内 run 存活且 pause active（先画后拆）；
//       present 返回后 run==nullptr && title!=nullptr（拆建在 present 尾屏障）。
//   (b) Run-destination 帧：Draw 拍内 pending_load==0；present 返回后
//       pending_load==1 && loading_frames==0（装载幕建立的两拍在帧尾，
//       Application.cpp:229；次帧不绘制语义不得重排——本探针只钉序）。
//   (c) NextStage(0xc) 帧：Draw 拍内 run 存活且 stage_transition_loading==false；
//       present 返回后 flag==true 且 run 仍存活（apply_destination 只置 flag
//       不拆场景，Application.cpp:1193-1196）；单帧后复位 flag 防污染。
//   (d) ending clear 迁移：submission_trace 断言 update 段零 screen Clear 项、
//       present 段先 Clear 后场景 Draw（select_target/clear_target 已从
//       update_scene_pass 移至 present 头）。
#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "sdl/ApplicationState.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

using th13::i32;
using th13::u32;
using App = th13::sdl::ApplicationState;
using th13::sdl::Destination;

constexpr float kFps = 60.0f;

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
        std::printf("  失败 boot: audio initialize_for_test: %s\n",
                    app.audio_device.error.c_str());
        return false;
    }
    if (!app.initialize(false)) {
        std::printf("  失败 boot: initialize: %s\n", app.failure.c_str());
        return false;
    }
    return true;
}

// Draw 段探针：闩锁第一次 Draw 拍，按模式断言帧内状态面（先画后拆）。
struct DrawProbe {
    enum class Mode { TitleDestination, RunDestination, NextStage };
    th13::FrameCallback callback{};
    App* app = nullptr;
    Mode mode = Mode::TitleDestination;
    int fires = 0;
    bool observed_ok = false;
    static i32 run(void* p) {
        auto& probe = *static_cast<DrawProbe*>(p);
        App& app = *probe.app;
        ++probe.fires;
        switch (probe.mode) {
        case Mode::TitleDestination:
            probe.observed_ok = app.run != nullptr && app.pause != nullptr &&
                                app.pause->active();
            break;
        case Mode::RunDestination:
            probe.observed_ok = app.pending_load == 0 && app.title != nullptr &&
                                app.run == nullptr;
            break;
        case Mode::NextStage:
            probe.observed_ok = app.run != nullptr && !app.stage_transition_loading;
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

private:
    th13::FrameScheduler* attached_scheduler = nullptr;  // 登记归属（FrameCallback::scheduler private）
};

// 行级 kind 统计（trace 行字节区间内 "kind":"clear"/"draw"/"target" 计数与首个位置）。
struct KindSpan {
    int clear = 0, draw = 0, target = 0;
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
        const bool is_target = line.find("\"kind\":\"target\"") != std::string::npos;
        if (is_clear) {
            if (span.first_clear < 0) span.first_clear = index;
            ++span.clear;
        }
        if (is_draw) {
            if (span.first_draw < 0) span.first_draw = index;
            ++span.draw;
        }
        if (is_target) ++span.target;
        ++index;
        if (line_end == std::string::npos || line_end >= end) break;
        pos = line_end + 1;
    }
    return span;
}

}  // namespace

TEST(destination_barrier_probe_sequence) {
    App app;
    CHECK(pin_boot(app, "t2probe-save.bin"));
    if (!app.failure.empty()) {
        std::printf("  失败 boot: %s\n", app.failure.c_str());
        return;
    }
    th13::GameInput input;

    // ---- (b) Run-destination 帧：先画（Draw 拍 pending_load==0）后建装载幕 ----
    CHECK(app.start_game(1));
    CHECK_EQ(app.pending_destination, i32(Destination::Run));
    {
        DrawProbe probe;
        CHECK(probe.attach(app, DrawProbe::Mode::RunDestination));
        input.update(0);
        CHECK(app.step(input, kFps, false));
        if (!app.failure.empty()) {
            std::printf("  失败 probe(b) step: %s\n", app.failure.c_str());
            return;
        }
        CHECK_EQ(probe.fires, 1);  // Draw 拍在帧内真实发生
        CHECK(probe.observed_ok);  // 帧内 pending_load==0、title 存活、run 未建
        CHECK_EQ(app.pending_load, 1u);      // present 尾置位（Application.cpp:1466）
        CHECK_EQ(app.loading_frames, 0u);    // Application.cpp:229
        CHECK_EQ(app.loading_startup, false);
        CHECK(app.loading_prayer != 0);      // 装载 caption 根已建
        CHECK_EQ(app.pending_destination, i32(Destination::Run));  // Run 分支不消费 destination
        probe.detach();
    }

    // ---- 次帧：complete_loading→apply_destination→begin_run（battle f0）----
    input.update(0);
    CHECK(app.update(input, false));
    CHECK(app.commit(kFps));
    CHECK(app.present());
    if (!app.failure.empty()) {
        std::printf("  失败 battle boot step: %s\n", app.failure.c_str());
        return;
    }
    CHECK(app.run != nullptr);
    CHECK_EQ(app.progress.stage_frame, 1);  // battle f0
    CHECK_EQ(app.pending_load, 0u);

    // 稳态对局帧 ×2（battle f1/f2，destination-free）。
    for (int F = 1; F <= 2; ++F) {
        input.update(0);
        CHECK(app.step(input, kFps, false));
        if (!app.failure.empty()) return;
        CHECK_EQ(app.progress.stage_frame, F + 1);
    }

    // ---- (c) NextStage 帧：只置 flag 不拆场景；Draw 拍内 flag 仍 false ----
    {
        DrawProbe probe;
        app.pending_destination = i32(Destination::NextStage);
        CHECK(probe.attach(app, DrawProbe::Mode::NextStage));
        input.update(0);
        CHECK(app.step(input, kFps, false));
        if (!app.failure.empty()) {
            std::printf("  失败 probe(c) step: %s\n", app.failure.c_str());
            return;
        }
        CHECK_EQ(probe.fires, 1);
        CHECK(probe.observed_ok);  // 帧内 run 存活且 stage_transition_loading==false
        CHECK(app.stage_transition_loading);          // present 尾 apply_destination 置位
        CHECK(app.run != nullptr);                    // 场景不拆（0x42c550 门内延续）
        CHECK_EQ(app.pending_destination, -1);        // 已消费
        CHECK_EQ(app.current_destination, i32(Destination::Run));
        probe.detach();
        // 单帧后复位（r4 R3）：flag 残留会让后续帧走 prepare_next_stage 污染同套件。
        app.stage_transition_loading = false;
    }
    // 复位后稳态帧真实可跑（防污染证据：不进装载门）。
    input.update(0);
    CHECK(app.step(input, kFps, false));
    if (!app.failure.empty()) return;
    CHECK(!app.stage_transition_loading);
    CHECK(app.run != nullptr);
    CHECK_EQ(app.pending_load, 0u);

    // ---- (a) Title-destination 帧：先画后拆（暂停菜单回标题的生产平台链）----
    {
        CHECK(app.pause != nullptr);
        CHECK(app.pause->open(th13::PauseEntrance::Pause));  // 生产入口（update 失焦路径同参）
        CHECK(app.pause->active());
        CHECK(app.return_to_title());  // RunPause 平台回调（ApplicationGame.cpp:308）
        CHECK_EQ(app.pending_destination, i32(Destination::Title));
        DrawProbe probe;
        CHECK(probe.attach(app, DrawProbe::Mode::TitleDestination));
        input.update(0);
        CHECK(app.step(input, kFps, false));
        if (!app.failure.empty()) {
            std::printf("  失败 probe(a) step: %s\n", app.failure.c_str());
            return;
        }
        CHECK_EQ(probe.fires, 1);
        CHECK(probe.observed_ok);  // Draw 拍内 run 存活且 pause active（先画）
        CHECK(app.run == nullptr);       // 后拆（present 尾 apply_destination→begin_title）
        CHECK(app.pause == nullptr);
        CHECK(app.title != nullptr);
        CHECK_EQ(app.pending_destination, -1);
        CHECK_EQ(app.current_destination, i32(Destination::Title));
        probe.detach();
    }

    // ---- (d) ending clear 迁移：update 段零 Clear、present 段先 Clear 后 Draw ----
    {
        // th13_debug_ending 同款生产引导（BrowserRuntime.cpp:581-597 语义）：
        // 结算态字段 + pending_destination=Ending + apply_destination 直消费。
        app.progress.character = 0;
        app.progress.difficulty = 1;
        app.progress.continues = 0;
        app.progress.practice_mode = 0;
        app.progress.stage = 6;
        app.title.reset();
        app.pending_destination = i32(Destination::Ending);
        CHECK(app.apply_destination());
        if (!app.failure.empty()) {
            std::printf("  失败 ending bootstrap: %s\n", app.failure.c_str());
            return;
        }
        CHECK(app.ending != nullptr);
        CHECK(app.run == nullptr);
        CHECK_EQ(app.pending_destination, -1);
        CHECK_EQ(app.pending_load, 0u);  // 稳态 ending 帧（无屏障帧混入）

        const char* trace_path = "t2-ending-clear-trace.jsonl";
        CHECK(app.graphics.submission_trace.open(trace_path));
        if (!app.graphics.submission_trace.active()) return;
        // 连续 120 个稳态 ending 帧进同一 trace：ending 首帧可能整帧无可见
        // 图元（淡入起点，draw 项为 0 属真实语义），Clear→Draw 次序断言在
        // 出现图元的帧上收口；update 段零 Clear 断言对全部帧成立。
        constexpr int kTracedFrames = 120;
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
            CHECK(mark >= 0);  // flush+tell（trace 写盘同步点）
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

        std::FILE* trace = std::fopen(trace_path, "rb");
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
        int ordered_present_spans = 0, unordered_present_spans = 0;
        int update_clear_total = 0, present_clear_total = 0, draw_frames = 0;
        for (int frame = 0; frame < kTracedFrames; ++frame) {
            const size_t update_end = size_t(update_ends[size_t(frame)]);
            CHECK(update_end <= text.size());
            const KindSpan update_span = scan_span(text, boundary, update_end);
            const size_t next_boundary =
                frame + 1 < kTracedFrames ? size_t(update_ends[size_t(frame) + 1])
                                          : text.size();
            const KindSpan present_span = scan_span(text, update_end, next_boundary);
            boundary = next_boundary;
            update_clear_total += update_span.clear;
            present_clear_total += present_span.clear;
            total_present_draws += present_span.draw;
            if (present_span.draw > 0) ++draw_frames;
            // update 段零 screen Clear（旧实现 :1345-1347 的 target+clear 对已迁出）。
            if (update_span.clear != 0)
                std::printf("  失败 probe(d) f%d: update 段含 %d 个 Clear 项（clear 未迁出模拟段）\n",
                            frame, update_span.clear);
            // present 段：每帧至少一个 Clear；出现图元的帧 Clear 必须先于场景 Draw。
            CHECK_EQ(update_span.clear, 0);
            CHECK(present_span.clear >= 1);
            if (present_span.draw > 0) {
                CHECK(present_span.first_clear >= 0);
                CHECK(present_span.first_draw >= 0);
                if (present_span.first_clear < present_span.first_draw)
                    ++ordered_present_spans;
                else {
                    ++unordered_present_spans;
                    std::printf("  失败 probe(d) f%d: present 段首 Clear(%d) 不先于首 Draw(%d)\n",
                                frame, present_span.first_clear, present_span.first_draw);
                }
            }
        }
        std::printf("# probe(d) trace: %d 帧 update 段 Clear 合计=%d，present 段 Clear 合计=%d、"
                    "Draw 合计=%d（含图元帧=%d，Clear<Draw 帧=%d，乱序帧=%d）\n",
                    kTracedFrames, update_clear_total, present_clear_total,
                    total_present_draws, draw_frames, ordered_present_spans,
                    unordered_present_spans);
        CHECK_EQ(update_clear_total, 0);
        CHECK(present_clear_total >= kTracedFrames);
        CHECK(total_present_draws >= 1);
        CHECK_EQ(unordered_present_spans, 0);
    }
}

int main() { return th13test::run_all("m1.unit_frame_split_order_probes"); }
