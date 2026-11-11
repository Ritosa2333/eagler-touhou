// ECL 558（0x41e451 flags bit20 镜像位）定向回归。
// 原版序列（games/th13/th13.exe 0x41e451 反汇编，2026-10-28 取证）：
//   eax = value<<20; eax ^= [edi+0x4030](flags); eax &= 0x100000; flags ^= eax
// 展开后 bit20_new = bit20_old ^ ((value<<20 ^ flags) & bit20) —— value 奇数
// 置位、偶数清位（不是"按参数异或翻转"：v=0 必须无条件清位）。
// 逐帧实证链（artifacts/verification/a-f3854/，原版 EXE vs 生产语义驱动）：
// st02 MainSub02 bf1965 op558(1) 置位、bf2925 op558(0) 清位；旧实现在 v=0
// 时为恒等空操作，镜像位残留使后续 root 相对 enmCreate 的 x 全部取反
// —— f3265 GirlC01 同帧 op300/op304 双生位置对调、f3515 GirlA02 镜像、
// f3532 得分首差、f3786 power 首差。修复后 4499 帧敌机/状态流零分歧。
#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"

#include <memory>

using namespace th13;

namespace {

constexpr u32 kMirror = 0x100000u;

struct Rig {
    // StageSim 超栈（harness 同款堆置），避免 -O2 栈帧差异下的溢出。
    std::unique_ptr<StageSim> sim = std::make_unique<StageSim>();
    EclContext context;
    Rig() {
        sim->commands.sim = sim.get();
        sim->gvars.sim = sim.get();
    }
    void mirror_on(StageEnemy& enemy, i32 value) {
        sim->current = &enemy;
        std::vector<u8> code;
        th13test::emit(code, 0, 558, 0, 255, 0, {u32(value)});
        context.instruction = reinterpret_cast<const EclInstruction*>(code.data());
        CHECK_EQ(sim->commands.execute(context, 558), 0);
    }
    // 与 ecl_interrupt_retirement_test 同款 op300 族编码：name="Child"
    //（length=8，"Chil"+'d'+NUL 填充），x/y 为相对父敌机的参数。
    const StageEnemy* spawn(u16 op, StageEnemy& parent, float x, float y) {
        sim->current = &parent;
        std::vector<u8> code;
        th13test::emit(code, 0, op, 0, 255, 0,
                       {8, 0x6c696843, 0x64, th13test::bits_of(x),
                        th13test::bits_of(y), 400, 1000, 1});
        context.instruction = reinterpret_cast<const EclInstruction*>(code.data());
        if (sim->commands.execute(context, op) != 0) return nullptr;
        return sim->enemies.empty() ? nullptr : sim->enemies.back().get();
    }
};

}  // namespace

TEST(op558_sets_bit_for_odd_values_and_clears_for_even_values) {
    Rig r;
    StageEnemy enemy;
    enemy.flags = 0x20020u;  // 零售 root 常见基底位，镜像位清除
    CHECK((enemy.flags & kMirror) == 0);
    r.mirror_on(enemy, 1);
    CHECK((enemy.flags & kMirror) != 0);  // v=1 置位（bf1965 MainSub02）
    r.mirror_on(enemy, 1);
    CHECK((enemy.flags & kMirror) != 0);  // 再置位幂等（旧"异或翻转"实现此处会清零）
    r.mirror_on(enemy, 3);
    CHECK((enemy.flags & kMirror) != 0);  // 奇数置位
    r.mirror_on(enemy, 0);
    CHECK((enemy.flags & kMirror) == 0);  // v=0 清位（bf2925 MainSub02；旧实现恒等空操作——本回归点）
    r.mirror_on(enemy, 1);
    CHECK((enemy.flags & kMirror) != 0);
    r.mirror_on(enemy, 2);
    CHECK((enemy.flags & kMirror) == 0);  // 偶数清位（bit20 = value&1）
    // 其余 flags 位不得被扰动。
    CHECK((enemy.flags & ~kMirror) == 0x20020u);
}

TEST(op300_family_positions_mirror_only_against_mirrored_parent) {
    Rig r;
    std::vector<u8> child, main;
    th13test::emit(child, 0, 23, 0, 255, 0, {600});
    th13test::emit(main, 0, 23, 0, 255, 0, {600});
    auto data = th13test::build_scpt({{"Child", child}, {"main", main}});
    CHECK_EQ(r.sim->program.attach(data.data(), u32(data.size())), 0);
    // 父敌机经 sim.spawn_enemy 建立（create_enemy 读取父线程难度，须有 threads）。
    auto* plain_parent = r.sim->spawn_enemy("main", 0, 0, 100, nullptr, false);
    CHECK(plain_parent != nullptr);
    if (!plain_parent) return;
    // 非镜像父敌机：op300 x 保持原号（bit20 清），op304 x 保持且子敌机 bit20 置位
    //（0x41fcc0:60-62 param2[6]=1）。对应原版 f3265 对：op300(-224)→-224、
    // op304(+224)→+224(镜像位)。
    const auto* a = r.spawn(300, *plain_parent, -224.f, 128.f);
    CHECK(a != nullptr);
    if (a) {
        CHECK_EQ(th13test::bits_of(a->position[0]), th13test::bits_of(-224.f));
        CHECK_EQ(th13test::bits_of(a->position[1]), th13test::bits_of(128.f));
        CHECK((a->flags & kMirror) == 0);
    }
    const auto* b = r.spawn(304, *plain_parent, 224.f, 128.f);
    CHECK(b != nullptr);
    if (b) {
        CHECK_EQ(th13test::bits_of(b->position[0]), th13test::bits_of(224.f));
        CHECK((b->flags & kMirror) != 0);
    }
    // 镜像父敌机（0x41fcc0:63-66）：翻转子的镜像位并 x 取负——正是旧 op558(0)
    // 空操作导致 root 镜像位残留后，f3265/f3515 出现的对调与镜像。
    auto* mirrored_parent = r.sim->spawn_enemy("main", 0, 0, 100, nullptr, false);
    CHECK(mirrored_parent != nullptr);
    if (!mirrored_parent) return;
    mirrored_parent->flags |= kMirror;
    const auto* c = r.spawn(300, *mirrored_parent, -224.f, 128.f);
    CHECK(c != nullptr);
    if (c) {
        CHECK_EQ(th13test::bits_of(c->position[0]), th13test::bits_of(224.f));
        CHECK((c->flags & kMirror) != 0);
    }
    const auto* d = r.spawn(304, *mirrored_parent, 224.f, 128.f);
    CHECK(d != nullptr);
    if (d) {
        CHECK_EQ(th13test::bits_of(d->position[0]), th13test::bits_of(-224.f));
        CHECK((d->flags & kMirror) == 0);
    }
}

int main() { return th13test::run_all("ecl-mirror-flag"); }
