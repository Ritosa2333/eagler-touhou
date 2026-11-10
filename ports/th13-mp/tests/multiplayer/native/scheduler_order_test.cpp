// m1.scheduler_order —— M1/T7 固定 case：Update 段调度次序锁定，T5 探针扩
// 暂停帧与 Replay 快进 restart 循环（workflow T7「Update 30→restart 一帧多次
// update_scene_pass 序不重排」）。
//
// 被测生产面（全部 public，零生产钩子）：
//   - FrameScheduler::add/remove/contains（FrameScheduler.hpp）公开探针挂点；
//     探针优先级取生产回调登记位之间的空闲 bracket（8/10/12/14/15/17/19/21/
//     23/27/29/33/35；Update@4 被 AsciiFrame text.update() 占用——禁用位）。
//   - ApplicationState::update 的 do/while restart 循环（Application.cpp:
//     1410-1418）：replay_fast_forward@30（:542-556 注册）返回
//     FrameAction::StopSuccess 截断本 pass → restart_update → 整段重跑；
//     每次重启从 bracket 8 重新升序穿过——「30→restart 序不重排」的本体。
//   - 暂停（Application.cpp:1403-1408 暂停入口）：失焦/Esc 开门后对局冻结
//     （RunScene.cpp:659 environment.paused 早退），帧体与调度照常——暂停帧
//     的 bracket 序与归因不得漂移。
//
// 断言命题：
//   A) 每帧探针触发序严格按 bracket 升序（含屏障帧/暂停帧/恢复帧）。
//   B) stage_frame 增量恰落 (19,21)（RunScene::update@20）；暂停帧全 bracket
//      持平且跨帧冻结（扩暂停帧窗）。
//   C) scenery->frames 增量恰落 (12,14)（scenery@13）；暂停帧持平。
//   D) Replay 快进 restart：按住 Z 的一次 update() 内多 pass；每 pass 升序、
//      截断 pass 不含 ≥33 bracket（30 的 StopSuccess 先于 34 主链）、恰一个
//      完成 pass；stage_frame 每恰 +1/pass；跨两次 burst 循环稳定。
//   E) 暂停中按住 Z：快进门被 paused 拦下（replay_fast_forward_restart 的
//      !paused 条件）——每 update() 恰 13 条探针（单 pass）、对局仍冻结。
#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "sdl/ApplicationState.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

namespace {

using th13::i32;
using th13::u32;
using th13::u8;
using App = th13::sdl::ApplicationState;
using th13::FramePass;
using th13::GameInput;

constexpr float kFps = 60.0f;
constexpr u32 kZ = 0x01;
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
    return app.pending_destination == 13 && app.failure.empty();  // Destination::Run
}

// ---- bracket 观测快照（只读公开字段）----
struct Obs {
    int stage_frame = 0;
    u32 frames = 0;
    u32 vis_seed = 0, vis_index = 0;
    u32 game_seed = 0, game_index = 0;
    u32 scenery_frames = 0;
    bool has_run = false;

    bool same_world(const Obs& o) const {
        return stage_frame == o.stage_frame && frames == o.frames &&
               vis_seed == o.vis_seed && vis_index == o.vis_index &&
               game_seed == o.game_seed && game_index == o.game_index &&
               scenery_frames == o.scenery_frames;
    }
};

Obs observe(App& app) {
    Obs o;
    o.stage_frame = app.progress.stage_frame;
    o.frames = app.frames;
    o.vis_seed = app.visual.seed();
    o.vis_index = app.visual.index();
    o.game_seed = app.game_random.seed();
    o.game_index = app.game_random.index();
    o.has_run = app.run != nullptr;
    if (app.run && app.run->scenery) o.scenery_frames = app.run->scenery->frames;
    return o;
}

// ---- Update 段 bracket 探针 ----
constexpr int kBrackets[] = {8, 10, 12, 14, 15, 17, 19, 21, 23, 27, 29, 33, 35};
constexpr int kBracketCount = 13;

std::vector<std::pair<int, Obs>>* g_probe_log = nullptr;

