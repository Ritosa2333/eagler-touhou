#include "../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/GameKeyboard.hpp"

#include <cstring>
#include <utility>

using namespace th13;

TEST(pressed_released_edges) {
    GameInput input;
    input.update(1);
    CHECK(input.pressed & 1);
    CHECK(!(input.released & 1));
    CHECK(input.held & 1);
    input.update(1);
    CHECK(!(input.pressed & 1));
    CHECK(!(input.released & 1));
    input.update(0);
    CHECK(input.released & 1);
    CHECK(!(input.held & 1));
    CHECK(!(input.pressed & 1));
}

TEST(repeat_first_at_26_then_every_8) {
    GameInput input;
    for (int frame = 1; frame <= 25; frame++) {
        input.update(1);
        CHECK(!(input.repeated & 1));
    }
    input.update(1);
    CHECK(input.repeated & 1);
    for (int frame = 0; frame < 7; frame++) {
        input.update(1);
        CHECK(!(input.repeated & 1));
    }
    input.update(1);
    CHECK(input.repeated & 1);
    CHECK(input.long_held & 1);
}

TEST(repeat_reset_on_release) {
    GameInput input;
    for (int frame = 0; frame < 30; frame++)
        input.update(1);
    input.update(0);
    CHECK_EQ(input.duration[0], 0);
    input.update(1);
    for (int frame = 0; frame < 24; frame++) {
        input.update(1);
        CHECK(!(input.repeated & 1));
    }
    input.update(1);
    CHECK(input.repeated & 1);
}

TEST(keyboard_mask_basic_bits) {
    bool keys[256]{};
    auto mask = [&]() { return keyboard_keys(keys); };
    CHECK_EQ(mask(), 0u);
    keys[90] = true;
    CHECK_EQ(mask(), 1u);
    keys[88] = true;
    CHECK_EQ(mask(), 1u | 2u);
    keys[16] = true;
    CHECK_EQ(mask() & 8u, 8u);
    std::memset(keys, 0, sizeof keys);
    keys[38] = keys[104] = true;
    CHECK_EQ(mask(), 0x10u);
    keys[40] = keys[98] = true;
    CHECK_EQ(mask(), 0x10u | 0x20u);
    keys[37] = keys[100] = true;
    keys[39] = keys[102] = true;
    CHECK_EQ(mask(), 0xf0u);
    std::memset(keys, 0, sizeof keys);
    keys[97] = true;
    CHECK_EQ(mask(), 0x60u);
    keys[99] = true;
    CHECK_EQ(mask(), 0x60u | 0xa0u);
    keys[103] = keys[105] = true;
    CHECK_EQ(mask(), 0x60u | 0xa0u | 0x50u | 0x90u);
}

TEST(keyboard_system_bits) {
    bool keys[256]{};
    keys[27] = true;
    CHECK_EQ(keyboard_keys(keys), 0x100u);
    std::memset(keys, 0, sizeof keys);
    keys[17] = true;
    CHECK_EQ(keyboard_keys(keys), 0x200u);
    std::memset(keys, 0, sizeof keys);
    keys[13] = true;
    CHECK_EQ(keyboard_keys(keys), 0x80000u);
}

TEST(keyboard_original_extended_bits_match_471620) {
    constexpr std::pair<int, u32> expected[] = {
        {81, 0x10000u}, {83, 0x20000u},  {36, 0x40000u},  {80, 0x40000u},
        {67, 0xa00u},   {68, 0x100000u}, {82, 0x200000u}, {121, 0x800000u},
    };
    for (const auto& binding : expected) {
        bool keys[256]{};
        keys[binding.first] = true;
        CHECK_EQ(keyboard_keys(keys), binding.second);
    }
    bool keys[256]{};
    keys[81] = keys[83] = keys[67] = keys[68] = keys[82] = true;
    keys[36] = keys[80] = keys[121] = true;
    CHECK_EQ(keyboard_keys(keys), 0x10000u | 0x20000u | 0xa00u | 0x100000u | 0x200000u | 0x40000u | 0x800000u);
}

TEST(keyboard_unmapped_keys_stay_zero) {
    bool keys[256]{};
    keys[65] = keys[66] = keys[112] = keys[9] = keys[32] = true;
    CHECK_EQ(keyboard_keys(keys), 0u);
}

int main() { return th13test::run_all("输入映射"); }
