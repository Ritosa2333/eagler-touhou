#define TH13_APPLICATION_PLATFORMS_ONLY
#include "../harness/test_framework.hpp"

#include "sdl/StageTransitionClock.hpp"

#include <fstream>
#include <iterator>
#include <string>

using namespace th13;
using namespace th13::sdl;

namespace {
std::string production_application_source() {
    const char* path = "eagler-touhou/ports/th13/web/cpp/sdl/Application.cpp";
    std::ifstream file(path);
    return std::string(std::istreambuf_iterator<char>(file), {});
}
}

TEST(loader_completion_clears_only_42b990_bit2) {
    CHECK_EQ(stage_loader_completed_scene_flags(kStageLoaderSceneFlag), 0u);
    CHECK_EQ(stage_loader_completed_scene_flags(0x4004u), 0x4000u);
    CHECK_EQ(stage_loader_completed_scene_flags(0x4014u), 0x4010u);
    CHECK_EQ(stage_loader_completed_scene_flags(0u), 0u);
}

TEST(kind5_exit_fade_freezes_through_loader_gate_then_ticks) {
    const u32 loader_gate = kStageLoaderSceneFlag;
    i32 time = 0;
    for (i32 frame = 0; frame < 120; ++frame)
        if (!stage_loading_holds_game_fade(5, loader_gate)) ++time;
    CHECK_EQ(time, 0);

    CHECK(stage_loading_holds_game_fade(5, 4u));
    CHECK(stage_loading_holds_game_fade(5, 1u));
    CHECK(!stage_loading_holds_game_fade(5, 0u));
    CHECK(!stage_loading_holds_game_fade(0, 4u));
    CHECK(!stage_loading_holds_game_fade(0, 0u));

    const u32 loader_done = stage_loader_completed_scene_flags(loader_gate);
    for (i32 frame = 0; frame < 30; ++frame)
        if (!stage_loading_holds_game_fade(5, loader_done)) ++time;
    CHECK_EQ(time, 30);
    CHECK_EQ(time >= 30, true);
    CHECK_EQ(time >= 32, false);
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
    i32 score = 7858841, max_point = 1095980, max_point_cap = 25000000, graze = 4321;
    run_entry_reset_points(true, 1, score, max_point, max_point_cap, graze);
    CHECK_EQ(score, 7858841);
    CHECK_EQ(max_point, 1095980);
    CHECK_EQ(max_point_cap, 25000000);
    CHECK_EQ(graze, 4321);
    run_entry_reset_points(true, 99, score, max_point, max_point_cap, graze);
    CHECK_EQ(score, 7858841);
    CHECK_EQ(max_point, 1095980);
    CHECK_EQ(max_point_cap, 25000000);
    CHECK_EQ(graze, 4321);
}

TEST(production_begin_run_passes_continuing_not_inverted) {
    const std::string source = production_application_source();
    CHECK(!source.empty());
    CHECK(source.find("run_entry_reset_points(continuing, progress.difficulty") != std::string::npos);
    CHECK(source.find("run_entry_reset_points(!continuing") == std::string::npos);
}

TEST(production_step_wires_loader_completion_and_fade_gate) {
    const std::string source = production_application_source();
    CHECK(!source.empty());
    CHECK(source.find("if (!run->start_transition()) return false;") != std::string::npos);
    CHECK(source.find("stage_loader_completed_scene_flags(progress.scene_flags)") != std::string::npos);
    CHECK(source.find("stage_loading_holds_game_fade(fade.kind, "
                      "fade.owner->progress.scene_flags)") != std::string::npos);
}

int main() { return th13test::run_all("stage transition clock"); }
