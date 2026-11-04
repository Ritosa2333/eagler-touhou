// 换关装载钟（0x42b990 / 0x42c550 / 0x409270 / 0x45f430）与关首数值重置
// 的专用回归。
//
// 原版证据：
//   0x42b990:45  装载入口 stage ctx +0x60 |= 4（bit2）；
//   0x42b990:231 装载成功清 bit2（FUN_0043b5a0 之后）；
//   0x42c550:248-261 `(scene_flags & 4)==0` 才放行 FUN_0042a370；
//   0x45f430     kind5 退场淡出在 bit0|bit2 置位时冻结计时（kind0 45ef50 无门）；
//   0x409270     装载门 state0 分支创建 30 帧退场淡出（duration 0x1e）。
//   0x42b990:103-110 dcc30!=0（新局/Retry）才清 be7c0/be7d0 并写 be7dc/be7e4；
//   0x42b990:54-78 的 0xc 换关延续只做 be810=max(be810,be7c0)。
//
// 本套件只测 Application.cpp 生产 step()/begin_run() 直接调用的同一批纯函数
// （sdl/StageTransitionClock.hpp）；不构造 SDL 设备层，也不重实现比较逻辑。
#define TH13_APPLICATION_PLATFORMS_ONLY
#include "../harness/test_framework.hpp"

#include "sdl/StageTransitionClock.hpp"

#include <fstream>
#include <iterator>
#include <string>

using namespace th13;
using namespace th13::sdl;

namespace {
// 静态调用点契约：helper 单测无法发现生产调用把 continuing 取反。读取生产
// Application.cpp 源码，锁定「传入 continuing」与两处装载完成/淡出门接线。
std::string production_application_source() {
    const char* path = "eagler-touhou/ports/th13/web/cpp/sdl/Application.cpp";
    std::ifstream file(path);
    return std::string(std::istreambuf_iterator<char>(file), {});
}
}  // namespace

TEST(loader_completion_clears_only_42b990_bit2) {
    // 门首：其它位（0x10 覆盖层、0x4000 通关演出）原样保留。
    CHECK_EQ(stage_loader_completed_scene_flags(kStageLoaderSceneFlag), 0u);
    CHECK_EQ(stage_loader_completed_scene_flags(0x4004u), 0x4000u);
    CHECK_EQ(stage_loader_completed_scene_flags(0x4014u), 0x4010u);
    CHECK_EQ(stage_loader_completed_scene_flags(0u), 0u);
}

TEST(kind5_exit_fade_freezes_through_loader_gate_then_ticks) {
    // 0x42b990:45..231 的装载窗：bit2 置位，45f430 计时器保持 0（alpha 0）。
    const u32 loader_gate = kStageLoaderSceneFlag;
    i32 time = 0;
    for (i32 frame = 0; frame < 120; ++frame)
        if (!stage_loading_holds_game_fade(5, loader_gate)) ++time;
    CHECK_EQ(time, 0);

    // truth table：bit0 与 bit2 任一置位都冻结；kind0 淡入不受门控。
    CHECK(stage_loading_holds_game_fade(5, 4u));
    CHECK(stage_loading_holds_game_fade(5, 1u));
    CHECK(!stage_loading_holds_game_fade(5, 0u));
    CHECK(!stage_loading_holds_game_fade(0, 4u));
    CHECK(!stage_loading_holds_game_fade(0, 0u));

    // 装载成功帧（0x42b990:231）清 bit2 后，0x409270 的 30 帧退场淡出才走：
    // time 0→30，duration=30 时 alpha 255；duration+2 移除（45f430）。
    const u32 loader_done = stage_loader_completed_scene_flags(loader_gate);
    for (i32 frame = 0; frame < 30; ++frame)
        if (!stage_loading_holds_game_fade(5, loader_done)) ++time;
    CHECK_EQ(time, 30);
    CHECK_EQ(time >= 30, true);   // alpha = 255 at duration
    CHECK_EQ(time >= 32, false);  // remove only at duration+2
    if (!stage_loading_holds_game_fade(5, loader_done)) ++time;
    ++time;
    CHECK_EQ(time, 32);
    CHECK_EQ(time >= 32, true);
}

TEST(kind0_enter_fade_never_holds_on_loader_bit) {
    i32 time = 0;
    for (i32 frame = 0; frame < 30; ++frame)
        if (!stage_loading_holds_game_fade(0, kStageLoaderSceneFlag)) ++time;
    CHECK_EQ(time, 30);
}

TEST(new_run_and_retry_reset_points_like_42b990_dcc30_nonzero) {
    // Normal 行是实机取证基线：开局 max_point=10000×100=1,000,000，
    // cap=250000×100=25,000,000（s2-root audit 004be7dc 起涨前值）。
    constexpr i32 init[6] = {500000, 1000000, 1500000, 2000000, 2000000, 10000000};
    constexpr i32 cap[6] = {10000000, 25000000, 50000000, 50000000, 50000000, 100000000};
    for (i32 difficulty = 0; difficulty < 6; ++difficulty) {
        i32 score = 987654, max_point = 7, max_point_cap = 8, graze = 9;
        run_entry_reset_points(false, difficulty, score, max_point, max_point_cap, graze);
        CHECK_EQ(score, 0);
        CHECK_EQ(max_point, init[difficulty]);
        CHECK_EQ(max_point_cap, cap[difficulty]);
        CHECK_EQ(graze, 0);
    }
}

TEST(next_stage_continuation_preserves_points_and_never_rewrites_cap) {
    // 0xc 换关（dcc30==0，:54-78）：be7c0/be7d0/be7dc/be7e4 全是上一关末值。
    // 用非表值的哨兵证明函数没有夹带重置或重写封顶。
    i32 score = 7858841, max_point = 1095980, max_point_cap = 25000000, graze = 4321;
    run_entry_reset_points(true, 1, score, max_point, max_point_cap, graze);
    CHECK_EQ(score, 7858841);
    CHECK_EQ(max_point, 1095980);
    CHECK_EQ(max_point_cap, 25000000);
    CHECK_EQ(graze, 4321);
    // 即便难度索引越界也不得触碰（继续路径不读表）。
    run_entry_reset_points(true, 99, score, max_point, max_point_cap, graze);
    CHECK_EQ(score, 7858841);
    CHECK_EQ(max_point, 1095980);
    CHECK_EQ(max_point_cap, 25000000);
    CHECK_EQ(graze, 4321);
}

TEST(production_begin_run_passes_continuing_not_inverted) {
    const std::string source = production_application_source();
    CHECK(!source.empty());
    // 正确调用：continuing=true（0xc 换关）→ helper 保留四项数值；
    // continuing=false（新局/Retry/回放冷启动）→ helper 重置。
    CHECK(source.find("run_entry_reset_points(continuing, progress.difficulty") !=
          std::string::npos);
    CHECK(source.find("run_entry_reset_points(!continuing") == std::string::npos);
}

TEST(production_step_wires_loader_completion_and_fade_gate) {
    const std::string source = production_application_source();
    CHECK(!source.empty());
    // 装载完成帧（start_transition 成功后）必须清 0x42b990 bit2。
    CHECK(source.find("if (!run->start_transition()) return false;") != std::string::npos);
    CHECK(source.find("stage_loader_completed_scene_flags(progress.scene_flags)") !=
          std::string::npos);
    // 淡出 update 必须走同一 bit0|bit2 门（45f430）。
    CHECK(source.find("stage_loading_holds_game_fade(fade.kind, "
                      "fade.owner->progress.scene_flags)") != std::string::npos);
}

int main() { return th13test::run_all("stage transition clock"); }