struct UpdateProbe {
    th13::FrameCallback callback{};
    App* app = nullptr;
    th13::FrameScheduler* attached_scheduler = nullptr;  // 登记归属（FrameCallback::scheduler private）
    int priority = 0;
    static i32 run(void* p) {
        auto& probe = *static_cast<UpdateProbe*>(p);
        g_probe_log->push_back({probe.priority, observe(*probe.app)});
        return i32(th13::FrameAction::Continue);
    }
    bool attach(App& target, int priority_) {
        app = &target;
        priority = priority_;
        callback.owner = this;
        callback.enabled = true;
        callback.run = &UpdateProbe::run;
        if (target.scheduler.add(callback, FramePass::Update, priority) < 0) return false;
        attached_scheduler = &target.scheduler;
        return true;
    }
    void detach() {
        // FrameScheduler::remove 对未注册回调幂等（c.scheduler!=this 跳过）。
        if (attached_scheduler) attached_scheduler->remove(callback);
        attached_scheduler = nullptr;
        app = nullptr;
    }
    ~UpdateProbe() { detach(); }
};

bool attach_all(App& app, std::vector<UpdateProbe*>& probes) {
    for (int priority : kBrackets) {
        probes.push_back(new UpdateProbe());
        if (!probes.back()->attach(app, priority)) return false;
    }
    return true;
}

void detach_all(std::vector<UpdateProbe*>& probes) {
    for (UpdateProbe* probe : probes) {
        probe->detach();
        delete probe;
    }
    probes.clear();
}

// 单帧断言：13 条记录、bracket 严格升序；stage_frame 归因 (19,21]；
// scenery 归因 (12,14]（仅 expect_scenery 且 run 存活帧断言增量，其余持平）。
void assert_frame_order(const std::vector<std::pair<int, Obs>>& entries, int frame_index,
                        bool expect_advance, bool expect_scenery = true) {
    CHECK_EQ(entries.size(), size_t(kBracketCount));
    for (size_t i = 0; i < entries.size() && i < size_t(kBracketCount); ++i)
        CHECK_EQ(entries[i].first, kBrackets[i]);
    const Obs* first = entries.empty() ? nullptr : &entries.front().second;
    const Obs* pre21 = nullptr;
    const Obs* post21 = nullptr;
    const Obs* pre12 = nullptr;
    const Obs* post14 = nullptr;
    for (const auto& entry : entries) {
        if (entry.first == 12) pre12 = &entry.second;
        if (entry.first == 14) post14 = &entry.second;
        if (entry.first == 19) pre21 = &entry.second;
        if (entry.first == 21) post21 = &entry.second;
    }
    if (first && pre21 && post21) {
        // (8,19] 持平于帧前值；(21,35] 持平于帧后值；增量恰落 (19,21]。
        for (const auto& entry : entries) {
            if (entry.first < 21 && entry.second.stage_frame != first->stage_frame) {
                std::printf("  失败 stage_frame f%d: bracket %d 偏离（应持平 %d，实际 %d）\n",
                            frame_index, entry.first, first->stage_frame, entry.second.stage_frame);
                CHECK(false);
            }
            if (entry.first > 21 && entry.second.stage_frame != post21->stage_frame) {
                std::printf("  失败 stage_frame f%d: bracket %d 偏离（应持平 %d，实际 %d）\n",
                            frame_index, entry.first, post21->stage_frame, entry.second.stage_frame);
                CHECK(false);
            }
        }
        if (post21->stage_frame != pre21->stage_frame + (expect_advance ? 1 : 0)) {
            std::printf("  失败 stage_frame 增量 f%d: pre21=%d post21=%d expect_advance=%d\n",
                        frame_index, pre21->stage_frame, post21->stage_frame,
                        expect_advance ? 1 : 0);
            CHECK(false);
        } else {
            CHECK(true);
        }
    }
    if (pre12 && post14 && pre12->has_run && expect_scenery) {
        if (post14->scenery_frames != pre12->scenery_frames + (expect_advance ? 1 : 0)) {
            std::printf("  失败 scenery 增量 f%d: pre12=%u post14=%u expect_advance=%d\n",
                        frame_index, pre12->scenery_frames, post14->scenery_frames,
                        expect_advance ? 1 : 0);
            CHECK(false);
        } else {
            CHECK(true);
        }
    }
}

