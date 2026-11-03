#include "../harness/test_framework.hpp"
#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"

using namespace th13;

TEST(enemy_manager_reserves_zero_and_assigns_birth_serials) {
    StageSim sim;
    std::vector<u8> code;
    th13test::emit(code, 0, 40, 0, 255, 0, {0});
    th13test::emit(code, 10000, 1, 0, 255, 0, {});
    auto bytes = th13test::build_scpt({{"main", code}});
    CHECK_EQ(sim.program.attach(bytes.data(), u32(bytes.size())), 0);
    sim.commands.sim = &sim; sim.gvars.sim = &sim;
    auto* root = sim.spawn_enemy("main", 0, 0, 100, nullptr, false);
    auto* child = sim.spawn_enemy("main", 0, 0, 100, root, false);
    CHECK(root != nullptr); CHECK(child != nullptr);
    // 0x418d30（0x42b990 关卡装载唯一 ctl 构造）memset 后 [0xc0]=0、
    // 计数器 [0xbc] 置 1（00418d30：uVar1=[0x2f]=0 → [0x30]=0 →
    // [0x2f]=uVar1+1）。原版 root main 走同一计数器：0x42c550 关卡首帧
    // 0042c766/0042c905 `push 0x4aade0("main"); push 0x54 清零块;
    // call 0x4183f0`，0x418f40:96 取 [ctl+0xbc] 写 +0x11E4 后 +1。
    // 故 root=1、首个 ECL 敌机=2，child.trace_id = parent->script_id = 1。
    // 原生实证：round-23 native-s5 battle-state-raw-v2 f0 敌链 =
    // [serial 2 (trace_id 1), serial 1 (trace_id 0)]。
    CHECK_EQ(root->serial, 1u); CHECK_EQ(root->script_id, 1);
    CHECK_EQ(child->serial, 2u); CHECK_EQ(child->trace_id, 1);
    // 0x417f40 反汇编：param_2 == 0 时直接返回 0（id 0 是「无目标」哨兵），
    // 非零才线性查 +0x11e4。计数器从 1 起，root 的 id=1 可寻址。
    CHECK(sim.find_by_script_id(0) == nullptr);
    CHECK(sim.find_by_script_id(1) == root);
    CHECK(sim.find_by_script_id(2) == child);
}

int main() { return th13test::run_all("enemy serial"); }
