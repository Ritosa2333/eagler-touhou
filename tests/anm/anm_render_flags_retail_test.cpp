// Independent 00462450 executions are retained in
// artifacts/th13-longrun-20261029/anm-render-flags-native.json.
#include "anm_test_fixtures.hpp"

using namespace th13;

TEST(retail_render_flag_opcodes_use_th13_bit_positions) {
    constexpr i32 opcodes[] = {307, 311, 424, 431, 432};
    constexpr u32 masks[] = {0x200, 0x400, 0x40, 0x80, 0x100};
    GameRandom random;
    for (u32 i = 0; i < 5; ++i) {
        std::vector<u8> bytes;
        th13test::push_insn(bytes, i16(opcodes[i]), 0, 0, {1});
        th13test::push_insn(bytes, 2, 0);
        auto script = th13test::make_script(std::move(bytes));
        AnmVm vm; CHECK(vm.bind(script)); vm.visual.render_flags = 0x4000;
        CHECK_EQ(vm.tick(random), 0);
        CHECK_EQ(vm.visual.render_flags, 0x4000u | masks[i]);
    }
}

TEST(ins424_does_not_recycle_an_active_registry_node) {
    std::vector<u8> bytes;
    th13test::push_insn(bytes, 424, 0, 0, {1});
    th13test::push_insn(bytes, 100, 0, 1, {10000, 7});
    th13test::push_insn(bytes, 3, 0);
    auto script = th13test::make_script(std::move(bytes));
    AnmRegistry registry;GameRandom random;
    auto* vm = registry.allocate(); CHECK(vm->bind(script));
    const u32 handle = registry.submit(*vm);
    CHECK(registry.update(random, 1));
    CHECK(registry.find(handle) == vm);
    CHECK_EQ(vm->variables.integers[0], 7);
    CHECK(vm->visual.render_flags & 0x40u);
    CHECK(registry.update(random, 1));
    CHECK(registry.find(handle) == vm);
    CHECK_EQ(registry.count(), 1u);
    CHECK_EQ(registry.layer(0).size(), size_t(1));
}

int main() { return th13test::run_all("retail ANM render flags"); }
