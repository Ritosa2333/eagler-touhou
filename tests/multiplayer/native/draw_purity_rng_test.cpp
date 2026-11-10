// m1.draw_purity_rng —— M1/T7 固定 case：Draw 纯度观察器法 + 300/600/1200
// 三窗 + present×3（workflow T7「T5 观察器法 + 300/600/1200 三窗 + present×3」）。
//
// 被测生产面（全部 public，零生产钩子）：
//   - GameRandom::instance_draw_observer（Rng.hpp:57-59 实例感知抽取观测，
//     只读不改变取值）：双流 = game 0x4dc324（app.game_random）/ visual
//     0x4dc31c（app.visual）（ApplicationState.hpp:389-394 注释）。
//   - ApplicationState::present（Application.cpp:1618-1632）：一次呈现段，
//     M1 红线「Draw 不得推进模拟/RNG」——观察器计数与双流 (seed,index) 在
//     present×N 后必须恒 0/不变。
//
// 断言命题：
//   1) 三个独立窗（300/600/1200 帧，回放 th13_01 对局 f 起）：每一帧的
//      present 段（常态 ×1、采样帧 ×3）双流抽取计数恒 0、流状态不变；
//   2) 正控：同窗 update+commit 段双流都发生真实抽取（累计 >0）——证明
//      计数面在真实抽取上会亮，纯度断言非空转；
//   3) 跨实例：300 帧窗内 update+commit+present×3 与 update+commit-only
//      双实例的双流 (seed,index) 逐帧全等——present 次数不改变任何流的
//      取值序列（跨实例口径，非自比）。
#include "../../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "sdl/ApplicationState.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace {

using th13::u32;
using th13::u8;
using App = th13::sdl::ApplicationState;
using th13::GameInput;

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

// 确定性回放 boot（套件同款拍：真实资产 + 回放种子恢复）。
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

// ---- 双流抽取计数器（instance_draw_observer 公开面）----
struct StreamCounters {
    const th13::GameRandom* visual = nullptr;
    const th13::GameRandom* game = nullptr;
    u32 visual_draws = 0;
    u32 game_draws = 0;
};
StreamCounters* g_counters = nullptr;

void counting_observer(const th13::GameRandom* stream, u32 /*before*/, u32 /*after*/) {
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

// RNG 流身份（seed+index 位级；GameRandom 0x472c90 镜像的状态对）。
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

}  // namespace

