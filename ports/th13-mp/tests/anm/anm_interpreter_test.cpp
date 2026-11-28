#include "anm_test_fixtures.hpp"

#include <cmath>

using namespace th13;

namespace {

th13::GameRandom rng{0};

TEST(time_gate_and_order) {
    std::vector<th13::u8> bytes;
    th13test::push_insn(bytes, 402, 0, 0, {0x40400000, 0x40400000});
    th13test::push_insn(bytes, 402, 3, 0, {0x40E00000, 0x41000000});
    th13test::push_insn(bytes, 2, 4);
    auto script = th13test::make_script(std::move(bytes));
    th13::AnmVm vm;
    CHECK(vm.bind(script));
    CHECK_EQ(vm.timer.current, 0);
    CHECK_EQ(vm.tick(rng), 0);
    CHECK_NEAR(vm.visual.scale.x, 3.0, 1e-6);
    CHECK_EQ(vm.timer.current, 1);
    vm.tick(rng);
    vm.tick(rng);
    CHECK_NEAR(vm.visual.scale.x, 3.0, 1e-6);
    CHECK_EQ(vm.tick(rng), 0);
    CHECK_NEAR(vm.visual.scale.x, 7.0, 1e-6);
    CHECK_NEAR(vm.visual.scale.y, 8.0, 1e-6);
}

TEST(terminator_and_opcodes_1_2) {
    {
        std::vector<th13::u8> bytes;
        th13test::push_insn(bytes, 1, 0);
        th13test::push_terminator(bytes);
        auto script = th13test::make_script(std::move(bytes));
        th13::AnmVm vm;
        CHECK(vm.bind(script));
        CHECK_EQ(vm.tick(rng), 1);
        CHECK(!vm.visible);
        CHECK_EQ(vm.instruction_offset, -1);
        CHECK_EQ(vm.tick(rng), 0);
    }
    {
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
    std::vector<th13::u8> bytes;
    th13test::push_insn(bytes, 3, 0);
    th13test::push_label(bytes, -1, 0);
    th13test::push_label(bytes, 7, 0);
    th13test::push_insn(bytes, 402, 0, 0, {0x41100000, 0x41100000});
    th13test::push_insn(bytes, 7, 0, 0, {});
    th13test::push_terminator(bytes);
    auto script = th13test::make_script(std::move(bytes));
    th13::AnmVm vm;
    CHECK(vm.bind(script));
    CHECK_EQ(vm.tick(rng), 0);
    CHECK(vm.visual.flags & 0x4000);
    CHECK(!vm.visible);
    CHECK_NEAR(vm.visual.scale.x, 1.0, 1e-6);
    vm.pending_interrupt = 7;
    CHECK_EQ(vm.tick(rng), 0);
    CHECK_NEAR(vm.visual.scale.x, 9.0, 1e-6);
    CHECK_EQ(vm.timer.current, 0);
    CHECK(vm.visual.flags & 0x4000);
    CHECK(vm.visible);
    vm.visual.scale = {1, 1};
    vm.pending_interrupt = 99;
    CHECK_EQ(vm.tick(rng), 0);
    CHECK_NEAR(vm.visual.scale.x, 9.0, 1e-6);
    CHECK_EQ(vm.timer.current, 0);
    th13::AnmVm bare;
    std::vector<th13::u8> plain;
    th13test::push_insn(plain, 3, 0);
    th13test::push_terminator(plain);
    auto plain_script = th13test::make_script(std::move(plain));
    bare.bind(plain_script);
    CHECK_EQ(bare.tick(rng), 0);
    bare.pending_interrupt = 5;
    CHECK_EQ(bare.tick(rng), 0);
    CHECK_EQ(bare.timer.current, 0);
    CHECK_EQ(bare.pending_interrupt, 0);
}

TEST(op6_time_rewind) {
    std::vector<th13::u8> bytes;
    th13test::push_insn(bytes, 6, 0, 0, {3});
    th13test::push_insn(bytes, 2, -10);
    auto script = th13test::make_script(std::move(bytes));
    th13::AnmVm vm;
    CHECK(vm.bind(script));
    CHECK_EQ(vm.tick(rng), 0);
    CHECK_EQ(vm.timer.current, -3);
    CHECK_NEAR(vm.timer.fractional, -3.0, 1e-6);
}

TEST(geometry_modes_th13) {
    struct Row {
        th13::i16 opcode;
        th13::u32 mode;
    };
    const Row rows[] = {{600, 9},  {601, 13}, {602, 14}, {603, 15}, {604, 16},
                        {605, 17}, {606, 19}, {607, 20}, {608, 21}};
    for (const auto& row : rows) {
        std::vector<th13::u8> bytes;
        th13test::push_insn(bytes, row.opcode, 0, 0, {4, 0x40400000});
        th13test::push_insn(bytes, 2, 0);
        auto script = th13test::make_script(std::move(bytes));
        th13::AnmVm vm;
        CHECK(vm.bind(script));
        CHECK_EQ(vm.tick(rng), 0);
        CHECK_EQ(vm.visual.draw_mode(), row.mode);
        if (row.opcode <= 602) {
            CHECK_EQ(vm.geometry.screen_vertices.size(), 8u);
            CHECK_EQ(vm.geometry.allocation_bytes, 4u * 56u);
        }
    }
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
    for (th13::i16 opcode :
         {th13::i16(109), th13::i16(123), th13::i16(127), th13::i16(422), th13::i16(507), th13::i16(509)}) {
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

TEST(opcodes_past_608_advance_without_error) {
    for (th13::i16 opcode : {th13::i16(609), th13::i16(610), th13::i16(611)}) {
        std::vector<th13::u8> bytes;
        th13test::push_insn(bytes, opcode, 0, 0, {0, 0, 0});
        th13test::push_terminator(bytes);
        auto script = th13test::make_script(std::move(bytes));
        th13::AnmVm vm;
        CHECK(vm.bind(script));
        CHECK_EQ(vm.tick(rng), 1);
        CHECK(vm.error.empty());
    }
}

TEST(variable_10026_single_parent) {
    const th13::i32 reference = th13::float_to_bits(10026.0f);
    std::vector<th13::u8> bytes;
    th13test::push_insn(bytes, 402, 0, 0x1, {reference, 0});
    th13test::push_insn(bytes, 2, 0);
    auto script = th13test::make_script(std::move(bytes));
    th13::AnmVm parent, vm;
    CHECK(parent.bind(script));
    vm.bind(script);
    vm.rotation_parent = &parent;
    parent.variables.rotation.z = 1.0f;
    vm.variables.rotation.z = 0.5f;
    CHECK_EQ(vm.tick(rng), 0);
    CHECK_NEAR(vm.visual.scale.x, 1.5, 1e-6);
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
    std::vector<th13::u8> bytes;
    th13test::push_insn(bytes, 402, 0, 0, {0x40000000, 0x40000000});
    th13test::push_insn(bytes, 402, 1, 0, {0x40A00000, 0x40A00000});
    th13test::push_insn(bytes, 2, 2);
    auto script = th13test::make_script(std::move(bytes));
    th13::AnmVm vm;
    CHECK(vm.bind(script));
    vm.visual.render_flags |= 0x100;
    CHECK_EQ(vm.tick(rng, 0.25f), 0);
    CHECK_EQ(vm.timer.current, 1);
    CHECK_EQ(vm.tick(rng, 0.25f), 0);
    CHECK_NEAR(vm.visual.scale.x, 5.0, 1e-6);
    th13::AnmVm plain;
    plain.bind(script);
    CHECK_EQ(plain.tick(rng, 0.25f), 0);
    CHECK_EQ(plain.timer.current, 0);
}

}

int main() { return th13test::run_all("anm-interpreter"); }
