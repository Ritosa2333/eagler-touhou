#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"
#include "ui_test_fixtures.hpp"

#include <memory>

using namespace th13;
using namespace th13::test;

namespace {

struct Graphics : ZunGraphics {
    touhou::graphics::PipelineState state;
    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&, u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32, const void*, u32) override {}
    void primitives(touhou::graphics::Topology, u32, const void*, u32) override {}
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}
};

}

TEST(mirror_velocity_snapshot_drives_rebind_and_306_transition_is_suppressed) {
    GameRandom random;
    AnmEnvironment env;
    AnmManager animations(random, env);
    Graphics graphics;
    AnmRenderer renderer(graphics);
    StageBattle battle;
    StagePresentation presentation(animations, renderer, {4, 5, 6}, 0);
    auto bytes = synthetic_anm(1, trivial_scripts(8));
    CHECK(load_bank(animations, 4, bytes));
    CHECK(load_bank(animations, 5, synthetic_anm(1, trivial_scripts(1))));
    CHECK(load_bank(animations, 6, synthetic_anm(1, trivial_scripts(1))));
    auto enemy = std::make_unique<StageEnemy>();
    auto* e = enemy.get();
    e->serial = 1;
    e->anm_file = 2;
    e->mirror_script_base = 2;
    battle.sim.enemies.push_back(std::move(enemy));
    battle.sim.commands.sim = &battle.sim;
    battle.sim.current = e;
    battle.sim.ecl_slot_anm = true;
    presentation.enemies_from_ecl = true;
    e->threads = std::make_unique<EclThreads>(battle.sim.program);
    auto command = [&](u16 op, std::initializer_list<u32> args) {
        EclContext c;
        std::vector<u8> code;
        th13test::emit(code, 0, op, 0, 255, 0, args);
        c.instruction = reinterpret_cast<const EclInstruction*>(code.data());
        CHECK_EQ(battle.sim.commands.execute(c, op), 0);
        CHECK(presentation.flush_ecl_animations(battle));
    };

    command(306, {0, 0});
    CHECK(e->flags & 0x200000u);
    CHECK(!e->mirror_ready);
    CHECK_EQ(e->mirror_dir, 0);
    const u32 root = e->anm_slot_handle[0];
    CHECK(root != 0);

    e->position[0] += 5.0f;
    e->mirror_velocity_x = 5.0f;
    CHECK(presentation.sync(battle, 0));
    CHECK(animations.update());
    CHECK_EQ(e->anm_slot_handle[0], root);
    CHECK_EQ(e->mirror_dir, 0);

    e->mirror_ready = true;
    e->mirror_velocity_x = -1.0f;
    CHECK(presentation.sync(battle, 0));
    CHECK(animations.update());
    CHECK(e->anm_slot_handle[0] != root);
    CHECK_EQ(e->mirror_dir, -1);
    {
        auto* vm = animations.registry.find(e->anm_slot_handle[0]);
        CHECK(vm);
        if (vm) {
            CHECK_EQ(vm->source_script, e->anm_base_script + 1);
            CHECK_EQ(vm->visual.layer, e->mirror_script_base + 7);
        }
    }

    e->mirror_ready = true;
    e->mirror_velocity_x = 1.0f;
    CHECK(presentation.sync(battle, 0));
    CHECK(animations.update());
    CHECK_EQ(e->mirror_dir, 1);
    {
        auto* vm = animations.registry.find(e->anm_slot_handle[0]);
        CHECK(vm);
        if (vm) CHECK_EQ(vm->source_script, e->anm_base_script + 2);
    }

    e->mirror_ready = true;
    e->mirror_velocity_x = 0.02f;
    CHECK(presentation.sync(battle, 0));
    CHECK(animations.update());
    CHECK_EQ(e->mirror_dir, 0);
    {
        auto* vm = animations.registry.find(e->anm_slot_handle[0]);
        CHECK(vm);
        if (vm) CHECK_EQ(vm->source_script, e->anm_base_script + 4);
    }
}

int main() { return th13test::run_all("enemy mirror velocity"); }
