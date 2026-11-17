// 自机弹 VM 主链成员回归（0x445c40 → 0x46f3f0 → 0x46f170）。
//
// 原版证据（capstone 0x445c40 / 0x46f3f0 / 0x46f170，反编译
// artifacts/decomp/jp/functions/00445c40.c:86-104）：
//   * 0x445c40 每次出膛：0x4706c0 池分配 → 0x403800 模板拷贝 → 0x462450
//     创建首 tick → 0x46f3f0 主链尾插（flags=0；非 0x46f4a0 头插）；
//   * 0x46f170 在 tick 每个节点前先保存 next，本帧尾插到链尾的子 VM 只有
//     在父之后还存在链节点时才会被同一趟遍历到达——自机弹 VM 是持续尾插
//     的主链正式成员，正是 Reimu S1 2406 一帧相位差缺失的后继节点；
//   * 0x470110 出界回收：render_flags|=0x20，链遍历看到 0x20 不 tick 直接
//     回收（0x46f170:29-35 → 0x46f690），下一帧 0x4464d0 查无 VM 清槽。
//
// 本套件用 StagePresentation::initialize_player_shot（生产创建路径）与
// AnmManager::update(false)（Application Update 34 = 0x46f170）做正负控：
//   * 有自机弹跟随者 → 子 VM 创建帧拿到第二次链 tick（age 2）；
//   * 无跟随者（父即链尾）→ 子 VM 只有创建首 tick（age 1）。
#include "anm_test_fixtures.hpp"
#include "game/AnmRenderer.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"
#include "game/ZunGraphics.hpp"

#include <memory>

using namespace th13;

namespace {

i32 bits(float value) { i32 result; std::memcpy(&result, &value, 4); return result; }

struct NullGraphics final : ZunGraphics {
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

struct Fixture {
    GameRandom random{7};
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    StagePresentation presentation{manager, renderer, {4, 5, 6}, 0};
    u32 parent = 0;
    bool ready = false;

    Fixture() {
        std::vector<u8> parent_script, child_script, shot_script;
        // 取消弹父（bullet.anm script167 形态）：time2 生成子 VM 后停等。
        th13test::push_insn(parent_script, 500, 2, 0, {1});
        th13test::push_insn(parent_script, 3, 0);
        th13test::push_terminator(parent_script);
        // 子 VM：time0 视觉抽签后停等；age 暴露创建首 tick 与同帧链 tick。
        th13test::push_insn(child_script, 117, 0, 3,
                            {bits(10010.f), bits(10011.f), bits(0.5f)});
        th13test::push_insn(child_script, 3, 0);
        th13test::push_terminator(child_script);
        // 自机弹脚本（plXX.anm 弹型脚本形态）：长寿命停等链节点。
        th13test::push_insn(shot_script, 3, 0);
        th13test::push_terminator(shot_script);
        const auto bullet = th13test::build_simple_anm({parent_script, child_script});
        const auto player = th13test::build_simple_anm({shot_script});
        if (!manager.load(5, bullet.data(), u32(bullet.size()))) return;
        if (!manager.load(6, player.data(), u32(player.size()))) return;
        // 0x411663 → 0x46f820 flags=0：取消弹父 VM 主链尾插。
        parent = manager.create(5, 0, -1, 0);
        if (!parent) return;
        ready = true;
    }

    // 0x46f170 主链遍历一趟（Application Update 34）。
    bool chain() { return manager.update(false); }

    u32 spawn_shot(u32 index, u32 generation) {
        PlayerShot shot;
        shot.alive = true;
        shot.script = 0;
        shot.generation = generation;
        if (!presentation.initialize_player_shot(shot, index)) return 0;
        // 0x46f3f0 尾插：刚创建的自机弹 VM 即主链尾节点。
        const u32 count = manager.registry.ordered_count(false);
        return count ? manager.registry.ordered_handle(false, count - 1) : 0;
    }

