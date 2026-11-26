// 恢复 0x0065ca60..0x0065cb41 的 50 槽效果分配，复用时保留其余旧字节。
#pragma once
#include "core/shared/ObjectRecord.hpp"

namespace kaikeidou {
using Effect = ObjectRecord<112>;
struct EffectPool { std::array<Effect, 50> effects{}; };
struct EffectAllocation {
    std::uint32_t kind = 0, color = 0;
    std::uint64_t x = 0, y = 0;
};
inline std::int32_t allocate_effect(EffectPool& pool, std::array<std::uint32_t, 100>& sounds, const EffectAllocation& input) {
    for (std::size_t index = 0; index < pool.effects.size(); ++index) {
        auto& effect = pool.effects[index];
        if (effect.word(0) != 0) continue;
        effect.word(0, 1); effect.word(4, 0);
        effect.quad(0x60, input.x); effect.quad(0x68, input.y);
        effect.word(0xc, input.kind); effect.word(8, input.color);
        if (input.kind == 0) sounds[21] = 1;
        else if (input.kind == 1) sounds[30] = 1;
        else if (input.kind == 10 || input.kind == 12 || input.kind == 13 || input.kind == 15) sounds[23] = 1;
        return static_cast<std::int32_t>(index);
    }
    return -1;
}
}
