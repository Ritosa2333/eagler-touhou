// draw 代表窗相机契约（代理 H）：
//  A. StageScenery::initialize 按原版 0x406120 预置 STD 相机块
//     （position (0,0,-600)/direction (0,300,600)/up (0,1,0)/offset 零），
//     使 st01.std off=100 的 op2 (-80,0,900) 首帧 delta.z = 1500；
//  B. AnmEnvironment 相机块按 0x43bbc0 启动值 (0,0,1000) 播种——ANM 变量
//     10016-10018 在场景 ECL 构造（ins_308 模板拷贝+立即 tick）期间即读取；
//  C. world_quad（draw_mode 8/18）在 scenery 趟之外沿用
//     renderer.scenery_camera/scenery_viewport（与 wind 分支同法）。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/AnmEnvironment.hpp"
#include "game/AnmRenderer.hpp"
#include "game/StageScenery.hpp"

#include <cstring>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

// 合成 st01.std（与 run_scene_test.cpp 同布局）：1 对象/1 图元（脚本 0）+
// 1 实例 + t=0 ins_2 位置设置脚本。仅用于驱动 StageResource::open +
// StageScenery::initialize 的相机预置路径。
std::vector<u8> synthetic_std() {
    std::vector<u8> out(0x110, 0);
    auto put16 = [&](u32 at, u16 v) { std::memcpy(out.data() + at, &v, 2); };
    auto put32 = [&](u32 at, u32 v) { std::memcpy(out.data() + at, &v, 4); };
    auto putf = [&](u32 at, float v) { std::memcpy(out.data() + at, &v, 4); };
    put16(0, 1);     // 对象数
    put16(2, 1);     // 图元数
    put32(4, 0xd0);  // 实例区
    put32(8, 0xe4);  // 脚本区
    const char* name = "scenery.anm";
    std::memcpy(out.data() + 0x10, name, std::strlen(name));
    put32(0x90, 0x94);  // 对象目录
    put32(0x94, 0x00010000);
    putf(0x98, 0.f); putf(0x9c, 0.f); putf(0xa0, 0.f);
    putf(0xa4, 64.f); putf(0xa8, 64.f); putf(0xac, 64.f);
    put16(0xb0, 0);
    put16(0xb2, 28);
    put16(0xb4, 0);
    put16(0xb6, 0);
    putf(0xb8, 0.f); putf(0xbc, 0.f); putf(0xc0, 0.f);
    putf(0xc4, 64.f); putf(0xc8, 64.f);
    put32(0xcc, 0x0004ffff);
    put32(0xd0, 0);
    putf(0xd4, 0.f); putf(0xd8, 0.f); putf(0xdc, 0.f);
    put32(0xe0, 0xffffffff);
    put32(0xe4, 0); put16(0xe8, 2); put16(0xea, 20);
    putf(0xec, 10.f); putf(0xf0, 20.f); putf(0xf4, 30.f);
    put32(0xf8, 0); put16(0xfc, 0); put16(0xfe, 8);
    for (u32 at = 0x100; at < 0x110; at += 4) put32(at, 0xffffffff);
    return out;
}

// 记录提交时生效的 View 矩阵/视口，用于断言 world_quad 的相机继承与恢复。
struct RecordingGraphics final : ZunGraphics {
    touhou::graphics::PipelineState state;
    Matrix4 view;
    GraphicsViewport viewport;
    u32 batches = 0;
    float draw_view_x = 0;
    float draw_viewport_width = 0;
    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&, u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind kind, const Matrix4& matrix) override {
        if (kind == touhou::graphics::MatrixKind::View) view = matrix;
    }
    void triangles(u32, const void*, u32) override {}
    // world_quad 走 submit → graphics.primitives（静态 VB 路径）。
    void primitives(touhou::graphics::Topology, u32, const void*, u32) override {
        ++batches;
        draw_view_x = view.m[12];
        draw_viewport_width = float(viewport.width);
    }
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport& value) override { viewport = value; }
};

