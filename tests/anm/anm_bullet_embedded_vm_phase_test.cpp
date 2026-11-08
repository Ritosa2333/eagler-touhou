// 0x461cb0 嵌入弹体表现 VM 的相位回归（B3 audit requiredTests #3）。
//
// 原版证据（capstone）：
//   * 0x461cb0 只做字段设置并调用一次 0x462450（0x461d58），没有 0x46f3f0/
//     0x46f4a0 入链调用——弹体根/表现 VM 不属于 0x46f170 主链；
//   * 0x46f990 的 op500 子 VM 经 0x46f3f0 尾插（ordering=0）进主链；
//   * 0x46f170 主链在 tick 前先保存 next（0x46f1c5/0x46f1d9），因此尾插子 VM
//     是否在创建帧被再次 tick，取决于创建（bullet-manager 0x40e780 相位）与
//     主链遍历的先后：bullet-manager 先于主链 → 当帧可达；主链先于
//     bullet-manager → 当帧不可达（下一帧才被主链 tick）。
//
// 本测试直接走生产路径 StagePresentation::initialize_bullet / tick_bullet
//（0x461cb0 的 port 对应物），用合成 bullet.anm bank 5（类型 0 → 脚本 35）：
//   (a) initialize_bullet/tick_bullet 后 registry 无该 VM，主链遍历不 tick 它；
//   (b) 该 VM 的 op500 子 VM ordering=0 且位于主链尾部；
//   (c) 子 VM 同帧可达性随 bullet-manager / 主链相位翻转（正负控）。
#include "anm_test_fixtures.hpp"
#include "../ui/ui_test_fixtures.hpp"

#include "game/AnmEnvironment.hpp"
#include "game/AnmRenderer.hpp"
#include "game/StagePresentation.hpp"
#include "game/StageSim.hpp"
#include "game/ZunGraphics.hpp"

#include <cstdio>
#include <cstring>
#include <vector>

using namespace th13;
namespace th13 {
using AnmRandomObserver = void (*)(const AnmVm&, i32, i32, i32, u32, u32, u32, u32, bool);
extern AnmRandomObserver anm_random_observer;
}
namespace {

i32 bits(float value) { i32 result; std::memcpy(&result, &value, 4); return result; }

// ZunGraphics 纯虚桩（同 ecl_anm_effect_flush_test：逻辑帧验证，不落 GPU）。
struct NullGraphics final : ZunGraphics {
    touhou::graphics::PipelineState& pipeline() override { return pipeline_state; }
    u32 texture(const AnmResource&, u32) override { return 0; }
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
    touhou::graphics::PipelineState pipeline_state;
};

// 观察到的 op117 视觉抽签：(VM 地址, 变量号)。
struct Draw {
    const AnmVm* vm = nullptr;
    i32 variable = 0;
};
std::vector<Draw> draws;
void observe(const AnmVm& vm, i32 opcode, i32 variable, i32, u32 before, u32, u32 after,
             u32, bool) {
    if (opcode == 117 && before != after) draws.push_back({&vm, variable});
}
size_t draws_for(const AnmVm* vm) {
    size_t count = 0;
    for (const auto& draw : draws)
        if (draw.vm == vm) ++count;
    return count;
}

struct Rig {
    GameRandom random{12345};
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    StagePresentation presentation{manager, renderer, StagePresentation::Banks{4, 5, 6}, 0};
    bool ready = false;

    Rig() {
        // 合成 bullet.anm（bank 5）：36 个脚本，类型 0 的外观脚本 = 35
        //（BulletAppearance.cpp 首项）。脚本 35：
        //   time0 op117（视觉抽签，观察者借此拿到嵌入 VM 地址）
        //   time0 op500(script 1)（0x46f990 子 VM）
        //   time1 op117（若主链错误地 tick 该 VM，会多出一次抽签）
        //   time5 op6(1000) 长等待。
        // 脚本 1（子 VM）：op6(20) 倒计时 + op2；timer -19/-18 可区分
        // 「创建首 tick」与「创建首 tick + 主链同帧 tick」。
        auto scripts = th13::test::trivial_scripts(36);
        scripts[35] = {
            {117, 0, {bits(10010.f), bits(10011.f), bits(0.5f)}, 3},
            {500, 0, {1}, 0},
            {117, 1, {bits(10010.f), bits(10011.f), bits(0.5f)}, 3},
            {6, 5, {1000}, 0},
            {-1, 0, {}, 0},
        };
        scripts[1] = {
            {6, 0, {20}, 0},
            {2, 0, {}, 0},
            {-1, 0, {}, 0},
        };
        const auto data = th13::test::synthetic_anm(8, scripts);
        ready = th13::test::load_bank(manager, 5, data);
    }

    // 0x461cb0 port 路径：创建首 tick（内部 0x462450 一次）。
    // interrupt=0：合成脚本没有外观脚本的 label 2，若带 pending_interrupt
    // 则下一 tick 会走「未解析中断」分支而不执行 op117，干扰相位断言；
    // 中断语义与本测试无关。
    bool initialize(StageBullet& bullet) {
        draws.clear();
        anm_random_observer = observe;
        return presentation.initialize_bullet(bullet, 0);
    }

    const AnmVm* embedded() const {
        for (const auto& draw : draws)
            if (draw.variable == 10011) return draw.vm;
        return nullptr;
    }

    const AnmVm* first_child(const AnmVm* parent) {
        if (!parent) return nullptr;
        const auto children = manager.registry.children(*parent);
        return children.empty() ? nullptr : children[0];
    }
};

}  // namespace

