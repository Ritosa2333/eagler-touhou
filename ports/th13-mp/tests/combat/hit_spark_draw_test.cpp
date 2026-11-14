// 2P shot draw decides from the VM alone. It never walks creation_parent,
// so the label-1 impact fade of a spent shot is submitted like
// single-player, and a parent that is not a live VM is never loaded.
#include "../harness/test_framework.hpp"

#include "game/AnmRenderer.hpp"
#include "game/ZunGraphics.hpp"

#include <cstdint>

using namespace th13;

namespace {
struct QuietGraphics final : ZunGraphics {
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

private:
    touhou::graphics::PipelineState state;
};
}  // namespace

TEST(stale_shot_parent_is_not_loaded) {
    QuietGraphics graphics;
    AnmRenderer renderer(graphics);
    AnmVm vm;
    vm.shot_effect_owner = 1;
    vm.creation_parent = reinterpret_cast<AnmVm*>(std::uintptr_t{1});
    vm.visual.flags = 0;
    CHECK_EQ(renderer.draw(vm), -1);
}

TEST(shot_owner_past_the_opacity_table_is_not_indexed) {
    QuietGraphics graphics;
    AnmRenderer renderer(graphics);
    AnmVm vm;
    vm.shot_effect_owner = 255;
    vm.visual.flags = 0;
    CHECK_EQ(renderer.draw(vm), -1);
}

TEST(spent_shot_fade_draws_through_parent) {
    QuietGraphics graphics;
    AnmRenderer renderer(graphics);
    AnmVm parent;
    parent.shot_effect_owner = 1;
    AnmVm vm;
    vm.shot_effect_owner = 1;
    vm.creation_parent = &parent;
    vm.visual.flags = 3;
    vm.visual.color = 0xffc0c0c0u;
    // Pixel snap places the empty quad at -0.5, which the default viewport
    // culls (0); -1 would mean the VM was rejected before the draw.
    CHECK_EQ(renderer.draw(vm), 0);
}

int main() { return th13test::run_all("hit spark draw"); }