// 快进 pass 切分（探针@8 = pass 起点；update() 调用段 [begin,end) 内）。
struct PassLog {
    std::vector<std::pair<int, Obs>> entries;
};

}  // namespace

// ---- A/B/C：live 开局窗 + 扩暂停帧（开→24 帧→关→恢复）bracket 序稳定 ----
TEST(scheduler_order_pause_frames_bracket_stable) {
    App app;
    std::vector<UpdateProbe*> probes;
    g_probe_log = new std::vector<std::pair<int, Obs>>();
    CHECK(attach_all(app, probes));
    CHECK(boot_live_run(app, "t7so-live-save.bin"));
    if (app.failure.empty()) {
        GameInput input;
        // 屏障两拍（同 unit 件）：第一拍 title 分支跑调度、present 尾建装载幕
        // （stage_frame 冻结）；第二拍 update 头 complete_loading→begin_run
        // （battle f0，stage_frame 在 (19,21] 内 +1）。
        for (int barrier = 0; barrier < 2 && !app.run; ++barrier) {
            const size_t begin = g_probe_log->size();
            input.update(0);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
            assert_frame_order(std::vector<std::pair<int, Obs>>(g_probe_log->begin() + ptrdiff_t(begin),
                                                                g_probe_log->end()),
                               -1, barrier == 1, /*expect_scenery=*/false);
        }
        CHECK(app.run != nullptr);
        CHECK_EQ(app.progress.stage_frame, 1);  // battle f0
        // battle f1..f6 正控（序 + 归因 + 推进）。
        for (int F = 1; F <= 6; ++F) {
            const size_t begin = g_probe_log->size();
            input.update(0);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
            CHECK_EQ(app.progress.stage_frame, F + 1);
            assert_frame_order(std::vector<std::pair<int, Obs>>(g_probe_log->begin() + ptrdiff_t(begin),
                                                                g_probe_log->end()),
                               F, true);
        }
        const i32 frozen_at = app.progress.stage_frame;
        const u32 frames_at_pause = app.frames;
        // Esc 开暂停（update 段头开门——当帧冻结）。
        {
            const size_t begin = g_probe_log->size();
            input.update(kEsc);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
            CHECK(app.pause->active());
            CHECK(app.pause->state.screen == th13::PauseScreen::Pause);
            CHECK(app.environment.paused == true);
            CHECK_EQ(app.progress.stage_frame, frozen_at);
            assert_frame_order(std::vector<std::pair<int, Obs>>(g_probe_log->begin() + ptrdiff_t(begin),
                                                                g_probe_log->end()),
                               -2, false);
        }
        // 等菜单 phase 6（age>9 → phase(6)），暂停帧序稳定。
        int wait = 0;
        while (app.pause->state.phase != 6 && wait < 20) {
            const size_t begin = g_probe_log->size();
            input.update(0);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
            assert_frame_order(std::vector<std::pair<int, Obs>>(g_probe_log->begin() + ptrdiff_t(begin),
                                                                g_probe_log->end()),
                               -3, false);
            ++wait;
        }
        CHECK_EQ(app.pause->state.phase, 6);
        // 扩暂停窗：24 帧全暂停——逐帧序/冻结/帧体推进。
        for (int F = 0; F < 24; ++F) {
            const size_t begin = g_probe_log->size();
            input.update(0);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
            CHECK(app.pause->active());
            CHECK_EQ(app.progress.stage_frame, frozen_at);  // 跨帧冻结
            assert_frame_order(std::vector<std::pair<int, Obs>>(g_probe_log->begin() + ptrdiff_t(begin),
                                                                g_probe_log->end()),
                               F, false);
        }
        CHECK_EQ(app.frames, frames_at_pause + u32(1 + wait + 24));  // 帧体照常 commit
        // Esc 关（phase16 收场后 finish_selection 恢复）。恢复拍在 Update@11
        // （PauseFrame 菜单回调）中途解除冻结——恢复帧的 scenery@13/对局@20
        // 在同 pass 内照常推进一帧；其余暂停帧全冻结。
        input.update(kEsc);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());
        CHECK(app.failure.empty());
        int close = 0;
        while (app.environment.paused && close < 40) {
            const size_t begin = g_probe_log->size();
            const bool paused_before = app.environment.paused;
            input.update(0);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
            const bool resumed_this_frame = paused_before && !app.environment.paused;
            assert_frame_order(std::vector<std::pair<int, Obs>>(g_probe_log->begin() + ptrdiff_t(begin),
                                                                g_probe_log->end()),
                               -4, resumed_this_frame);
            ++close;
        }
        CHECK(app.environment.paused == false);
        CHECK(!app.pause->active());
        CHECK_EQ(app.progress.scene_flags & 0x10u, 0u);
        // 恢复后 10 帧：推进归因复位（序不因暂停窗漂移）。
        for (int F = 0; F < 10; ++F) {
            const size_t begin = g_probe_log->size();
            input.update(0);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
            assert_frame_order(std::vector<std::pair<int, Obs>>(g_probe_log->begin() + ptrdiff_t(begin),
                                                                g_probe_log->end()),
                               F, true);
        }
        CHECK(app.progress.stage_frame > frozen_at);
    }
    detach_all(probes);
    delete g_probe_log;
    g_probe_log = nullptr;
}

