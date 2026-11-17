// m1.unit_draw_purity_probes —— M1/T5 Draw 纯度探针（workflow m1.draw_purity_rng
// 的本层 unit 件）。全部经公开观测面（GameRandom::instance_draw_observer、
// ApplicationState 三段入口、scheduler.add Draw 探针、AnmRegistry/AnmVm 字段），
// 零生产钩子；实测窗口=回放 th13_01 对局帧（games/replays/th13_01.rpy 生产资产）。
//
// 断言命题（M1「Draw 不得推进模拟/RNG」+ 装载 caption 绘制半段 alpha 覆写
// save/restore，ApplicationState.hpp:592-593 副作用台账 / Application.cpp:310-370）：
//   1) present×0/1/3 后双流（game 0x4dc324 / visual 0x4dc31c，ApplicationState.hpp:389-394）
//      observer 抽取计数恒 0、种子与 index 不变。正控：同一观察器在 update+commit
//      段计数 >0（两流都被模拟消费）——证明计数面本身在真实抽取上会亮，纯度断言
//      不是空转。
//   2) update+commit+present 与 update+commit 双实例 RNG 流（seed+index）逐帧全等
//      ——present 次数不改变任何流的取值序列（跨实例口径，非自比）。
//   3) 装载 caption 绘制半段（Draw72=draw_loading_battle 的 alpha 覆写
//      save/restore）：Draw 探针挂 scheduler.add 公开面 priority 73（Draw72 之后、
//      layer28@74 之前）采样 loading_animations 层31 前池 VM visual.color/
//      secondary_color，与 commit 后快照逐 VM 位级全等——覆写在半段内已恢复；
//      并逐帧验证覆写本身非空转（commit 值 ≠ 覆写值，f1..f9 至少一 VM），
//      使 save/restore 断言对「覆写被移除」与「恢复被移除」两类变异都敏感。
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

// 确定性回放 boot（m1.unit_frame_split_signature 同款拍：真实资产 + 回放种子恢复）。
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

// ---- 双流抽取计数器：GameRandom::instance_draw_observer 公开面 ----
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

// ---- Draw 段 caption 探针（priority 73：Draw72=loading_battle_draw 之后、
// layer28@74 之前；与 m1.unit_frame_split_order_probes 的 Draw73 空位一致）。
struct CaptionColorProbe {
    th13::FrameCallback callback{};
    App* app = nullptr;
    th13::FrameScheduler* attached_scheduler = nullptr;  // 登记归属（FrameCallback::scheduler private）
    int fires = 0;
    // 最近一次 Draw73 拍的层31 前池 (source_script, color, secondary_color) 序列。
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
        // FrameScheduler::remove 对未注册回调幂等（c.scheduler!=this 跳过）。
        if (attached_scheduler) attached_scheduler->remove(callback);
        attached_scheduler = nullptr;
        app = nullptr;
    }
    ~CaptionColorProbe() { detach(); }
};

// commit 后的层31 前池 (script,color,secondary) 快照（与探针同式读法）。
std::vector<std::tuple<u32, u32, u32>> commit_caption_colors(const App& app) {
    std::vector<std::tuple<u32, u32, u32>> out;
    const std::vector<th13::AnmVm*>& fore = app.loading_animations.registry.layer(31);
    for (const th13::AnmVm* vm : fore) {
        if (!vm) continue;
        out.emplace_back(vm->source_script, vm->visual.color, vm->visual.secondary_color);
    }
    return out;
}

}  // namespace

// ---- 1) present×0/1/3：双流 observer 抽取计数恒 0、index 不变 ----
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

    // 预热回放对话窗（battle f0..f59）：零输入驱动，物理键不参与对局。
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

    // (a) 观察器正控：update+commit 段双流都发生真实抽取（battle f60..f70 实测
    // game 流 ECL/弹幕抽 + visual 流 sync 特效 VM 出生抽）。若观察器计数在此为 0，
    // 后续「present 段为 0」即为计数面空转，断言无效。
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
        CHECK(counters.game_draws > 0);   // game 流被模拟段真实消费
        CHECK(counters.visual_draws > 0); // visual 流被模拟段真实消费
        guard.uninstall();
    }

    // (b) 纯度窗口：destination-free 帧上 present×{0,1,3}，双流抽取计数恒 0、
    // 种子与 index 不变。0/1/3 三臂各自从同一 commit 后状态出发。
    for (int F = 71; F <= 74; ++F) {
        input.update(0);
        CHECK(app.update(input, false));
        CHECK(app.commit(kFps));
        if (!app.failure.empty()) {
            std::printf("  失败 purity update f%d: %s\n", F, app.failure.c_str());
            return;
        }
        CHECK_EQ(app.pending_destination, -1);  // 屏障帧不入纯度窗口
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

// ---- 2) update+commit+present 与 update+commit：双流 RNG 逐帧全等 ----
TEST(draw_purity_update_present_rng_streams_frame_equal) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    // 双实例同 boot（回放种子恢复同一状态）；A 臂全 step、B 臂止于 commit。
    App arm_full, arm_split;
    CHECK(boot_replay_battle(arm_full, rpy, "t5dp-armfull-save.bin"));
    CHECK(boot_replay_battle(arm_split, rpy, "t5dp-armsplit-save.bin"));
    if (!arm_full.failure.empty() || !arm_split.failure.empty() || !arm_full.run || !arm_split.run)
        return;
    th13::GameInput in_full, in_split;
    // battle f0..f44：装载 caption 窗（f0..f9 retire 面）+ 对话开局——present 侧
    // 绘制回调最多的窗；RNG 流必须与不 present 的臂逐帧一致。
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

// ---- 3) caption 绘制半段 alpha 覆写 save/restore：Draw73 探针 VM visual 位不变 ----
TEST(draw_purity_caption_alpha_save_restore_visual_bits) {
    std::vector<u8> rpy;
    CHECK(read_repo_file("games/replays/th13_01.rpy", rpy));
    if (rpy.empty()) return;
    App app;
    CHECK(boot_replay_battle(app, rpy, "t5dp-caption-save.bin"));
    if (!app.failure.empty() || !app.run) return;
    CHECK(app.loading_prayer != 0);  // 装载树随 draw_loading(false) 建立
    CaptionColorProbe probe;         // 先于 app 声明序要求：probe 在 app 之后析构安全
    CHECK(probe.attach(app));
    th13::GameInput input;
    // battle f0..f10：Draw72 alpha 覆写窗（f10 retire 后树只标 0x20 不再覆写）。
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
        CHECK_EQ(probe.fires, F + 1);  // 探针在每次 present 的 Draw72 之后真实触发
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
        // 覆写非空转（f1..f9）：script14/16 的覆写值至少在一枚 VM 上与 commit 值
        // 不同——否则 save/restore 断言对「覆写被移除」类变异不敏感。
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