    AnmVm* child() {
        for (u32 h = 1; h < 0x10000u; ++h)
            if (auto* vm = manager.registry.find(h))
                if (vm->source_script == 1) return vm;
        return nullptr;
    }
};

}  // namespace

TEST(player_shot_vm_is_tail_inserted_main_chain_member) {
    Fixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    const u32 first = fixture.spawn_shot(0, 11);
    const u32 second = fixture.spawn_shot(1, 22);
    CHECK(first != 0);
    CHECK(second != 0);
    // 0x46f3f0 尾插序：链 = [父(0x46f820 尾插), 自机弹 A, 自机弹 B]，
    // 先创建的自机弹在链前，后创建的在链尾。
    CHECK_EQ(fixture.manager.registry.ordered_handle(false, 0), fixture.parent);
    CHECK_EQ(fixture.manager.registry.ordered_handle(false, 1), first);
    CHECK_EQ(fixture.manager.registry.ordered_handle(false, 2), second);
    // 创建首 tick 已由 0x462450 完成（age 1）；链遍历再统一 tick 一次。
    auto* a = fixture.manager.registry.find(first);
    auto* b = fixture.manager.registry.find(second);
    CHECK(a && b);
    if (!a || !b) return;
    CHECK_EQ(a->age.current, 1);
    CHECK_EQ(b->age.current, 1);
    CHECK(fixture.chain());
    CHECK_EQ(a->age.current, 2);
    CHECK_EQ(b->age.current, 2);
}

TEST(birth_path_writes_the_shot_pose_into_the_registered_vm) {
    // 0x445c40.c:129-131：出生尾段把弹对象当前位姿写回 VM+0x574/578/57c。
    // 生产 sync 会在链 tick 前再写一次；直接调用出生路径也必须得到弹位姿。
    Fixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    PlayerShot shot;
    shot.alive = true;
    shot.script = 0;
    shot.generation = 51;
    shot.x = 123.5f;
    shot.y = 45.25f;
    CHECK(fixture.presentation.initialize_player_shot(shot, 0));
    const u32 count = fixture.manager.registry.ordered_count(false);
    const u32 handle = count ? fixture.manager.registry.ordered_handle(false, count - 1) : 0;
    auto* vm = fixture.manager.registry.find(handle);
    CHECK(vm != nullptr);
    if (!vm) return;
    CHECK_NEAR(vm->visual.translation.x, 123.5, 1e-6);
    CHECK_NEAR(vm->visual.translation.y, 45.25, 1e-6);
    CHECK_NEAR(vm->visual.translation.z, 0.0, 1e-6);
}

TEST(tail_inserted_player_shot_lets_child_tick_same_frame) {
    // 正控：父 VM（取消弹）之后尾插了自机弹 VM；父在本帧链遍历中生成
    // script166 子 VM 时，遍历的 next 非空，继续走到链尾的子 VM，子 VM
    // 获得创建首 tick + 同帧链 tick（age 2）——原生 2387 双 0x462450。
    Fixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    CHECK(fixture.spawn_shot(0, 31) != 0);
    // 父在 time2 生成子：创建首 tick + 两趟链遍历后到达 time2。
    for (int frame = 0; frame < 3 && !fixture.child(); ++frame) CHECK(fixture.chain());
    AnmVm* child = fixture.child();
    CHECK(child != nullptr);
    if (!child) return;
    CHECK_EQ(child->age.current, 2);
    CHECK(fixture.manager.registry.find(fixture.parent) != nullptr);
}

TEST(tail_parent_without_follower_misses_same_frame_child_tick) {
    // 负控：父即链尾（没有自机弹等后继节点），子 VM 只有创建首 tick
    //（age 1）——即端口旧 shots_ 链外形态下 Reimu S1 2406 的相位差。
    Fixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    for (int frame = 0; frame < 3 && !fixture.child(); ++frame) CHECK(fixture.chain());
    AnmVm* child = fixture.child();
    CHECK(child != nullptr);
    if (!child) return;
    CHECK_EQ(child->age.current, 1);
}

TEST(retired_player_shot_is_collected_without_chain_tick) {
    // 0x470110：出界死把 VM render_flags|=0x20；链遍历不 tick 直接回收
    //（0x46f170:29-35），age 不再推进；句柄失效（0x4464d0 查无 VM 清槽）。
    Fixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    const u32 handle = fixture.spawn_shot(0, 41);
    CHECK(handle != 0);
    AnmVm* vm = fixture.manager.registry.find(handle);
    CHECK(vm != nullptr);
    if (!vm) return;
    CHECK_EQ(vm->age.current, 1);
    u32 retired = handle;
    CHECK(fixture.manager.retire(retired));
    CHECK_EQ(retired, 0u);
    CHECK_EQ(vm->visual.render_flags & 0x20u, 0x20u);
    CHECK(fixture.chain());
    CHECK_EQ(vm->age.current, 1);  // 未被链遍历 tick
    CHECK(fixture.manager.registry.find(handle) == nullptr);
}

// 钩子生命周期：构造时安装、release 清 battle_（钩子跳过）、析构清空钩子。
// AnmManager::update(false) 在无战斗绑定时仍返回 true（不写快照）。
TEST(presentation_installs_and_clears_the_main_chain_hook) {
    GameRandom random{3};
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    {
        auto presentation = std::make_unique<StagePresentation>(
            manager, renderer, StagePresentation::Banks{4, 5, 6}, 0);
        CHECK(manager.after_main_update != nullptr);
        CHECK(manager.update(false));
        CHECK(presentation->error().empty());
        presentation->release();
        CHECK(manager.update(false));  // battle_ 已清空：钩子跳过且不失败
        CHECK(presentation->error().empty());
        presentation.reset();
    }
    CHECK(manager.after_main_update == nullptr);
}

int main() { return th13test::run_all("player shot main chain membership"); }