// ---- D：Replay 快进 restart 循环（两次 burst；pass 序不重排）----
TEST(scheduler_order_replay_fastforward_restart_loop) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app;
    std::vector<UpdateProbe*> probes;
    g_probe_log = new std::vector<std::pair<int, Obs>>();
    CHECK(attach_all(app, probes));
    CHECK(boot_replay_battle(app, rpy, "t7so-ff-save.bin"));
    if (app.failure.empty() && app.run) {
        GameInput input;
        // f0..f7 零输入基线（consumed 对齐 8 的倍数，快进模式 8 pass/call）。
        for (int F = 0; F < 8; ++F) {
            input.update(0);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
        }
        CHECK_EQ(app.replay_consumed % 8u, 0u);
        // 两次快进 burst（restart 循环稳定性），每次 5 个快进 update + 4 帧间隔。
        for (int burst = 0; burst < 2; ++burst) {
            for (int k = 0; k < 5; ++k) {
                const i32 sf0 = app.progress.stage_frame;
                const u32 consumed0 = app.replay_consumed;
                const size_t begin = g_probe_log->size();
                input.update(kZ);
                CHECK(app.update(input, false));
                CHECK(app.failure.empty());
                // 分 pass（探针@8 = pass 起点）。
                std::vector<std::vector<std::pair<int, Obs>>> passes;
                for (size_t i = begin; i < g_probe_log->size(); ++i) {
                    if ((*g_probe_log)[i].first == 8 || passes.empty())
                        passes.push_back(std::vector<std::pair<int, Obs>>());
                    passes.back().push_back((*g_probe_log)[i]);
                }
                CHECK(passes.size() >= 2);  // 快进重启真实发生
                CHECK_EQ(app.progress.stage_frame, sf0 + i32(passes.size()));  // 每 pass 推进一帧
                CHECK_EQ(app.replay_consumed, consumed0 + u32(passes.size())); // 每 pass 恰消费一帧
                size_t complete_passes = 0;
                for (const auto& pass : passes) {
                    const bool complete = pass.size() == size_t(kBracketCount);
                    if (complete) {
                        ++complete_passes;
                        CHECK_EQ(pass.back().first, 35);
                        for (size_t i = 0; i < pass.size(); ++i)
                            CHECK_EQ(pass[i].first, kBrackets[i]);  // 完成 pass 升序全 brackets
                    } else {
                        // 截断 pass：30 的 StopSuccess 剪掉 33/35（34 主链未跑）。
                        CHECK(pass.size() < size_t(kBracketCount));
                        bool saw_late = false;
                        for (const auto& entry : pass)
                            if (entry.first >= 33) saw_late = true;
                        if (saw_late) {
                            std::printf("  失败 ff burst%d k%d: 截断 pass 触发 >=33 bracket"
                                        "（快进未在 30 截断）\n", burst, k);
                        }
                        CHECK(!saw_late);
                    }
                }
                CHECK_EQ(complete_passes, size_t(1));  // 每次 update() 恰一个完成 pass
                CHECK(app.commit(kFps));
                CHECK(app.present());
                CHECK(app.failure.empty());
            }
            // burst 间隔：松开 Z 4 帧（单 pass，序稳定）。
            for (int k = 0; k < 4; ++k) {
                const size_t begin = g_probe_log->size();
                input.update(0);
                CHECK(app.update(input, false));
                CHECK(app.commit(kFps));
                CHECK(app.present());
                CHECK(app.failure.empty());
                CHECK_EQ(g_probe_log->size() - begin, size_t(kBracketCount));  // 单 pass
            }
        }
    }
    detach_all(probes);
    delete g_probe_log;
    g_probe_log = nullptr;
}

