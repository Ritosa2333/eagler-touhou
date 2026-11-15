// ANM 精灵共享批的冲批上限：复刻原生 UP 批池容量，而非移植期任意值 6144。
//
// 原生证据：
//   0x467960 初始化批池：基址 = [DAT_004dc688 + 0xb781f4]，当前指针
//     [0xef81f4]；指针字段 0xef81f4 之前容量 0x380000 字节。
//   0x467a80 每次追加一个 quad：6 顶点 × 28B（0xa8），指针 += 0xa8、
//     quad 计数 [0xb781f0] += 1。
//   0x4679a0 冲批只在 [0xb781f0] != 0 时提交 DrawPrimitiveUP(4,
//     count*2, [0xef81f8], 0x1c)，由材质键/管线态变化触发；池满不检查。
//   容量 0x380000 / 0xa8 = 21845 quads = 131070 顶点。
//
// 端口旧上限 6144 会在原生池远未满时 flush，把同一原生 DrawPrimitiveUP
// 拆成两个 draw：gate-my laser 窗 f5450 op183 原生 12 三角形 vs web
// 10+2（顶点/状态逐位相同，draw_batch_diagnostic aligned=true）。
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
    void triangles(u32 count, const void*, u32) override { ++draws; last_triangles = count; }
    void primitives(touhou::graphics::Topology, u32, const void*, u32) override { ++primitive_calls; }
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}

    u32 draws = 0, primitive_calls = 0, last_triangles = 0;

private:
    touhou::graphics::PipelineState state;
};
}  // namespace

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
    // count=14 → 12 三角形 → 每次追加 36 顶点。171 次 = 6156 顶点：
    // 旧上限 6144 会在第 171 次前 flush（提交 6120 顶点、只留 36）。
    for (u32 i = 0; i < 171; ++i)
        CHECK_EQ(renderer.draw_screen_strip(vm, strip.data(), 14), 0);
    CHECK_EQ(renderer.pending().size(), 6156u);
    CHECK_EQ(graphics.draws, 0u);
    // 继续到原生池容量：第 3641 次调用前 131040+36 > 131070 → flush 一次，
    // 提交 131040/3 = 43680 三角形（原生单批上限 21845 quads × 2 = 43690）。
    for (u32 i = 0; i < 3641 - 171; ++i)
        CHECK_EQ(renderer.draw_screen_strip(vm, strip.data(), 14), 0);
    CHECK_EQ(graphics.draws, 1u);
    CHECK_EQ(graphics.last_triangles, 43680u);
    CHECK_EQ(renderer.pending().size(), 36u);
}

int main() { return th13test::run_all("ANM 精灵批容量"); }
