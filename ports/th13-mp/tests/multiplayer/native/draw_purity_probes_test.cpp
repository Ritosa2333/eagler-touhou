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

bool boot_replay_battle(App& app, const std::vector<u8>& rpy, const char* save_name) {
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

struct StreamCounters {
    const th13::GameRandom* visual = nullptr;
    const th13::GameRandom* game = nullptr;
    u32 visual_draws = 0;
    u32 game_draws = 0;
};
StreamCounters* g_counters = nullptr;

void counting_observer(const th13::GameRandom* stream, u32 , u32 ) {
    if (!g_counters || !stream) return;
    if (stream == g_counters->visual) ++g_counters->visual_draws;
    else if (stream == g_counters->game) ++g_counters->game_draws;
}

struct ObserverGuard {
    th13::GameRandom::InstanceDrawObserver previous = nullptr;
    bool installed = false;
    void install(StreamCounters& counters) {
        previous = th13::GameRandom::instance_draw_observer;
        g_counters = &counters;
        counters.visual_draws = 0;
        counters.game_draws = 0;
        th13::GameRandom::instance_draw_observer = &counting_observer;
        installed = true;
    }
    void uninstall() {
        if (!installed) return;
        th13::GameRandom::instance_draw_observer = previous;
        g_counters = nullptr;
        installed = false;
    }
    ~ObserverGuard() { uninstall(); }
};

struct StreamState {
    u32 visual_seed = 0, visual_index = 0;
    u32 game_seed = 0, game_index = 0;
};

StreamState capture_streams(const App& app) {
    StreamState s;
    s.visual_seed = app.visual.seed();
    s.visual_index = app.visual.index();
    s.game_seed = app.game_random.seed();
    s.game_index = app.game_random.index();
    return s;
}

bool operator==(const StreamState& a, const StreamState& b) {
    return a.visual_seed == b.visual_seed && a.visual_index == b.visual_index &&
           a.game_seed == b.game_seed && a.game_index == b.game_index;
}

struct CaptionColorProbe {
    th13::FrameCallback callback{};
    App* app = nullptr;
    th13::FrameScheduler* attached_scheduler = nullptr;
    int fires = 0;
    std::vector<std::tuple<u32, u32, u32>> snapshot;
    static i32 run(void* p) {
        auto& probe = *static_cast<CaptionColorProbe*>(p);
        ++probe.fires;
        probe.snapshot.clear();
        const std::vector<th13::AnmVm*>& fore = probe.app->loading_animations.registry.layer(31);
        for (const th13::AnmVm* vm : fore) {
            if (!vm) continue;
            probe.snapshot.emplace_back(vm->source_script, vm->visual.color,
                                        vm->visual.secondary_color);
        }
        return i32(th13::FrameAction::Continue);
    }
    bool attach(App& target) {
        app = &target;
        callback.owner = this;
        callback.enabled = true;
        callback.run = &CaptionColorProbe::run;
        if (target.scheduler.add(callback, th13::FramePass::Draw, 73) < 0) return false;
        attached_scheduler = &target.scheduler;
        return true;
    }
    void detach() {
        if (attached_scheduler) attached_scheduler->remove(callback);
        attached_scheduler = nullptr;
        app = nullptr;
    }
    ~CaptionColorProbe() { detach(); }
};

std::vector<std::tuple<u32, u32, u32>> commit_caption_colors(const App& app) {
    std::vector<std::tuple<u32, u32, u32>> out;
    const std::vector<th13::AnmVm*>& fore = app.loading_animations.registry.layer(31);
    for (const th13::AnmVm* vm : fore) {
        if (!vm) continue;
        out.emplace_back(vm->source_script, vm->visual.color, vm->visual.secondary_color);
    }
    return out;
}

}

TEST(draw_purity_present_x0_x1_x3_observer_zero) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app;
    CHECK(boot_replay_battle(app, rpy, "t5dp-purity-save.bin"));
    if (!app.failure.empty() || !app.run) return;
    th13::GameInput input;
    StreamCounters counters;
    counters.visual = &app.visual;
    counters.game = &app.game_random;

    for (int F = 0; F < 60; ++F) {
        input.update(0);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        CHECK(app.present());
        if (!app.failure.empty()) {
            std::printf("  失败 warmup f%d: %s\n", F, app.failure.c_str());
            return;
        }
    }

    {
        ObserverGuard guard;
        guard.install(counters);
        for (int F = 60; F <= 70; ++F) {
            input.update(0);
            CHECK(app.update(input, false));
            CHECK(app.commit(kFps));
            CHECK(app.present());
            if (!app.failure.empty()) {
                std::printf("  失败 sanity f%d: %s\n", F, app.failure.c_str());
                return;
            }
        }
        CHECK(counters.game_draws > 0);
        CHECK(counters.visual_draws > 0);
        guard.uninstall();
    }

    for (int F = 71; F <= 74; ++F) {
        input.update(0);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        if (!app.failure.empty()) {
            std::printf("  失败 purity update f%d: %s\n", F, app.failure.c_str());
            return;
        }
        CHECK_EQ(app.pending_destination, -1);
        for (const int presents : {0, 1, 3}) {
            ObserverGuard guard;
            guard.install(counters);
            const StreamState before = capture_streams(app);
            for (int p = 0; p < presents; ++p) CHECK(app.present());
            const u32 visual_draws = counters.visual_draws;
            const u32 game_draws = counters.game_draws;
            guard.uninstall();
            if (visual_draws != 0 || game_draws != 0) {
                std::printf("  失败 purity f%d presents=%d: visual_draws=%u game_draws=%u"
                            "（Draw 段推进了 RNG）\n", F, presents, visual_draws, game_draws);
            }
            CHECK_EQ(visual_draws, 0u);
            CHECK_EQ(game_draws, 0u);
            const StreamState after = capture_streams(app);
            if (!(before == after)) {
                std::printf("  失败 purity f%d presents=%d: RNG 流漂移 vis=%u,%u->%u,%u"
                            " game=%u,%u->%u,%u\n", F, presents, before.visual_seed,
                            before.visual_index, after.visual_seed, after.visual_index,
                            before.game_seed, before.game_index, after.game_seed,
                            after.game_index);
            }
            CHECK(before == after);
            if (!app.failure.empty()) {
                std::printf("  失败 purity present f%d: %s\n", F, app.failure.c_str());
                return;
            }
        }
    }
}

