#include "../ecl/ecl_test_fixtures.hpp"
#include "game/AnmCoordinates.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"
#include "ui_test_fixtures.hpp"

#include <cmath>
#include <memory>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct Graphics : ZunGraphics {
    touhou::graphics::PipelineState state;
    std::vector<AnmGeometryVertex> submitted;
    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&, u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32 count, const void* data, u32 stride) override {
        if (stride == sizeof(AnmGeometryVertex) && data) {
            auto* v = static_cast<const AnmGeometryVertex*>(data);
            submitted.insert(submitted.end(), v, v + count * 3);
        }
    }
    void primitives(touhou::graphics::Topology, u32, const void*, u32) override {}
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}
};

std::vector<std::vector<AnmIns>> rotation_scripts() {
    std::vector<std::vector<AnmIns>> scripts(8);
    scripts[0] = {AnmIns{424, 0, {1}}, wait(), stop()};
    scripts[1] = {AnmIns{401, 0, {0, 0, 0x3f000000}}, wait(), stop()};
    scripts[2] = {AnmIns{424, 0, {1}}, wait(), stop()};
    for (u32 i = 3; i < 8; i++)
        scripts[i] = {wait(), stop()};
    return scripts;
}

struct Rig {
    GameRandom random;
    AnmEnvironment env;
    AnmManager animations{random, env};
    Graphics graphics;
    AnmRenderer renderer{graphics};
    StageBattle battle;
    StagePresentation presentation{animations, renderer, {4, 5, 6}, 0};
    StageEnemy* e = nullptr;
    bool init(bool retail = false) {
        std::vector<u8> bytes;
        if (retail) {
            if (!th13test::TestFileReader{}.read("artifacts/reference/raw/st07enm.anm", bytes)) return false;
        } else {
            bytes = synthetic_anm(1, rotation_scripts());
        }
        if (!load_bank(animations, 4, bytes)) return false;
        if (!load_bank(animations, 5, synthetic_anm(1, trivial_scripts(1)))) return false;
        if (!load_bank(animations, 6, synthetic_anm(1, trivial_scripts(1)))) return false;
        auto enemy = std::make_unique<StageEnemy>();
        e = enemy.get();
        e->serial = 1;
        e->anm_file = 2;
        e->threads = std::make_unique<EclThreads>(battle.sim.program);
        battle.sim.enemies.push_back(std::move(enemy));
        battle.sim.commands.sim = &battle.sim;
        battle.sim.current = e;
        battle.sim.ecl_slot_anm = true;
        presentation.enemies_from_ecl = true;
        renderer.set_viewport({0, 0, 640, 480});
        env.screen_offsets = {320, 16, 320, 16};
        return true;
    }
    bool command(u16 op, std::initializer_list<u32> args) {
        EclContext c;
        std::vector<u8> code;
        th13test::emit(code, 0, op, 0, 255, 0, args);
        c.instruction = reinterpret_cast<const EclInstruction*>(code.data());
        if (battle.sim.commands.execute(c, op) != 0) return false;
        return presentation.flush_ecl_animations(battle);
    }
    AnmVm* slot_vm(u32 slot) { return animations.registry.find(e->anm_slot_handle[slot]); }
};

}

TEST(op424_rotation_follows_motion_delta_quadrants_and_stop) {
    Rig r;
    CHECK(r.init());
    CHECK(r.command(306, {0, 0}));
    CHECK(r.e->flags & 0x200000u);
    CHECK(r.animations.update());
    auto* vm = r.slot_vm(0);
    CHECK(vm);
    if (!vm) return;
    CHECK(vm->visual.render_flags & 0x40u);

    r.e->position[0] = 10.0f;
    r.e->position[1] = 5.0f;

    const float quarter = 1.5707963705062866f;
    struct Case {
        float dx, dy, want;
    };
    const Case cases[] = {
        {1, 0, 0.0f},        {0, 1, quarter},       {-1, 0, 3.14159274f},   {0, -1, -quarter}, {1, 1, 0.785398185f},
        {-1, 1, 2.3561945f}, {-1, -1, -2.3561945f}, {1, -1, -0.785398185f}, {0, 0, 0.0f},      {0, 1, quarter},
    };
    for (const auto& c : cases) {
        r.e->motion_delta[0] = c.dx;
        r.e->motion_delta[1] = c.dy;
        CHECK(r.presentation.sync(r.battle, 0));
        vm = r.slot_vm(0);
        CHECK(vm);
        if (!vm) return;
        CHECK_EQ(vm->variables.rotation.z, c.want);
        CHECK(vm->visual.flags & 4u);
        CHECK(r.animations.update());
    }
}

