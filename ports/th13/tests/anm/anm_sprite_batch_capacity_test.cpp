#include "anm_test_fixtures.hpp"

#include "game/AnmRenderer.hpp"
#include "game/ZunGraphics.hpp"

#include <array>

using namespace th13;

namespace {
struct CountingGraphics final : ZunGraphics {
    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&, u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32 count, const void*, u32) override {
        ++draws;
        last_triangles = count;
    }
    void primitives(touhou::graphics::Topology, u32, const void*, u32) override { ++primitive_calls; }
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}

    u32 draws = 0, primitive_calls = 0, last_triangles = 0;

private:
    touhou::graphics::PipelineState state;
};
}

TEST(sprite_batch_flushes_at_the_native_pool_capacity_not_6144) {
    auto bytes = th13test::build_simple_anm({});
    AnmResource resource;
    CHECK(resource.open(bytes.data(), u32(bytes.size())));
    CHECK(!resource.sprites.empty());
    AnmVm vm;
    vm.resource = &resource;
    vm.sprite = &resource.sprites[0];
    vm.visual.flags = 3;
    vm.visual.sprite = 0;
    CountingGraphics graphics;
    AnmRenderer renderer(graphics);
    std::array<AnmGeometryVertex, 14> strip{};
    for (u32 i = 0; i < 171; ++i)
        CHECK_EQ(renderer.draw_screen_strip(vm, strip.data(), 14), 0);
    CHECK_EQ(renderer.pending().size(), 6156u);
    CHECK_EQ(graphics.draws, 0u);
    for (u32 i = 0; i < 3641 - 171; ++i)
        CHECK_EQ(renderer.draw_screen_strip(vm, strip.data(), 14), 0);
    CHECK_EQ(graphics.draws, 1u);
    CHECK_EQ(graphics.last_triangles, 43680u);
    CHECK_EQ(renderer.pending().size(), 36u);
}

int main() { return th13test::run_all("ANM 精灵批容量"); }
