#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/StagePresentation.hpp"
#include <fstream>
using namespace th13;
using namespace th13::test;
namespace {
struct Graphics final : ZunGraphics {
    touhou::graphics::PipelineState state;
    std::vector<touhou::graphics::PipelineState> draws;
    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&, u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32 n, const void*, u32) override { if (n) draws.push_back(state); }
    void primitives(touhou::graphics::Topology, u32 n, const void*, u32) override { if (n) draws.push_back(state); }
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}
};
struct Rig {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager animations{random, environment};
    Graphics graphics;
    AnmRenderer renderer{graphics};
    StagePresentation presentation{animations, renderer, {4, 5, 6}, 0};
    StageSim sim;
    bool init(i32 initial_blend = 0, bool later_change = false) {
        auto scripts = trivial_scripts(36);
        scripts[35] = {sprite(0), {303, 0, {initial_blend}}, wait(), label(2)};
        if (later_change) scripts[35].push_back({303, 2, {2}});
        scripts[35].push_back({3, i16(later_change ? 2 : 0), {}});
        scripts[35].push_back(stop());
        auto data = synthetic_anm(1, scripts);
        u32 sprite_at; std::memcpy(&sprite_at, data.data() + 64, 4);
        const float size = 16;
        std::memcpy(data.data() + sprite_at + 12, &size, 4);
        std::memcpy(data.data() + sprite_at + 16, &size, 4);
        if (!load_bank(animations, 5, data)) return false;
        renderer.set_viewport({0, 0, 640, 480});
        sim.interactive = true;
        sim.bullet_animation = [&](StageBullet& b, bool birth) { return presentation.tick_bullet(b, birth); };
        return true;
    }
    void emit(i32 arg) {
        StageEt e; e.reset(); e.speed1 = 0;
        e.ex[0].op = 0x100000; e.ex[0].args[4] = float(arg);
        const float pos[] = {0, 200, 0}; sim.emit_et(e, 0, 0, pos, 0);
    }
    bool draw() { graphics.draws.clear(); const bool ok = presentation.draw(3); renderer.flush(); return ok; }
};
TEST(etex_blend_changes_actual_submission_without_changing_collision_flags) {
    using namespace touhou::graphics;
    for (i32 arg : {0, 1, -1}) {
        Rig r; CHECK(r.init(2)); r.emit(arg);
        CHECK(r.sim.error().empty()); CHECK_EQ(r.sim.bullets.size(), size_t(1));
        CHECK_EQ(r.sim.bullets[0].flags, 0x12u);
        CHECK_EQ(r.sim.bullets[0].animation_blend, -1);
        CHECK(r.draw()); CHECK_EQ(r.graphics.draws.size(), size_t(1));
        if (r.graphics.draws.empty()) continue;
        CHECK(r.graphics.draws[0].sourceBlend == BlendFactor::SourceAlpha);
        CHECK(r.graphics.draws[0].destinationBlend == (arg ? BlendFactor::One : BlendFactor::InverseSourceAlpha));
        CHECK(r.graphics.draws[0].blendEquation == BlendEquation::Add);
    }
}
TEST(etex_blend_is_consumed_once_and_later_anm_instruction_can_replace_it) {
    Rig r; CHECK(r.init(0, true)); r.emit(1);
    CHECK(r.draw()); CHECK(!r.graphics.draws.empty());
    for (int i = 0; i < 3; ++i) r.sim.advance_bullets();
    CHECK(r.draw()); CHECK_EQ(r.graphics.draws.size(), size_t(1));
    if (!r.graphics.draws.empty())
        CHECK(r.graphics.draws[0].blendEquation == touhou::graphics::BlendEquation::ReverseSubtract);
    CHECK_EQ(r.sim.bullets[0].flags, 0x12u);
}
TEST(etex_runtime_blend_write_preserves_scaled_collision_and_uses_last_command) {
    Rig r; CHECK(r.init()); r.emit(0);
    auto& b = r.sim.bullets[0]; b.flags = 0x52;
    b.ex_cursor = 0; b.ex[0].args[4] = 1;
    b.ex[1].op = 0x100000; b.ex[1].args[4] = 0;
    r.sim.advance_bullets(); CHECK_EQ(r.sim.bullets[0].flags, 0x52u);
    CHECK(r.draw()); CHECK_EQ(r.graphics.draws.size(), size_t(1));
    if (!r.graphics.draws.empty())
        CHECK(r.graphics.draws[0].destinationBlend == touhou::graphics::BlendFactor::InverseSourceAlpha);
}
TEST(retail_bullet_scripts_submit_the_requested_blend_for_stage5_types) {
    for (u32 type : {7u, 22u, 30u}) {
        Rig r;
        std::ifstream file("artifacts/reference/raw/bullet.anm", std::ios::binary);
        const std::vector<u8> data{std::istreambuf_iterator<char>(file), {}};
        CHECK(!data.empty());
        CHECK(load_bank(r.animations, 5, data));
        r.renderer.set_viewport({0, 0, 640, 480});
        r.sim.interactive = true;
        r.sim.bullet_animation = [&](StageBullet& b, bool birth) { return r.presentation.tick_bullet(b, birth); };
        StageEt e; e.reset(); e.sprite = type; e.speed1 = 0;
        e.ex[0].op = 0x100000; e.ex[0].args[4] = 1;
        const float pos[] = {0, 200, 0}; r.sim.emit_et(e, 0, 0, pos, 0);
        CHECK(r.sim.error().empty()); CHECK_EQ(r.sim.bullets.size(), size_t(1));
        CHECK(r.draw()); CHECK_EQ(r.graphics.draws.size(), size_t(1));
        if (!r.graphics.draws.empty()) {
            CHECK(r.graphics.draws[0].sourceBlend == touhou::graphics::BlendFactor::SourceAlpha);
            CHECK(r.graphics.draws[0].destinationBlend == touhou::graphics::BlendFactor::One);
            CHECK(r.graphics.draws[0].blendEquation == touhou::graphics::BlendEquation::Add);
        }
    }
}
}
int main() { return th13test::run_all("enemy bullet blend"); }
