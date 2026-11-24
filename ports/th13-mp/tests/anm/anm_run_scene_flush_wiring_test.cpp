#include "../harness/test_framework.hpp"
#include "../ui/ui_test_fixtures.hpp"

#include "game/FrameScheduler.hpp"
#include "game/RunScene.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct NullGraphics final : ZunGraphics {
    touhou::graphics::PipelineState& pipeline() override { return pipeline_state; }
    u32 texture(const AnmResource&, u32) override { return 0; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32, const void*, u32) override {}
    void primitives(touhou::graphics::Topology, u32, const void*, u32) override {}
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}

private:
    touhou::graphics::PipelineState pipeline_state;
};

struct RunHost final : RunSceneServices {
    AnmManager& manager;
    ZunGraphics* graphics = nullptr;
    explicit RunHost(AnmManager& manager) : manager(manager) {}
    bool request_pause() override { return true; }
    bool request_game_over() override { return true; }
    bool request_practice_end() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool camera(DrawCamera, bool) override { return true; }
    bool hud_text(const HudTextDraw&) override { return true; }
    bool read_asset(const std::string&, std::vector<u8>&, bool = true) override { return false; }
    ZunGraphics& stage_graphics() override { return *graphics; }
    bool preload_scenery_animation(i32, const std::string&) override { return false; }
};

struct WiringHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    FrameScheduler scheduler;
    RunHost host{manager};
    std::unique_ptr<RunScene> scene;

    WiringHarness() {
        progress.character = 0;
        CHECK(load_bank(manager, 3, synthetic_anm(8, trivial_scripts(176))));
        CHECK(load_bank(manager, 2, synthetic_anm(8, trivial_scripts(46))));
        CHECK(load_bank(manager, 4, synthetic_anm(8, trivial_scripts(4))));
        CHECK(load_bank(manager, 5, synthetic_anm(8, trivial_scripts(1))));
        CHECK(load_bank(manager, 6, synthetic_anm(64, trivial_scripts(35))));
        host.graphics = &graphics;
        StagePresentation::Banks banks{4, 5, 6};
        scene = std::make_unique<RunScene>(progress, player, score, manager, renderer, environment, host, scheduler,
                                           banks, 3, 2, 7, "st01.std");
    }
};

}

TEST(run_scene_wires_slot_flush_and_slots_only_callbacks) {
    WiringHarness harness;
    auto& sim = harness.scene->battle.sim;
    CHECK(sim.flush_anm_slots != nullptr);
    CHECK(sim.enemy_animation != nullptr);
    CHECK(sim.enemy_animation_interrupt != nullptr);
    if (!sim.flush_anm_slots || !sim.enemy_animation || !sim.enemy_animation_interrupt) return;

    StageEnemy enemy;
    enemy.serial = 42;
    enemy.anm_file = 2;
    sim.anm_effects.push_back({0, 0, 12.5f, 34.25f, 0.75f});
    sim.anm_effects.push_back({2, 0, 0, 0, 0, 1, 303, 0, 42});
    const u32 before = harness.manager.registry.ordered_count(false);

    CHECK(sim.flush_anm_slots(&enemy));
    CHECK(enemy.anm_slot_handle[0] != 0);
    CHECK_EQ(harness.manager.registry.ordered_count(false), before + 1);
    CHECK_EQ(sim.anm_effects.size(), 1u);
    if (!sim.anm_effects.empty()) CHECK_EQ(sim.anm_effects[0].kind, 0);

    AnmVm* slot = sim.enemy_animation(enemy, 0);
    CHECK(slot != nullptr);
    CHECK_EQ(sim.anm_effects.size(), 1u);
    if (!sim.anm_effects.empty()) CHECK_EQ(sim.anm_effects[0].kind, 0);
    CHECK_EQ(harness.manager.registry.ordered_count(false), before + 1);

    sim.anm_effects.push_back({2, 1, 0, 0, 0, 1, 303, 1, 42});
    CHECK(sim.enemy_animation_interrupt(enemy, 1, 10));
    CHECK(enemy.anm_slot_handle[1] != 0);
    CHECK_EQ(harness.manager.registry.ordered_count(false), before + 2);
    CHECK_EQ(sim.anm_effects.size(), 1u);
    if (!sim.anm_effects.empty()) CHECK_EQ(sim.anm_effects[0].kind, 0);

    CHECK(harness.scene->presentation->flush_ecl_animations(harness.scene->battle));
    CHECK(sim.anm_effects.empty());
    CHECK_EQ(harness.manager.registry.ordered_count(false), before + 3);
}

int main() { return th13test::run_all("anm run scene flush wiring"); }
