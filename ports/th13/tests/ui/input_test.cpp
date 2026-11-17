// 输入映射单测：GameInput 边沿与重复参数（首重复 26 帧、此后每 8 帧）、
// GameKeyboard 位掩码表（ui-base.md §六接入位 + 不接入位为零）。
#include "../harness/test_framework.hpp"

#include "game/GameInput.hpp"
#include "game/GameKeyboard.hpp"

#include <cstring>
#include <utility>

using namespace th13;

TEST(pressed_released_edges) {
    GameInput input;
    input.update(1);  // Z 按下
    CHECK(input.pressed & 1);
    CHECK(!(input.released & 1));
    CHECK(input.held & 1);
    input.update(1);  // 保持：无新边沿
    CHECK(!(input.pressed & 1));
    CHECK(!(input.released & 1));
    input.update(0);  // 松开
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
    input.update(1);  // 第 26 帧：首次重复
    CHECK(input.repeated & 1);
    for (int frame = 0; frame < 7; frame++) {
        input.update(1);
        CHECK(!(input.repeated & 1));
    }
    input.update(1);  // 之后每 8 帧
    CHECK(input.repeated & 1);
    CHECK(input.long_held & 1);  // ≥8 帧即长按
}

TEST(repeat_reset_on_release) {
    GameInput input;
    for (int frame = 0; frame < 30; frame++) input.update(1);
    input.update(0);
    CHECK_EQ(input.duration[0], 0);
    input.update(1);  // 重新按下：重复计数从零重计
    for (int frame = 0; frame < 24; frame++) {
        input.update(1);
        CHECK(!(input.repeated & 1));  // 第 2..25 帧不重复
    }
    input.update(1);  // 第 26 帧
    CHECK(input.repeated & 1);
}

TEST(keyboard_mask_basic_bits) {
    bool keys[256]{};
    auto mask = [&]() { return keyboard_keys(keys); };
    CHECK_EQ(mask(), 0u);
    keys[90] = true;  // Z
    CHECK_EQ(mask(), 1u);
    keys[88] = true;  // X
    CHECK_EQ(mask(), 1u | 2u);
    keys[16] = true;  // Shift（KeyboardMap 层已把左右 Shift 归一为 vk16）
    CHECK_EQ(mask() & 8u, 8u);
    std::memset(keys, 0, sizeof keys);
    keys[38] = keys[104] = true;  // ↑ + 小键盘 8
    CHECK_EQ(mask(), 0x10u);
    keys[40] = keys[98] = true;   // ↓ + 小键盘 2
    CHECK_EQ(mask(), 0x10u | 0x20u);
    keys[37] = keys[100] = true;  // ← + 小键盘 4
    keys[39] = keys[102] = true;  // → + 小键盘 6
    CHECK_EQ(mask(), 0xf0u);
    std::memset(keys, 0, sizeof keys);
    keys[97] = true;   // 小键盘 7：左上
    CHECK_EQ(mask(), 0x60u);
    keys[99] = true;   // 小键盘 1：左下
    CHECK_EQ(mask(), 0x60u | 0xa0u);
    keys[103] = keys[105] = true;  // 小键盘 9/3：右上/右下
    CHECK_EQ(mask(), 0x60u | 0xa0u | 0x50u | 0x90u);
}

TEST(keyboard_system_bits) {
    bool keys[256]{};
    keys[27] = true;  // Esc
    CHECK_EQ(keyboard_keys(keys), 0x100u);
    std::memset(keys, 0, sizeof keys);
    keys[17] = true;  // Ctrl
    CHECK_EQ(keyboard_keys(keys), 0x200u);
    std::memset(keys, 0, sizeof keys);
    keys[13] = true;  // Enter
    CHECK_EQ(keyboard_keys(keys), 0x80000u);
}

TEST(keyboard_original_extended_bits_match_471620) {
    // 原版 0x471620 的 GetKeyboardState 掩码构造（0x47165f..0x471805）逐键
    // 读 vk 字节并移位拼位；下面每个 (vk, bit) 与反汇编逐一对应：
    //   0x79 F10→0x800000、0x52 R→0x200000、0x44 D→0x100000、
    //   0x0d Enter→0x80000、0x50 P / 0x24 Home→0x40000、
    //   0x51 Q→0x10000、0x53 S→0x20000、0x43 C→0xa00（0x800|0x200）、
    //   0x11 Ctrl→0x200、0x1b Esc→0x100、0x5a Z→0x1、0x58 X→0x2。
    constexpr std::pair<int, u32> expected[] = {
        {81, 0x10000u},   // Q
        {83, 0x20000u},   // S
        {36, 0x40000u},   // Home
        {80, 0x40000u},   // P
        {67, 0xa00u},     // C：原版同时置 0x800 与 0x200
        {68, 0x100000u},  // D
        {82, 0x200000u},  // R
        {121, 0x800000u}, // F10
    };
    for (const auto& binding : expected) {
        bool keys[256]{};
        keys[binding.first] = true;
        CHECK_EQ(keyboard_keys(keys), binding.second);
    }
    // 组合位按位或（0x47165f..0x471805）。
    bool keys[256]{};
    keys[81] = keys[83] = keys[67] = keys[68] = keys[82] = true;
    keys[36] = keys[80] = keys[121] = true;
    CHECK_EQ(keyboard_keys(keys),
             0x10000u | 0x20000u | 0xa00u | 0x100000u | 0x200000u |
                 0x40000u | 0x800000u);
}

TEST(keyboard_unmapped_keys_stay_zero) {
    // 0x471620 的 vk 列表外无隐式映射（A/B/F1/Tab/Space）。
    bool keys[256]{};
    keys[65] = keys[66] = keys[112] = keys[9] = keys[32] = true;
    CHECK_EQ(keyboard_keys(keys), 0u);
}

int main() { return th13test::run_all("输入映射"); }
