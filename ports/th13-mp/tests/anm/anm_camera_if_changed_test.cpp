#include "anm_test_fixtures.hpp"

#include "game/AnmRenderer.hpp"
#include "game/ScreenViews.hpp"
#include "game/ZunGraphics.hpp"

using namespace th13;

namespace {
struct CountingGraphics final : ZunGraphics {
    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&, u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override { ++matrix_calls; }
    void triangles(u32, const void*, u32) override { ++triangle_calls; }
    void primitives(touhou::graphics::Topology, u32, const void*, u32) override {}
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override { ++viewport_calls; }

    u32 viewport_calls = 0, matrix_calls = 0, triangle_calls = 0;

private:
    touhou::graphics::PipelineState state;
};
}

TEST(identical_fullscreen_camera_is_not_reapplied_but_offset_still_updates) {
    CountingGraphics graphics;
    AnmEnvironment environment;
    AnmRenderer renderer(graphics);
    ScreenViews views(renderer, environment);
    CHECK(views.camera_if_changed(DrawCamera::Fullscreen, false));
    CHECK_EQ(graphics.viewport_calls, 1u);
    CHECK_EQ(graphics.matrix_calls, 2u);
    CHECK(views.camera_if_changed(DrawCamera::Fullscreen, false));
    CHECK_EQ(graphics.viewport_calls, 1u);
    CHECK_EQ(graphics.matrix_calls, 2u);
    environment.screen_shake_offset = {3.f, -2.f};
    CHECK(views.camera_if_changed(DrawCamera::Fullscreen, false));
    CHECK(renderer.offset.x == 3.f);
    CHECK(renderer.offset.y == -2.f);
    CHECK_EQ(graphics.viewport_calls, 1u);
    CHECK_EQ(graphics.matrix_calls, 2u);
    CHECK(views.camera_if_changed(DrawCamera::Playfield, false));
    CHECK_EQ(graphics.viewport_calls, 2u);
    CHECK_EQ(graphics.matrix_calls, 4u);
}

int main() { return th13test::run_all("战斗直绘 pass 相机"); }