// ---- 1) 300/600/1200 三窗：每帧 present 段双流计数恒 0 + 采样帧 present×3 ----
TEST(draw_purity_rng_observer_windows_300_600_1200) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    for (const int window : {300, 600, 1200}) {
        App app;
        const std::string save_name = "t7dp-window" + std::to_string(window) + "-save.bin";
        CHECK(boot_replay_battle(app, rpy, save_name.c_str()));
        if (app.failure.empty() && app.run) {
            StreamCounters counters;
            counters.visual = &app.visual;
            counters.game = &app.game_random;
            ObserverGuard guard;
            guard.install(counters);
            u32 game_positive = 0, visual_positive = 0;
            GameInput input;
            for (int f = 0; f < window; ++f) {
                input.update(0);
                CHECK(app.update(input, false));
                CHECK(app.commit(kFps));
                if (!app.failure.empty()) {
                    std::printf("  失败 window%d f%d update: %s\n", window, f, app.failure.c_str());
                    guard.uninstall();
                    return;
                }
                // 正控累计：update+commit 段的抽取计入窗合计后清零——
                // 后续 present 段计数必须从零开始且保持为零。
                game_positive += counters.game_draws;
                visual_positive += counters.visual_draws;
                counters.game_draws = 0;
                counters.visual_draws = 0;
                // stage1 回放 f0..f1200 实测 destination-free；屏障帧出现即
                // 窗口选点漂移（回放换关在 f11675 之外）。
                if (app.pending_destination >= 0) {
                    std::printf("  失败 window%d f%d: 屏障帧混入纯度窗（pending_destination=%d）\n",
                                window, f, app.pending_destination);
                }
                CHECK_EQ(app.pending_destination, -1);
                const StreamState before = capture_streams(app);
                const bool sample = (f % 50 == 49) || (f == window - 1);
                const int presents = sample ? 3 : 1;
                for (int p = 0; p < presents; ++p) {
                    CHECK(app.present());
                    if (!app.failure.empty()) {
                        std::printf("  失败 window%d f%d present: %s\n", window, f, app.failure.c_str());
                        guard.uninstall();
                        return;
                    }
                }
                if (counters.visual_draws != 0 || counters.game_draws != 0) {
                    std::printf("  失败 window%d f%d presents=%d: visual_draws=%u game_draws=%u"
                                "（Draw 段推进了 RNG）\n", window, f, presents,
                                counters.visual_draws, counters.game_draws);
                }
                CHECK_EQ(counters.visual_draws, 0u);
                CHECK_EQ(counters.game_draws, 0u);
                const StreamState after = capture_streams(app);
                if (!(before == after)) {
                    std::printf("  失败 window%d f%d presents=%d: 流漂移 vis=%u,%u->%u,%u"
                                " game=%u,%u->%u,%u\n", window, f, presents, before.visual_seed,
                                before.visual_index, after.visual_seed, after.visual_index,
                                before.game_seed, before.game_index, after.game_seed,
                                after.game_index);
                }
                CHECK(before == after);
            }
            // 正控收口：模拟段双流都有真实抽取（计数面非空转）。
            if (game_positive == 0 || visual_positive == 0) {
                std::printf("  失败 window%d: 正控失败 game=%u visual=%u（观察器计数面失效）\n",
                            window, game_positive, visual_positive);
            }
            CHECK(game_positive > 0);
            CHECK(visual_positive > 0);
            guard.uninstall();
        }
    }
}

// ---- 2) 跨实例：update+commit+present×3 与 update+commit-only 双流逐帧全等 ----
TEST(draw_purity_rng_streams_cross_instance_present_x3_300) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    // 双实例同 boot（回放种子恢复同一状态）；full 臂每帧 present×3、split 臂
    // 不 present——300 帧全窗双流 (seed,index) 逐帧全等。
    App arm_full, arm_split;
    CHECK(boot_replay_battle(arm_full, rpy, "t7dp-xfull-save.bin"));
    CHECK(boot_replay_battle(arm_split, rpy, "t7dp-xsplit-save.bin"));
    if (!arm_full.failure.empty() || !arm_split.failure.empty() || !arm_full.run || !arm_split.run)
        return;
    GameInput in_full, in_split;
    for (int f = 0; f < 300; ++f) {
        in_full.update(0);
        CHECK(arm_full.update(in_full, false));
        CHECK(arm_full.commit(kFps));
        in_split.update(0);
        CHECK(arm_split.update(in_split, false));
        CHECK(arm_split.commit(kFps));
        if (!arm_full.failure.empty() || !arm_split.failure.empty()) {
            std::printf("  失败 cross f%d: full='%s' split='%s'\n", f,
                        arm_full.failure.c_str(), arm_split.failure.c_str());
            return;
        }
        for (int p = 0; p < 3; ++p) {
            CHECK(arm_full.present());
            if (!arm_full.failure.empty()) {
                std::printf("  失败 cross f%d present: %s\n", f, arm_full.failure.c_str());
                return;
            }
        }
        const StreamState full = capture_streams(arm_full);
        const StreamState split = capture_streams(arm_split);
        if (!(full == split)) {
            std::printf("  失败 cross f%d: full(vis %u,%u game %u,%u) != "
                        "split(vis %u,%u game %u,%u)\n", f, full.visual_seed,
                        full.visual_index, split.visual_seed, split.visual_index,
                        full.game_seed, split.game_index);
        }
        CHECK(full == split);
    }
}

int main() { return th13test::run_all("m1.draw_purity_rng"); }
