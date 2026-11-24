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
    return app.pending_destination == 13 && app.failure.empty();
}

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

constexpr int kBrackets[] = {8, 10, 12, 14, 15, 17, 19, 21, 23, 27, 29, 33, 35};
constexpr int kBracketCount = 13;

std::vector<std::pair<int, Obs>>* g_probe_log = nullptr;

struct UpdateProbe {
    th13::FrameCallback callback{};
    App* app = nullptr;
    th13::FrameScheduler* attached_scheduler = nullptr;
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

struct PassLog {
    std::vector<std::pair<int, Obs>> entries;
};

}

TEST(scheduler_order_pause_frames_bracket_stable) {
    App app;
    std::vector<UpdateProbe*> probes;
    g_probe_log = new std::vector<std::pair<int, Obs>>();
    CHECK(attach_all(app, probes));
    CHECK(boot_live_run(app, "t7so-live-save.bin"));
    if (app.failure.empty()) {
        GameInput input;
        for (int barrier = 0; barrier < 2 && !app.run; ++barrier) {
            const size_t begin = g_probe_log->size();
            input.update(0);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
            assert_frame_order(std::vector<std::pair<int, Obs>>(g_probe_log->begin() + ptrdiff_t(begin),
                                                                g_probe_log->end()),
                               -1, barrier == 1, false);
        }
        CHECK(app.run != nullptr);
        CHECK_EQ(app.progress.stage_frame, 1);
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
        for (int F = 0; F < 24; ++F) {
            const size_t begin = g_probe_log->size();
            input.update(0);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
            CHECK(app.pause->active());
            CHECK_EQ(app.progress.stage_frame, frozen_at);
            assert_frame_order(std::vector<std::pair<int, Obs>>(g_probe_log->begin() + ptrdiff_t(begin),
                                                                g_probe_log->end()),
                               F, false);
        }
        CHECK_EQ(app.frames, frames_at_pause + u32(1 + wait + 24));
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
        for (int F = 0; F < 8; ++F) {
            input.update(0);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
        }
        CHECK_EQ(app.replay_consumed % 8u, 0u);
        for (int burst = 0; burst < 2; ++burst) {
            for (int k = 0; k < 5; ++k) {
                const i32 sf0 = app.progress.stage_frame;
                const u32 consumed0 = app.replay_consumed;
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
                CHECK_EQ(app.replay_consumed, consumed0 + u32(passes.size()));
                size_t complete_passes = 0;
                for (const auto& pass : passes) {
                    const bool complete = pass.size() == size_t(kBracketCount);
                    if (complete) {
                        ++complete_passes;
                        CHECK_EQ(pass.back().first, 35);
                        for (size_t i = 0; i < pass.size(); ++i)
                            CHECK_EQ(pass[i].first, kBrackets[i]);
                    } else {
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
                CHECK_EQ(complete_passes, size_t(1));
                CHECK(app.commit(kFps));
                CHECK(app.present());
                CHECK(app.failure.empty());
            }
            for (int k = 0; k < 4; ++k) {
                const size_t begin = g_probe_log->size();
                input.update(0);
                CHECK(app.update(input, false));
                CHECK(app.commit(kFps));
                CHECK(app.present());
                CHECK(app.failure.empty());
                CHECK_EQ(g_probe_log->size() - begin, size_t(kBracketCount));
            }
        }
    }
    detach_all(probes);
    delete g_probe_log;
    g_probe_log = nullptr;
}

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
        for (int F = 0; F < 6; ++F) {
            const size_t begin = g_probe_log->size();
            input.update(kZ);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            CHECK(app.failure.empty());
            CHECK_EQ(g_probe_log->size() - begin, size_t(kBracketCount));
            CHECK_EQ(app.progress.stage_frame, frozen_at);
            CHECK_EQ(app.replay_consumed, consumed_at);
            CHECK(app.environment.paused == true);
        }
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