// 建一个 draw_mode 8 的主池 VM（synthetic bank 的脚本 0 = sprite0+park）。
AnmVm* make_world_quad(AnmManager& manager, AnmEnvironment& environment) {
    if (!load_bank(manager, 9, synthetic_anm(8, {{sprite(0), wait(), stop()}})))
        return nullptr;
    const u32 handle = manager.create(9, 0);
    AnmVm* vm = manager.registry.find(handle);
    if (!vm) return nullptr;
    vm->visual.flags = (vm->visual.flags & ~(31u << 25)) | (8u << 25) | 3u;
    vm->visual.render_flags &= ~0x20u;
    vm->visual.color = 0xffffffffu;
    vm->visual.translation = {5.f, 6.f, 7.f};
    vm->environment = &environment;
    return vm;
}

}  // namespace

// A：0x406120 直读——0x409c..0x40a4=(0,0,-600)（0x4af024=0xc4160000）、
// 0x40a8..0x40b0=(0,300,600)（0x4af020=300/0x4af01c=600）、0x40b4..0x40bc=up。
TEST(stage_scenery_initialize_seeds_406120_camera_block) {
    GameRandom random{0};
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    const auto std_bytes = synthetic_std();
    StageScenery scenery;
    CHECK(scenery.file.open(std_bytes.data(), u32(std_bytes.size())));
    CHECK(load_bank(manager, 7, synthetic_anm(8, trivial_scripts(1))));
    CHECK(scenery.initialize(manager, 7));
    const auto& camera = scenery.script.state.camera;
    CHECK_EQ(camera.position.x, 0.f);
    CHECK_EQ(camera.position.y, 0.f);
    CHECK_EQ(camera.position.z, -600.f);
    CHECK_EQ(camera.direction.x, 0.f);
    CHECK_EQ(camera.direction.y, 300.f);
    CHECK_EQ(camera.direction.z, 600.f);
    CHECK_EQ(camera.up.x, 0.f);
    CHECK_EQ(camera.up.y, 1.f);
    CHECK_EQ(camera.up.z, 0.f);
    CHECK_EQ(camera.eye_offset.x, 0.f);
    CHECK_EQ(camera.eye_offset.y, 0.f);
    CHECK_EQ(camera.eye_offset.z, 0.f);
    CHECK_EQ(camera.target_offset.x, 0.f);
    CHECK_EQ(camera.target_offset.y, 0.f);
    CHECK_EQ(camera.target_offset.z, 0.f);
    CHECK_EQ(camera.animation_delta.z, 0.f);
    CHECK_EQ(camera.fov, 0.f);
    CHECK_EQ(camera.fog.color, 0u);
    CHECK_EQ(scenery.script.state.culling_distance_squared, 9610000.f);
    // st01.std off=100 的 op2 首帧差分基线：900 - (-600) = 1500。
    const float op2_z = 900.f;
    CHECK_EQ(float(op2_z - camera.position.z), 1500.f);
}

// B：0x43bbc0/0x43bed0 启动块 = position(0,0,1000)、dir/reference/eye 零。
// 00461e80.c:56-61：var10018 = 0x4dcb38 + 0x4dcafc = 1000（camera_origin.z +
// screen_translation.z）；var10016/10017 = 0。换关延续（块非零）不覆盖。
TEST(anm_environment_native_camera_block_seed) {
    AnmEnvironment environment;
    CHECK_EQ(environment.screen_translation.z, 0.f);  // release_run 清零后的新局态
    CHECK(seed_native_camera_block_if_cleared(environment));
    CHECK_EQ(environment.screen_translation.x, 0.f);
    CHECK_EQ(environment.screen_translation.y, 0.f);
    CHECK_EQ(environment.screen_translation.z, 1000.f);
    CHECK_EQ(environment.camera_origin.x, 0.f);
    CHECK_EQ(environment.camera_origin.y, 0.f);
    CHECK_EQ(environment.camera_origin.z, 0.f);
    CHECK_EQ(environment.reference_position.x, 0.f);
    CHECK_EQ(environment.reference_position.z, 0.f);
    CHECK_EQ(environment.stage_direction.z, 0.f);
    CHECK_EQ(environment.camera_origin.z + environment.screen_translation.z, 1000.f);

    environment.screen_translation = {1.f, 2.f, 3.f};
    environment.camera_origin = {4.f, 5.f, 6.f};
    CHECK(!seed_native_camera_block_if_cleared(environment));
    CHECK_EQ(environment.screen_translation.x, 1.f);
    CHECK_EQ(environment.screen_translation.z, 3.f);
    CHECK_EQ(environment.camera_origin.z, 6.f);
}