TEST(embedded_bullet_vm_is_not_a_main_chain_node) {
    Rig rig;
    CHECK(rig.ready);
    if (!rig.ready) return;
    StageBullet bullet;
    bullet.sprite = 0;  // 类型 0 → 外观脚本 35
    bullet.subtype = 0;
    bullet.generation = 1;

    CHECK(rig.initialize(bullet));
    const AnmVm* embedded = rig.embedded();
    CHECK(embedded != nullptr);
    if (!embedded) return;
    // 创建首 tick 恰一次视觉抽签；op500 子 VM 已尾插主链。
    CHECK_EQ(draws_for(embedded), size_t(1));
    CHECK_EQ(rig.manager.registry.ordered_count(false), 1u);
    // 嵌入 VM 不是主链节点：registry 无句柄、主链句柄表里没有它。
    CHECK_EQ(rig.manager.registry.handle(*embedded), 0u);
    for (u32 i = 0; i < rig.manager.registry.ordered_count(false); ++i) {
        const u32 handle = rig.manager.registry.ordered_handle(false, i);
        CHECK(handle != 0);
        CHECK(rig.manager.registry.find(handle) != embedded);
    }
    // 主链遍历（0x46f170）不得 tick 它：抽签数不变，脚本偏移不前进。
    const i32 offset = embedded->instruction_offset;
    CHECK(rig.manager.update());
    CHECK_EQ(draws_for(embedded), size_t(1));
    CHECK_EQ(embedded->instruction_offset, offset);
    // 生产 tick_bullet（bullet-manager 相位的后续 tick）才推进它。
    CHECK(rig.presentation.tick_bullet(bullet, false));
    CHECK_EQ(draws_for(embedded), size_t(2));
    // tick_bullet 之后仍不在主链。
    CHECK_EQ(rig.manager.registry.handle(*embedded), 0u);
    anm_random_observer = nullptr;
}

TEST(op500_child_of_embedded_bullet_vm_is_tail_inserted_ordering_zero) {
    Rig rig;
    CHECK(rig.ready);
    if (!rig.ready) return;
    // 先建一个头插（ordering=2）标记 VM，证明子 VM 走尾插而非头插。
    const u32 marker = rig.manager.create(5, 2, -1, 2);
    CHECK(marker != 0);
    StageBullet bullet;
    bullet.sprite = 0;
    bullet.subtype = 0;
    bullet.generation = 1;
    CHECK(rig.initialize(bullet));
    const AnmVm* embedded = rig.embedded();
    CHECK(embedded != nullptr);
    if (!embedded) return;
    const auto children = rig.manager.registry.children(*embedded);
    CHECK_EQ(children.size(), size_t(1));
    if (children.size() != 1) return;
    // 0x46f990 → 0x46f3f0：ordering=0 尾插。
    CHECK_EQ(children[0]->ordering, 0u);
    const u32 child = rig.manager.registry.handle(*children[0]);
    CHECK(child != 0);
    CHECK_EQ(rig.manager.registry.ordered_count(false), 2u);
    CHECK_EQ(rig.manager.registry.ordered_handle(false, 0), marker);  // 头插标记在前
    CHECK_EQ(rig.manager.registry.ordered_handle(false, 1), child);   // op500 子 VM 在尾
    anm_random_observer = nullptr;
}

TEST(bullet_manager_tick_before_main_chain_reaches_child_same_frame) {
    // 正控（原版任务序：bullet-manager 0x40e780 → 主链 0x46f170）：
    // 子 VM 创建首 tick 后，同一帧稍后的主链遍历尾插可达 → 创建帧两次 tick。
    Rig rig;
    CHECK(rig.ready);
    if (!rig.ready) return;
    StageBullet bullet;
    bullet.sprite = 0;
    bullet.subtype = 0;
    bullet.generation = 1;
    CHECK(rig.initialize(bullet));
    const AnmVm* embedded = rig.embedded();
    const AnmVm* child = rig.first_child(embedded);
    CHECK(child != nullptr);
    if (!child) return;
    CHECK_EQ(child->timer.current, -19);  // 仅 0x46f990 创建首 tick
    CHECK(rig.manager.update());          // 同帧主链
    CHECK_EQ(child->timer.current, -18);  // 尾插子 VM 当帧被 tick
    CHECK_EQ(draws_for(embedded), size_t(1));  // 嵌入 VM 自身未被主链 tick
    anm_random_observer = nullptr;
}

TEST(main_chain_before_bullet_manager_tick_misses_child_until_next_frame) {
    // 负控（相位翻转：主链先于 bullet-manager）：子 VM 在创建帧不可达，
    // 只有创建首 tick；下一帧主链才第二次 tick 它。
    Rig rig;
    CHECK(rig.ready);
    if (!rig.ready) return;
    CHECK(rig.manager.update());  // 本帧主链先跑（此时尚无子 VM）
    StageBullet bullet;
    bullet.sprite = 0;
    bullet.subtype = 0;
    bullet.generation = 1;
    CHECK(rig.initialize(bullet));  // bullet-manager 相位晚于主链
    const AnmVm* embedded = rig.embedded();
    const AnmVm* child = rig.first_child(embedded);
    CHECK(child != nullptr);
    if (!child) return;
    CHECK_EQ(child->timer.current, -19);  // 当帧不可达
    CHECK(rig.manager.update());          // 下一帧主链
    CHECK_EQ(child->timer.current, -18);
    anm_random_observer = nullptr;
}

int main() { return th13test::run_all("anm bullet embedded vm phase"); }
