// AnmVm::tick 单测（④：时间门槛/结束语义/标签扫描/中断保存恢复/几何模式/变量）。
// 合成指令流按 §三 布局构造；断言依据 §四 行为表。
#include "anm_test_fixtures.hpp"

#include <cmath>

using namespace th13;

namespace {

th13::GameRandom rng{0};

TEST(time_gate_and_order) {
    // time 门槛：insn.time <= current 才执行（00462450.c:314）。
    std::vector<th13::u8> bytes;
    th13test::push_insn(bytes, 402, 0, 0, {0x40400000, 0x40400000});  // scale=(3,3)
    th13test::push_insn(bytes, 402, 3, 0, {0x40E00000, 0x41000000});  // scale=(7,8) @f3
    th13test::push_insn(bytes, 2, 4);                                 // t4 停止
    auto script = th13test::make_script(std::move(bytes));
    th13::AnmVm vm;
    CHECK(vm.bind(script));
    CHECK_EQ(vm.timer.current, 0);
    CHECK_EQ(vm.tick(rng), 0);
    CHECK_NEAR(vm.visual.scale.x, 3.0, 1e-6);
    CHECK_EQ(vm.timer.current, 1);
    vm.tick(rng);
    vm.tick(rng);
    CHECK_NEAR(vm.visual.scale.x, 3.0, 1e-6);  // 第 3 帧末仍未到 time=3
    CHECK_EQ(vm.tick(rng), 0);                 // 第 4 帧：t3 执行，t4 门槛挡下
    CHECK_NEAR(vm.visual.scale.x, 7.0, 1e-6);
    CHECK_NEAR(vm.visual.scale.y, 8.0, 1e-6);
}

TEST(terminator_and_opcodes_1_2) {
    {   // op1 结束：清可见位、IP=-1、tick 返回 1（§四 结束语义）。
        std::vector<th13::u8> bytes;
        th13test::push_insn(bytes, 1, 0);
        th13test::push_terminator(bytes);
        auto script = th13test::make_script(std::move(bytes));
        th13::AnmVm vm;
        CHECK(vm.bind(script));
        CHECK_EQ(vm.tick(rng), 1);
        CHECK(!vm.visible);
        CHECK_EQ(vm.instruction_offset, -1);
        CHECK_EQ(vm.tick(rng), 0);  // IP=-1：休眠，不再推进
    }
    {   // op2 停止：IP 置空、VM 存活休眠，返回 0。
        std::vector<th13::u8> bytes;
        th13test::push_insn(bytes, 2, 0);
        th13test::push_terminator(bytes);
        auto script = th13test::make_script(std::move(bytes));
        th13::AnmVm vm;
        CHECK(vm.bind(script));
        CHECK_EQ(vm.tick(rng), 0);
        CHECK_EQ(vm.instruction_offset, -1);
    }
}

TEST(label_scan_and_fallback) {
    // 中断标签扫描：命中 id；无命中回落 id==-1（§三/§四）。
    // 布局：op3 停等在前，标签在正常流中不可达（op3 不推进 IP 即返回）。
    std::vector<th13::u8> bytes;
    th13test::push_insn(bytes, 3, 0);                                      // 停等中断 @0
    th13test::push_label(bytes, -1, 0);                                    // 回落标签 @8
    th13test::push_label(bytes, 7, 0);                                     // 目标标签 @20
    th13test::push_insn(bytes, 402, 0, 0, {0x41100000, 0x41100000});       // scale=(9,9) @32
    th13test::push_insn(bytes, 7, 0, 0, {});                               // 中断返回 @44
    th13test::push_terminator(bytes);
    auto script = th13test::make_script(std::move(bytes));
    th13::AnmVm vm;
    CHECK(vm.bind(script));
    CHECK_EQ(vm.tick(rng), 0);
    CHECK(vm.visual.flags & 0x4000);   // 停等
    CHECK(!vm.visible);
    CHECK_NEAR(vm.visual.scale.x, 1.0, 1e-6);  // 标签块未执行
    // 中断 label=7：保存 {time=0, IP@op3}，跳至标签后执行，op7 恢复后再次停等。
    vm.pending_interrupt = 7;
    CHECK_EQ(vm.tick(rng), 0);
    CHECK_NEAR(vm.visual.scale.x, 9.0, 1e-6);
    CHECK_EQ(vm.timer.current, 0);     // op7 恢复保存的时间
    CHECK(vm.visual.flags & 0x4000);   // 恢复后再次停等
    CHECK(vm.visible);                 // 中断置可见（flags|=1），op3 不清
    // 无命中标签：回落 ins_5(-1)，同样跳转执行。
    vm.visual.scale = {1, 1};
    vm.pending_interrupt = 99;
    CHECK_EQ(vm.tick(rng), 0);
    CHECK_NEAR(vm.visual.scale.x, 9.0, 1e-6);
    CHECK_EQ(vm.timer.current, 0);
    // 无任何命中且无回落：停等一帧返回（decrement+tick 相抵，时间冻结）。
    th13::AnmVm bare;
    std::vector<th13::u8> plain;
    th13test::push_insn(plain, 3, 0);
    th13test::push_terminator(plain);
    auto plain_script = th13test::make_script(std::move(plain));
    bare.bind(plain_script);
    CHECK_EQ(bare.tick(rng), 0);
    bare.pending_interrupt = 5;
    CHECK_EQ(bare.tick(rng), 0);
    CHECK_EQ(bare.timer.current, 0);   // 无命中：时间冻结一帧
    CHECK_EQ(bare.pending_interrupt, 0);
}

TEST(op6_time_rewind) {
    // op6：fractional -= frames 后取整（§四 时间推进；参数为正的回退帧数）。
    std::vector<th13::u8> bytes;
    th13test::push_insn(bytes, 6, 0, 0, {3});
    th13test::push_insn(bytes, 2, -10);  // 门槛远低于回退点：不再推进
    auto script = th13test::make_script(std::move(bytes));
    th13::AnmVm vm;
    CHECK(vm.bind(script));
    CHECK_EQ(vm.tick(rng), 0);
    CHECK_EQ(vm.timer.current, -3);
    CHECK_NEAR(vm.timer.fractional, -3.0, 1e-6);
}

TEST(geometry_modes_th13) {
    // 几何模式号：th13 分发器实测 600→9、603→15、604→16、606→19、608→21。
    struct Row { th13::i16 opcode; th13::u32 mode; };
    const Row rows[] = {{600, 9}, {601, 13}, {602, 14}, {603, 15}, {604, 16}, {605, 17}, {606, 19}, {607, 20}, {608, 21}};
    for (const auto& row : rows) {
        std::vector<th13::u8> bytes;
        th13test::push_insn(bytes, row.opcode, 0, 0, {4, 0x40400000});
        th13test::push_insn(bytes, 2, 0);  // 尾随停止：tick 干净返回 0
        auto script = th13test::make_script(std::move(bytes));
        th13::AnmVm vm;
        CHECK(vm.bind(script));
        CHECK_EQ(vm.tick(rng), 0);
        CHECK_EQ(vm.visual.draw_mode(), row.mode);
        if (row.opcode <= 602) {
            // 600/601/602：count*2 个屏幕顶点（malloc(count*0x38)）。
            CHECK_EQ(vm.geometry.screen_vertices.size(), 8u);
            CHECK_EQ(vm.geometry.allocation_bytes, 4u * 56u);
        }
    }
    // 604：尺寸 x + 工作变量 10000 ← arg1。
    std::vector<th13::u8> bytes;
    th13test::push_insn(bytes, 604, 0, 0, {0x40800000, 33});
    th13test::push_insn(bytes, 2, 0);
    auto script = th13test::make_script(std::move(bytes));
    th13::AnmVm vm;
    CHECK(vm.bind(script));
    CHECK_EQ(vm.tick(rng), 0);
    CHECK_NEAR(vm.visual.sprite_size.x, 4.0, 1e-6);
    CHECK_EQ(vm.variables.integers[0], 33);
}

TEST(unimplemented_opcodes_rejected) {
    // 计划 §四/§六：th13 未用的 th15 残留 opcode 显式报错。
    for (th13::i16 opcode : {th13::i16(109), th13::i16(123), th13::i16(127), th13::i16(422),
                             th13::i16(507), th13::i16(509), th13::i16(609), th13::i16(610), th13::i16(611)}) {
        std::vector<th13::u8> bytes;
        th13test::push_insn(bytes, opcode, 0, 0, {0, 0, 0});
        th13test::push_terminator(bytes);
        auto script = th13test::make_script(std::move(bytes));
        th13::AnmVm vm;
        CHECK(vm.bind(script));
        CHECK_EQ(vm.tick(rng), -1);
        CHECK(vm.error.find("Unimplemented") != std::string::npos);
    }
}

TEST(variable_10026_single_parent) {
    // 10026：一层父的旋转 z + 自身 z 归一（00461e80.c case 0x272a）。
    // mask 置位的浮点参数槽存 float(id) 的位型（thanm 约定），非裸 i32 id。
    const th13::i32 reference = th13::float_to_bits(10026.0f);
    std::vector<th13::u8> bytes;
    th13test::push_insn(bytes, 402, 0, 0x1, {reference, 0});
    th13test::push_insn(bytes, 2, 0);
    auto script = th13test::make_script(std::move(bytes));
    th13::AnmVm parent, vm;
    CHECK(parent.bind(script));  // 任意脚本占位
    vm.bind(script);
    vm.rotation_parent = &parent;
    parent.variables.rotation.z = 1.0f;
    vm.variables.rotation.z = 0.5f;
    CHECK_EQ(vm.tick(rng), 0);
    CHECK_NEAR(vm.visual.scale.x, 1.5, 1e-6);
    // 无父时只取自身。
    th13::AnmVm lone;
    std::vector<th13::u8> more;
    th13test::push_insn(more, 402, 0, 0x1, {reference, 0});
    th13test::push_insn(more, 2, 0);
    auto script2 = th13test::make_script(std::move(more));
    lone.bind(script2);
    lone.variables.rotation.z = 0.25f;
    CHECK_EQ(lone.tick(rng), 0);
    CHECK_NEAR(lone.visual.scale.x, 0.25, 1e-6);
}

TEST(rate_bit_forces_one) {
    // render_flags bit8（0x100）置位 → 倍率强制 1（00462450.c:303，th15 为 bit9）。
    std::vector<th13::u8> bytes;
    th13test::push_insn(bytes, 402, 0, 0, {0x40000000, 0x40000000});
    th13test::push_insn(bytes, 402, 1, 0, {0x40A00000, 0x40A00000});
    th13test::push_insn(bytes, 2, 2);
    auto script = th13test::make_script(std::move(bytes));
    th13::AnmVm vm;
    CHECK(vm.bind(script));
    vm.visual.render_flags |= 0x100;
    CHECK_EQ(vm.tick(rng, 0.25f), 0);
    CHECK_EQ(vm.timer.current, 1);  // 强制 +1，而非 +0.25
    CHECK_EQ(vm.tick(rng, 0.25f), 0);  // time=1 执行，t2 门槛挡下
    CHECK_NEAR(vm.visual.scale.x, 5.0, 1e-6);
    th13::AnmVm plain;
    plain.bind(script);
    CHECK_EQ(plain.tick(rng, 0.25f), 0);
    CHECK_EQ(plain.timer.current, 0);  // 无该位：分数累加 0.25 取整仍 0
}

}  // namespace

int main() { return th13test::run_all("anm-interpreter"); }