TEST(draw_purity_update_present_rng_streams_frame_equal) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App arm_full, arm_split;
    CHECK(boot_replay_battle(arm_full, rpy, "t5dp-armfull-save.bin"));
    CHECK(boot_replay_battle(arm_split, rpy, "t5dp-armsplit-save.bin"));
    if (!arm_full.failure.empty() || !arm_split.failure.empty() || !arm_full.run || !arm_split.run)
        return;
    th13::GameInput in_full, in_split;
    for (int F = 0; F <= 44; ++F) {
        in_full.update(0);
        CHECK(arm_full.update(in_full, false));
        CHECK(arm_full.commit(kFps));
        CHECK(arm_full.present());
        in_split.update(0);
        CHECK(arm_split.update(in_split, false));
        CHECK(arm_split.commit(kFps));
        if (!arm_full.failure.empty() || !arm_split.failure.empty()) {
            std::printf("  失败 arm drive f%d: full='%s' split='%s'\n", F,
                        arm_full.failure.c_str(), arm_split.failure.c_str());
            return;
        }
        const StreamState full = capture_streams(arm_full);
        const StreamState split = capture_streams(arm_split);
        if (!(full == split)) {
            std::printf("  失败 stream f%d: full(vis %u,%u game %u,%u) != "
                        "split(vis %u,%u game %u,%u)\n", F, full.visual_seed,
                        full.visual_index, full.game_seed, full.game_index,
                        split.visual_seed, split.visual_index, split.game_seed,
                        split.game_index);
        }
        CHECK(full == split);
    }
}

TEST(draw_purity_caption_alpha_save_restore_visual_bits) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app;
    CHECK(boot_replay_battle(app, rpy, "t5dp-caption-save.bin"));
    if (!app.failure.empty() || !app.run) return;
    CHECK(app.loading_prayer != 0);
    CaptionColorProbe probe;
    CHECK(probe.attach(app));
    th13::GameInput input;
    for (int F = 0; F <= 10; ++F) {
        input.update(0);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        if (!app.failure.empty()) {
            std::printf("  失败 caption update f%d: %s\n", F, app.failure.c_str());
            return;
        }
        const std::vector<std::tuple<u32, u32, u32>> commit_colors = commit_caption_colors(app);
        const i32 battle_frame = app.progress.stage_frame - 1;
        CHECK_EQ(battle_frame, F);
        CHECK(app.present());
        if (!app.failure.empty()) {
            std::printf("  失败 caption present f%d: %s\n", F, app.failure.c_str());
            return;
        }
        CHECK_EQ(probe.fires, F + 1);
        if (probe.snapshot != commit_colors) {
            std::printf("  失败 caption f%d: Draw73 拍 VM visual 位与 commit 后不一致"
                        "（alpha 覆写 save/restore 泄漏）；commit=%zu 项\n", F,
                        commit_colors.size());
            for (size_t i = 0; i < commit_colors.size() && i < probe.snapshot.size(); ++i) {
                if (commit_colors[i] != probe.snapshot[i])
                    std::printf("    vm[%zu] script=%u commit=%08x/%08x draw73=%08x/%08x\n", i,
                                std::get<0>(commit_colors[i]), std::get<1>(commit_colors[i]),
                                std::get<2>(commit_colors[i]), std::get<1>(probe.snapshot[i]),
                                std::get<2>(probe.snapshot[i]));
            }
        }
        CHECK(probe.snapshot == commit_colors);
        if (battle_frame >= 1 && battle_frame <= 9) {
            const u32 fade = u32(30 - battle_frame);
            bool overwrite_visible = false;
            for (const auto& [script, color, secondary] : commit_colors) {
                u32 alpha = 0;
                bool in_pass = false;
                switch (script) {
                case 14: alpha = u32(u8(0x9Fu * fade / 30u)); in_pass = true; break;
                case 15: alpha = 0; in_pass = true; break;
                case 16: alpha = u32(u8(0x66u * (10u - u32(battle_frame)) / 10u));
                         in_pass = true; break;
                default: break;
                }
                if (!in_pass) continue;
                const u32 over_color = (color & 0x00ffffffu) | (alpha << 24);
                const u32 over_secondary = (secondary & 0x00ffffffu) | (alpha << 24);
                if (over_color != color || over_secondary != secondary) overwrite_visible = true;
            }
            if (!overwrite_visible) {
                std::printf("  失败 caption f%d: 覆写值与 commit 值全等（非空转前提失效，"
                            "请核对窗口相位）\n", F);
            }
            CHECK(overwrite_visible);
        }
    }
    probe.detach();
}

int main() { return th13test::run_all("m1.unit_draw_purity_probes"); }