// ---- E：暂停中按住 Z——快进门被 paused 拦下（单 pass）+ 对局仍冻结 ----
TEST(scheduler_order_fastforward_gate_paused_single_pass) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app;
    std::vector<UpdateProbe*> probes;
    g_probe_log = new std::vector<std::pair<int, Obs>>();
    CHECK(attach_all(app, probes));
    CHECK(boot_replay_battle(app, rpy, "t7so-ffpause-save.bin"));
    if (app.failure.empty() && app.run) {
        GameInput input;
        for (int F = 0; F < 8; ++F) {
            input.update(0);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
        }
        // Esc 开暂停（等 phase 6 后再入快进帧，隔离菜单相位）。
        input.update(kEsc);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());
        CHECK(app.failure.empty());
        CHECK(app.pause->active());
        int wait = 0;
        while (app.pause->state.phase != 6 && wait < 20) {
            input.update(0);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
            ++wait;
        }
        CHECK_EQ(app.pause->state.phase, 6);
        const i32 frozen_at = app.progress.stage_frame;
        const u32 consumed_at = app.replay_consumed;
        // 暂停中按住 Z：6 帧——单 pass（快进门 !paused 拦下）、对局冻结、
        // 回放游标冻结。
        for (int F = 0; F < 6; ++F) {
            const size_t begin = g_probe_log->size();
            input.update(kZ);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
            CHECK_EQ(g_probe_log->size() - begin, size_t(kBracketCount));  // 单 pass
            CHECK_EQ(app.progress.stage_frame, frozen_at);
            CHECK_EQ(app.replay_consumed, consumed_at);  // 回放游标暂停冻结
            CHECK(app.environment.paused == true);
        }
        // 暂停中松开 Z、Esc 恢复；恢复后快进重启恢复多 pass（门只看 paused）。
        input.update(kEsc);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());
        CHECK(app.failure.empty());
        int close = 0;
        while (app.environment.paused && close < 40) {
            input.update(0);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
            ++close;
        }
        CHECK(app.environment.paused == false);
        // 恢复后按住 Z：多 pass 重启真实发生（每 pass 推进一帧）。
        {
            const i32 sf0 = app.progress.stage_frame;
            const size_t begin = g_probe_log->size();
            input.update(kZ);
            CHECK(app.update(input, false));
            CHECK(app.failure.empty());
            std::vector<std::vector<std::pair<int, Obs>>> passes;
            for (size_t i = begin; i < g_probe_log->size(); ++i) {
                if ((*g_probe_log)[i].first == 8 || passes.empty())
                    passes.push_back(std::vector<std::pair<int, Obs>>());
                passes.back().push_back((*g_probe_log)[i]);
            }
            CHECK(passes.size() >= 2);
            CHECK_EQ(app.progress.stage_frame, sf0 + i32(passes.size()));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
        }
    }
    detach_all(probes);
    delete g_probe_log;
    g_probe_log = nullptr;
}

int main() { return th13test::run_all("m1.scheduler_order"); }
