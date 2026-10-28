#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/AnmRenderer.hpp"
#include <cmath>

using namespace th13;
using namespace touhou::graphics;

namespace {
struct Graphics final : ZunGraphics {
    struct Vertex { Vec3 position; float reciprocal_w; u32 color; };
    PipelineState state;
    Topology expected = Topology::Fan;
    u32 primitives_drawn = 0;
    bool drawn_with_fog = false;
    VertexLayout layout = VertexLayout::ScreenColor;
    std::vector<Vertex> vertices;
    PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&, u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(VertexLayout value) override { layout = value; }
    void set_matrix(MatrixKind, const Matrix4&) override {}
    void triangles(u32, const void*, u32) override {}
    void primitives(Topology topology, u32 count, const void* data, u32 stride) override {
        CHECK_EQ(int(topology), int(expected));
        primitives_drawn = count;
        drawn_with_fog = state.fog;
        if (layout == VertexLayout::WorldUv) {
            CHECK_EQ(stride, 20u);
            return;
        }
        CHECK_EQ(stride, u32(sizeof(Vertex)));
        if (topology != expected) return;
        const auto* source = static_cast<const Vertex*>(data);
        vertices.assign(source, source + vertex_count(topology, count));
    }
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}
};

void circle(u32 mode, Topology topology) {
    Graphics graphics;
    graphics.expected = topology;
    AnmRenderer renderer{graphics};
    AnmVm vm;
    vm.visual.flags = 3u | mode << 25;
    vm.visual.render_flags = 0;
    vm.visual.translation = {100, 120, 0};
    vm.visual.sprite_size = {20, 0};
    vm.visual.scale = {2, 1};
    vm.visual.color = 0xff123456u;
    vm.visual.secondary_color = 0x80112233u;
    vm.variables.integers[0] = 8;
    CHECK_EQ(renderer.draw(vm), 0);
    CHECK_EQ(graphics.primitives_drawn, 8u);
    CHECK_EQ(graphics.vertices.size(), mode == 16 ? 10u : 9u);
    if (graphics.vertices.empty()) return;
    const size_t edge = mode == 16 ? 1 : 0;
    if (mode == 16) {
        CHECK_EQ(graphics.vertices[0].position.x, 100.f);
        CHECK_EQ(graphics.vertices[0].position.y, 120.f);
        CHECK_EQ(graphics.vertices[0].color, vm.visual.color);
    }
    for (size_t i = edge; i < graphics.vertices.size(); ++i) {
        const auto& vertex = graphics.vertices[i];
        const float dx = vertex.position.x - 100, dy = vertex.position.y - 120;
        CHECK_NEAR(std::sqrt(dx * dx + dy * dy), 40.f, 0.0001f);
        CHECK_EQ(vertex.color, mode == 16 ? vm.visual.secondary_color : vm.visual.color);
    }
    CHECK_NEAR(graphics.vertices[edge].position.x, graphics.vertices.back().position.x, 0.0001f);
    CHECK_NEAR(graphics.vertices[edge].position.y, graphics.vertices.back().position.y, 0.0001f);
}

TEST(anm604_submits_original_filled_fan) { circle(16, Topology::Fan); }
TEST(anm605_submits_original_outline_line_strip) { circle(17, Topology::LineStrip); }

TEST(anm_mode18_uses_world_quad_with_fog) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    CHECK(th13::test::load_bank(manager, 1, th13::test::synthetic_anm(1, {{
        th13::test::sprite(0), th13::test::wait(), th13::test::stop(),
    }})));
    const auto handle = manager.create(1, 0);
    auto* vm = manager.registry.find(handle);
    CHECK(vm);
    if (!vm) return;
    vm->visual.flags = (vm->visual.flags & ~0x3e000000u) | (18u << 25) | 3u;
    vm->visual.color = 0xffffffffu;
    Graphics graphics;
    graphics.expected = Topology::Strip;
    AnmRenderer renderer{graphics};
    CHECK_EQ(renderer.draw(*vm), 0);
    CHECK_EQ(int(graphics.layout), int(VertexLayout::WorldUv));
    CHECK_EQ(graphics.primitives_drawn, 2u);
    CHECK(graphics.drawn_with_fog);
    CHECK(!graphics.state.fog);
}

TEST(anm606_rectangle_preserves_primary_to_secondary_gradient) {
    Graphics graphics;
    graphics.expected = Topology::Strip;
    AnmRenderer renderer{graphics};
    AnmVm vm;
    vm.visual.flags = 3u | 19u << 25;
    vm.visual.render_flags = 0;
    vm.visual.sprite_size = {32, 16};
    vm.visual.color = 0xff123456u;
    vm.visual.secondary_color = 0x80112233u;
    CHECK_EQ(renderer.draw(vm), 0);
    CHECK_EQ(graphics.vertices.size(), 4u);
    for (size_t i = 0; i < graphics.vertices.size(); ++i)
        CHECK_EQ(graphics.vertices[i].color,
                 (i & 1) ? vm.visual.secondary_color : vm.visual.color);
}
}

int main() { return th13test::run_all("ANM shape geometry"); }
