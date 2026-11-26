// 对话共享状态对应A73058..A73117；尚未确认语义的字段保留原字节。
#pragma once
#include "core/shared/ObjectRecord.hpp"

namespace kaikeidou {
struct DialogueState {
    ObjectRecord<192> fields;
    std::uint32_t word_a7e7fc = 0;
    std::uint32_t word_a78e30 = 0;
};

// 恢复644AC0..644BA4，按原顺序初始化22个字段，其余字节保持原值。
inline void initialize_dialogue(DialogueState& state, std::uint32_t mode) noexcept {
    auto& f = state.fields;
    f.word(0x00, mode);
    f.word(0x08, 1);
    f.word(0x04, 0);
    f.word(0x0c, 0);
    f.word(0x10, 0);
    f.word(0x14, 0xffffffffu);
    f.word(0x2c, 0);
    f.quad(0x48, 0);
    f.quad(0x50, 0x4075c00000000000ull);
    f.quad(0x78, 0x408f380000000000ull);
    f.word(0x18, 0xffffffffu);
    f.word(0x1c, 0xffffffffu);
    f.word(0x20, 0xffffffffu);
    f.word(0x38, 0);
    f.quad(0x80, 0x4078000000000000ull);
    f.quad(0x88, 0x4079800000000000ull);
    f.quad(0xb0, 0x408f380000000000ull);
    f.word(0x24, 0xffffffffu);
    f.word(0x28, 0xffffffffu);
    f.quad(0xb8, 0x3ff0000000000000ull);
    state.word_a7e7fc = 0;
    state.word_a78e30 = 0;
}
}
