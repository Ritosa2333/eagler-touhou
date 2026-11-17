// 敌机自动朝向（0x41a9d1 主路径）回归。
//
// 反汇编证据（games/th13/th13.exe，主会话 2026-11-08 capstone 亲验）：
//   - 0x41a9d1 `test dword [vm+0x1c], 0x40`：仅当脚本 op424 置 render_flags
//     bit6 时进入朝向写入分支（AnmInterpreter case 424 写 bit6）。
//   - 0x41a9f5 `fld dword [enemy+0x7c]`（motion_delta[1]=y）、`fld dword
//     [enemy+0x78]`（motion_delta[0]=x）、`call 0x4977aa`（atan2(y,x)）：
//     角度 = atan2(motion_delta.y, motion_delta.x)。不加 π/2（与弹体
//     0x472e90 的 +π/2 路径不同源）、不取位置角、不用位置差分。
//   - 结果写 VM+0x50（variables.rotation.z）并 visual.flags |= 4。
//   - enemy flags bit27（0x08000000）= 槽位直接跟随分支：位置直接用敌机位
//     （无 inner+160 槽偏移），该分支不写 rotation。
//   - 循环覆盖全部 0xe 槽位（0x41a8a0），非 slot0 同规则。
//
// 生产侧对应：StagePresentation.cpp 槽位同步（sync 的 enemies_from_ecl 槽
// 循环）按同一契约写 rotation/flags。本文件只覆盖该回归；夹具沿用
// enemy_mirror_velocity_test.cpp / enemy_animation_reset_test.cpp。
#include "ui_test_fixtures.hpp"
#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"
#include "game/AnmCoordinates.hpp"

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

// bank4 = 8 个脚本：0 = op424(1) 开 bit6、1 = op401 写 0.5 弧度 rotation、
// 2 = op424(1)（供非 slot0 测试）、3..7 平凡。
std::vector<std::vector<AnmIns>> rotation_scripts() {
    std::vector<std::vector<AnmIns>> scripts(8);
    scripts[0] = {AnmIns{424, 0, {1}}, wait(), stop()};
    scripts[1] = {AnmIns{401, 0, {0, 0, 0x3f000000}}, wait(), stop()};
    scripts[2] = {AnmIns{424, 0, {1}}, wait(), stop()};
    for (u32 i = 3; i < 8; i++) scripts[i] = {wait(), stop()};
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
            if (!th13test::TestFileReader{}.read("artifacts/reference/raw/st07enm.anm", bytes))
                return false;
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
        // 生产敌机恒有 ECL 线程块；threads 非空且 mirror_ready 为假时
        // 0x419450 镜像分支当帧不参与（同 enemy_mirror_velocity_test）。
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

}  // namespace

// op424 开启时：rotation = atan2(motion_delta.y, motion_delta.x)，四象限 +
// 停止(0,0)；不加 π/2、不用位置角（位置非零且不动仍按 motion_delta）、
// 不用位置差分，且 visual.flags 置 bit2。
TEST(op424_rotation_follows_motion_delta_quadrants_and_stop) {
    Rig r;
    CHECK(r.init());
    CHECK(r.command(306, {0, 0}));  // slot0 = bank4 脚本 0（op424(1)）
    CHECK(r.e->flags & 0x200000u);
    CHECK(r.animations.update());
    auto* vm = r.slot_vm(0);
    CHECK(vm);
    if (!vm) return;
    CHECK(vm->visual.render_flags & 0x40u);  // 0x41a9d1 的 bit6 门来自脚本 424

    // 位置角/位置差分判别基线：位置停在非原点且不再移动，角度仍逐帧跟随
    // motion_delta（位置角 atan2(5,10)≈0.4636、位置差分 0、+π/2 均不匹配）。
    r.e->position[0] = 10.0f;
    r.e->position[1] = 5.0f;

    const float quarter = 1.5707963705062866f;  // 原版 π/2 常量域
    struct Case { float dx, dy, want; };
    const Case cases[] = {
        {1, 0, 0.0f},        // 右：不加 π/2（+π/2 路径会得 π/2）
        {0, 1, quarter},     // 下（屏幕 y 向下）：象限 II 侧
        {-1, 0, 3.14159274f},
        {0, -1, -quarter},
        {1, 1, 0.785398185f},
        {-1, 1, 2.3561945f},
        {-1, -1, -2.3561945f},
        {1, -1, -0.785398185f},
        {0, 0, 0.0f},        // 停止：atan2(0,0)=0，不保留旧角
        {0, 1, quarter},     // 停止后再动：恢复跟随
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

// 无 bit40（脚本只用 op401 写 rotation）：0x41a9d1 分支不进入，脚本旋转值
// 原样保留、visual.flags 不置 bit2。
TEST(no_op424_keeps_script_rotation) {
    Rig r;
    CHECK(r.init());
    CHECK(r.command(306, {0, 1}));  // slot0 = 脚本 1（op401 写 0.5，无 424）
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

// flags bit27（0x08000000）直接跟随分支：位置无槽偏移跟随敌机本体，且不写
// rotation（0x41a9d1 分支被绕过），bit2 不置位。
TEST(flag_08000000_direct_follow_skips_rotation_and_slot_offset) {
    Rig r;
    CHECK(r.init());
    CHECK(r.command(306, {0, 0}));  // 脚本 0 = op424(1)
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
    CHECK_EQ(vm->variables.rotation.z, 0.0f);  // 不写 rotation
    CHECK(!(vm->visual.flags & 4u));
    CHECK_EQ(vm->visual.translation.x, 10.0f);  // 直接跟随：无 +7/+8/+9 偏移
    CHECK_EQ(vm->visual.translation.y, 20.0f);
    CHECK_EQ(vm->visual.translation.z, 30.0f);
    CHECK(r.animations.update());
}

// 非 slot0 同规则：306 建 slot0（平凡脚本），303 挂 slot1（op424 脚本）；
// 朝向只写有 bit40 的 slot1，slot0 不受牵连。
TEST(non_slot0_slot_gets_same_rotation_rule) {
    Rig r;
    CHECK(r.init());
    CHECK(r.command(306, {0, 3}));  // slot0 = 脚本 3（平凡）
    CHECK(r.command(303, {1, 2}));  // slot1 = 脚本 2（op424(1)）
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

// 真实 st07enm.anm script10（Extra BossCard9_EtAtBird 敌机本体，含 302(1)/
// 424(1)）：加载路径正确时脚本 op424 置 bit6，朝向按 motion_delta 的
// atan2 写入并提交几何。
TEST(real_st07enm_script10_auto_rotation) {
    Rig r;
    CHECK(r.init(true));
    CHECK(r.command(306, {0, 10}));  // slot0 = st07enm 局部脚本 10
    CHECK(r.animations.update());
    auto* vm = r.slot_vm(0);
    CHECK(vm);
    if (!vm) return;
    CHECK_EQ(vm->source_script, 10);
    CHECK(vm->visual.render_flags & 0x40u);  // script10 的 op424(1) 已生效

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

    // 渲染提交路径：本体所在层逐 VM 提交四边形（2 三角形 6 顶点）。
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