TEST(no_op424_keeps_script_rotation) {
    Rig r;
    CHECK(r.init());
    CHECK(r.command(306, {0, 1}));
    CHECK(r.animations.update());
    auto* vm = r.slot_vm(0);
    CHECK(vm);
    if (!vm) return;
    CHECK(!(vm->visual.render_flags & 0x40u));
    const float script_rotation = vm->variables.rotation.z;
    CHECK_EQ(script_rotation, 0.5f);
    vm->visual.flags &= ~4u;

    r.e->motion_delta[0] = 3.0f;
    r.e->motion_delta[1] = 4.0f;
    CHECK(r.presentation.sync(r.battle, 0));
    vm = r.slot_vm(0);
    CHECK(vm);
    if (!vm) return;
    CHECK_EQ(vm->variables.rotation.z, script_rotation);
    CHECK(!(vm->visual.flags & 4u));
    CHECK(r.animations.update());
    CHECK_EQ(r.slot_vm(0)->variables.rotation.z, script_rotation);
}

TEST(flag_08000000_direct_follow_skips_rotation_and_slot_offset) {
    Rig r;
    CHECK(r.init());
    CHECK(r.command(306, {0, 0}));
    CHECK(r.animations.update());
    auto* vm = r.slot_vm(0);
    CHECK(vm);
    if (!vm) return;
    CHECK(vm->visual.render_flags & 0x40u);

    vm->visual.flags &= ~4u;
    r.e->flags |= 0x8000000u;
    r.e->anm_slot_offset[0] = {7.0f, 8.0f, 9.0f};
    r.e->position[0] = 10.0f;
    r.e->position[1] = 20.0f;
    r.e->position[2] = 30.0f;
    r.e->motion_delta[0] = 0.0f;
    r.e->motion_delta[1] = 1.0f;
    CHECK(r.presentation.sync(r.battle, 0));
    vm = r.slot_vm(0);
    CHECK(vm);
    if (!vm) return;
    CHECK_EQ(vm->variables.rotation.z, 0.0f);
    CHECK(!(vm->visual.flags & 4u));
    CHECK_EQ(vm->visual.translation.x, 10.0f);
    CHECK_EQ(vm->visual.translation.y, 20.0f);
    CHECK_EQ(vm->visual.translation.z, 30.0f);
    CHECK(r.animations.update());
}

TEST(non_slot0_slot_gets_same_rotation_rule) {
    Rig r;
    CHECK(r.init());
    CHECK(r.command(306, {0, 3}));
    CHECK(r.command(303, {1, 2}));
    CHECK(r.animations.update());
    auto* vm1 = r.slot_vm(1);
    CHECK(vm1);
    if (!vm1) return;
    CHECK(vm1->visual.render_flags & 0x40u);

    r.slot_vm(0)->visual.flags &= ~4u;
    r.e->motion_delta[0] = -1.0f;
    r.e->motion_delta[1] = 0.0f;
    CHECK(r.presentation.sync(r.battle, 0));
    vm1 = r.slot_vm(1);
    auto* vm0 = r.slot_vm(0);
    CHECK(vm1);
    CHECK(vm0);
    if (!vm1 || !vm0) return;
    CHECK_EQ(vm1->variables.rotation.z, 3.14159274f);
    CHECK(vm1->visual.flags & 4u);
    CHECK_EQ(vm0->variables.rotation.z, 0.0f);
    CHECK(!(vm0->visual.render_flags & 0x40u));
    CHECK(!(vm0->visual.flags & 4u));
    CHECK(r.animations.update());
}

TEST(real_st07enm_script10_auto_rotation) {
    Rig r;
    CHECK(r.init(true));
    CHECK(r.command(306, {0, 10}));
    CHECK(r.animations.update());
    auto* vm = r.slot_vm(0);
    CHECK(vm);
    if (!vm) return;
    CHECK_EQ(vm->source_script, 10);
    CHECK(vm->visual.render_flags & 0x40u);

    r.e->motion_delta[0] = 1.0f;
    r.e->motion_delta[1] = 0.0f;
    CHECK(r.presentation.sync(r.battle, 0));
    vm = r.slot_vm(0);
    CHECK(vm);
    if (!vm) return;
    CHECK_EQ(vm->variables.rotation.z, 0.0f);
    CHECK(vm->visual.flags & 4u);
    CHECK(r.animations.update());

    r.e->motion_delta[0] = 0.0f;
    r.e->motion_delta[1] = 1.0f;
    CHECK(r.presentation.sync(r.battle, 0));
    CHECK_EQ(r.slot_vm(0)->variables.rotation.z, 1.5707963705062866f);
    CHECK(r.animations.update());

    const u32 layer = r.slot_vm(0)->visual.layer;
    r.graphics.submitted.clear();
    CHECK(r.renderer.draw_layer(r.animations.registry.layer(layer)));
    r.renderer.flush();
    CHECK_EQ(r.graphics.submitted.size(), size_t(6));
    if (r.graphics.submitted.size() != 6) return;
    Vec3 expected[4];
    CHECK(anm_quad_positions(*r.slot_vm(0), expected));
    const u32 order[] = {0, 1, 2, 1, 2, 3};
    for (u32 i = 0; i < 6; ++i) {
        CHECK_EQ(r.graphics.submitted[i].position.x, expected[order[i]].x);
        CHECK_EQ(r.graphics.submitted[i].position.y, expected[order[i]].y);
    }
}

int main() { return th13test::run_all("enemy auto rotation"); }