// C：scenery 趟末已恢复 Fullscreen 相机；world_quad 绘制时必须临时选回
// scenery_camera/scenery_viewport，并在提交后恢复调用方相机/视口。
TEST(world_quad_inherits_stage_camera_outside_scenery_pass) {
    GameRandom random{0};
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    AnmVm* vm = make_world_quad(manager, environment);
    CHECK(vm != nullptr);
    if (!vm) return;
    RecordingGraphics graphics;
    AnmRenderer renderer{graphics};
    renderer.camera.view.m[12] = 99.f;             // 当前 Fullscreen 相机
    renderer.scenery_camera.view.m[12] = 12.f;     // 保存的 STD 舞台相机
    renderer.scenery_viewport = {116, 2, 408, 476, 0, 1};
    renderer.scenery_camera_ready = true;
    CHECK_EQ(renderer.draw(*vm), 0);
    CHECK_EQ(graphics.batches, 1u);
    CHECK_EQ(graphics.draw_view_x, 12.f);
    CHECK_EQ(graphics.draw_viewport_width, 408.f);
    CHECK_EQ(renderer.camera.view.m[12], 99.f);    // 调用方相机已恢复
    CHECK_EQ(graphics.view.m[12], 99.f);           // 设备 View 已恢复
    CHECK_EQ(renderer.viewport.width, 640u);
}

TEST(world_quad_without_stage_camera_keeps_current_camera) {
    GameRandom random{0};
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    AnmVm* vm = make_world_quad(manager, environment);
    CHECK(vm != nullptr);
    if (!vm) return;
    RecordingGraphics graphics;
    AnmRenderer renderer{graphics};
    AnmCamera current;
    current.view.m[12] = 99.f;
    renderer.set_camera(current);  // 设备态与调用方相机同步（生产里由上一趟绘制建立）
    renderer.scenery_camera_ready = false;
    CHECK_EQ(renderer.draw(*vm), 0);
    CHECK_EQ(graphics.draw_view_x, 99.f);
    CHECK_EQ(graphics.draw_viewport_width, 640.f);
    CHECK_EQ(renderer.camera.view.m[12], 99.f);
}

// 副链/界面层（layer22+，原版 0x46ef60 在 layer21 已切 Fullscreen）的世界 quad
// 不继承 STD 舞台相机：如 layer31 ascii/loading 世界 quad，原生用全屏相机
// （view t=(-320,240,895.69)、proj fov30 4:3）。
TEST(world_quad_fore_pool_layer_keeps_current_camera) {
    GameRandom random{0};
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    AnmVm* vm = make_world_quad(manager, environment);
    CHECK(vm != nullptr);
    if (!vm) return;
    vm->visual.layer = 31;
    RecordingGraphics graphics;
    AnmRenderer renderer{graphics};
    AnmCamera current;
    current.view.m[12] = 99.f;
    renderer.set_camera(current);
    renderer.scenery_camera.view.m[12] = 12.f;
    renderer.scenery_viewport = {116, 2, 408, 476, 0, 1};
    renderer.scenery_camera_ready = true;
    CHECK_EQ(renderer.draw(*vm), 0);
    CHECK_EQ(graphics.draw_view_x, 99.f);
    CHECK_EQ(graphics.draw_viewport_width, 640.f);
    CHECK_EQ(renderer.camera.view.m[12], 99.f);
}

int main() { return th13test::run_all("H draw camera"); }
