// 镜像朝向判定（0x419450 → 0x4196d3-0x419827）主路径回归。
//
// 反汇编证据（games/th13/th13.exe，2026-11-02 round-23 取证）：
//   - 0x419450 在 0x419950 内先算 inner+0x78（速度子位置累加、round001/钳制
//     前）；0x419694 `flds 0x78(%edi)`，0x4aee20/0x4aee18 为 ±0.03f 的 double
//     视图：v < -0.03f → -1，|v| ≤ 0.03f → 0，v > 0.03f → +1。
//   - 0x41a8a0 调用序 = 0x419450（integrate 后）→ ECL 线程体；0x80000 自门使
//     构造/重挂当帧 manager 遍历空转，该帧不做朝向比较。
//   - 0x4196d3-0x419711 步号表：dir<0→1、dir>0→2；dir==0 时旧 -1→3、
//     旧 +1→4；0x4196db je 0x41982f 跳过相等分支。
//   - 0x41974a-0x41975b 继承旧槽 VM 位置；0x419731 脚本解析自当前 slot0 VM
//     的 resource；0x419764 layer=+0x278+7；0x419827 写 +0x270=dir。
//
// 生产侧对应：StageSim::tick_enemy 在 integrate 后、ECL 线程体前把
// motion_delta[0] 快照进 mirror_velocity_x 并置 mirror_ready；306 在 ECL
// 体内清 ready、以当帧位置作过渡基线。本测试覆盖主路径读快照、306 过渡帧
// 抑制比较、以及三步号映射（含 0.03f 死区）。
#include "ui_test_fixtures.hpp"
#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"

#include <memory>

using namespace th13;
using namespace th13::test;

namespace {

struct Graphics : ZunGraphics {
    touhou::graphics::PipelineState state;
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
};

}  // namespace

TEST(mirror_velocity_snapshot_drives_rebind_and_306_transition_is_suppressed) {
    GameRandom random;
    AnmEnvironment env;
    AnmManager animations(random, env);
    Graphics graphics;
    AnmRenderer renderer(graphics);
    StageBattle battle;
    StagePresentation presentation(animations, renderer, {4, 5, 6}, 0);
    auto bytes = synthetic_anm(1, trivial_scripts(8));
    CHECK(load_bank(animations, 4, bytes));
    CHECK(load_bank(animations, 5, synthetic_anm(1, trivial_scripts(1))));
    CHECK(load_bank(animations, 6, synthetic_anm(1, trivial_scripts(1))));
    auto enemy = std::make_unique<StageEnemy>();
    auto* e = enemy.get();
    e->serial = 1;
    e->anm_file = 2;
    e->mirror_script_base = 2;
    battle.sim.enemies.push_back(std::move(enemy));
    battle.sim.commands.sim = &battle.sim;
    battle.sim.current = e;
    battle.sim.ecl_slot_anm = true;
    presentation.enemies_from_ecl = true;
    // 生产敌机恒有 ECL 线程块（0x4183f0:28）；threads 非空是主路径
    // （mirror_ready || !threads）判定的一部分，此处不 step 线程。
    e->threads = std::make_unique<EclThreads>(battle.sim.program);
    auto command = [&](u16 op, std::initializer_list<u32> args) {
        EclContext c;
        std::vector<u8> code;
        th13test::emit(code, 0, op, 0, 255, 0, args);
        c.instruction = reinterpret_cast<const EclInstruction*>(code.data());
        CHECK_EQ(battle.sim.commands.execute(c, op), 0);
        CHECK(presentation.flush_ecl_animations(battle));
    };

    command(306, {0, 0});  // 0x41b260 case3：+0x26c=script、+0x270=0、置 bit20
    CHECK(e->flags & 0x200000u);
    CHECK(!e->mirror_ready);  // 306 在 ECL 体内，当帧 0x419450 已过去
    CHECK_EQ(e->mirror_dir, 0);
    const u32 root = e->anm_slot_handle[0];
    CHECK(root != 0);

    // 过渡帧（0x80000 自门）：即使位置差分/速度看起来像转向也不得重挂。
    e->position[0] += 5.0f;  // 位置差分回退会得到 +1
    e->mirror_velocity_x = 5.0f;
    CHECK(presentation.sync(battle, 0));
    CHECK(animations.update());
    CHECK_EQ(e->anm_slot_handle[0], root);
    CHECK_EQ(e->mirror_dir, 0);

    // 下一帧：tick_enemy 已快照 +0x78；old 0 → new -1 → step 1。
    e->mirror_ready = true;
    e->mirror_velocity_x = -1.0f;
    CHECK(presentation.sync(battle, 0));
    CHECK(animations.update());
    CHECK(e->anm_slot_handle[0] != root);
    CHECK_EQ(e->mirror_dir, -1);
    {
        auto* vm = animations.registry.find(e->anm_slot_handle[0]);
        CHECK(vm);
        if (vm) {
            CHECK_EQ(vm->source_script, e->anm_base_script + 1);
            CHECK_EQ(vm->visual.layer, e->mirror_script_base + 7);
        }
    }

    // old -1 → new +1 → step 2（0x419708 neg/sbb/add 3 → 2）。
    e->mirror_ready = true;
    e->mirror_velocity_x = 1.0f;
    CHECK(presentation.sync(battle, 0));
    CHECK(animations.update());
    CHECK_EQ(e->mirror_dir, 1);
    {
        auto* vm = animations.registry.find(e->anm_slot_handle[0]);
        CHECK(vm);
        if (vm) CHECK_EQ(vm->source_script, e->anm_base_script + 2);
    }

    // 死区 |vx| ≤ 0.03f → dir 0；old +1 → new 0 → step 4。
    e->mirror_ready = true;
    e->mirror_velocity_x = 0.02f;
    CHECK(presentation.sync(battle, 0));
    CHECK(animations.update());
    CHECK_EQ(e->mirror_dir, 0);
    {
        auto* vm = animations.registry.find(e->anm_slot_handle[0]);
        CHECK(vm);
        if (vm) CHECK_EQ(vm->source_script, e->anm_base_script + 4);
    }
}

int main() { return th13test::run_all("enemy mirror velocity"); }
