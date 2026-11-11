// 战斗直绘 pass 入口的重复同值 Fullscreen 相机不得重设视口/矩阵（不冲 UP 批）。
//
// 原生证据：
//   0x4679a0 只在批非空时提交 DrawPrimitiveUP，由材质键（纹理+管线态）变化
//   触发；相机/视口切换（0x43b900 SetViewport/SetTransform）不冲批。
//   0x467a80 每 quad 追加 0xa8 字节到共享批。
//   故同一材质键的 sprite 跨 pass 合批：gate-my laser 窗 f5450 op183 原生
//   12 三角形单 draw，web 旧实现每 pass 入口 host.camera 无条件 flush →
//   10+2（19551dd1e 记录同一现象；efc1422f3 回退全局同值跳过以保显示）。
//
// 本测试锁定 RunScene 专用入口 camera_if_changed 的语义：
//   同值 → 不调用 set_viewport/set_matrix（不冲批），offset 仍更新；
//   异值 → 正常设置。
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
}  // namespace

TEST(identical_fullscreen_camera_is_not_reapplied_but_offset_still_updates) {
    CountingGraphics graphics;
    AnmEnvironment environment;
    AnmRenderer renderer(graphics);
    ScreenViews views(renderer, environment);
    CHECK(views.camera_if_changed(DrawCamera::Fullscreen, false));
    CHECK_EQ(graphics.viewport_calls, 1u);
    CHECK_EQ(graphics.matrix_calls, 2u);  // view + projection
    // 同值重复（自机 pass 入口）：不重设视口/矩阵 → 不冲批。
    CHECK(views.camera_if_changed(DrawCamera::Fullscreen, false));
    CHECK_EQ(graphics.viewport_calls, 1u);
    CHECK_EQ(graphics.matrix_calls, 2u);
    // offset 仍更新（screen shake 在顶点生成时烘焙，不影响批边界）。
    environment.screen_shake_offset = {3.f, -2.f};
    CHECK(views.camera_if_changed(DrawCamera::Fullscreen, false));
    CHECK(renderer.offset.x == 3.f);
    CHECK(renderer.offset.y == -2.f);
    CHECK_EQ(graphics.viewport_calls, 1u);
    CHECK_EQ(graphics.matrix_calls, 2u);
    // 异值相机（Playfield 视口 {116,2,408,476}）：正常设置。
    CHECK(views.camera_if_changed(DrawCamera::Playfield, false));
    CHECK_EQ(graphics.viewport_calls, 2u);
    CHECK_EQ(graphics.matrix_calls, 4u);
}

int main() { return th13test::run_all("战斗直绘 pass 相机"); }
