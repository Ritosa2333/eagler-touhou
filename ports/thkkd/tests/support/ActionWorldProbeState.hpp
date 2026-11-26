// 在既有46区后追加炸弹池和关卡参数，字节顺序对应action_world_layout.py。
#pragma once
#include "core/player/PlayerBombPool.hpp"
#include "core/stages/StagePatternContext.hpp"
#include <iostream>
namespace kaikeidou {
struct ActionWorldProbeState {
    PlayerBombPool bombs;
    StagePatternContext stage;
    void read(std::istream& input) {
        const auto raw = [&](auto& value) { input.read(reinterpret_cast<char*>(&value), sizeof(value)); };
        raw(bombs.bombs); raw(stage.integers_6_to_19); raw(stage.real_4); raw(stage.reals_10_to_19); raw(stage.boss_hp_multiplier);
        if (!input) throw std::runtime_error("Action world input truncated");
    }
    void write(std::ostream& output) const {
        const auto raw = [&](const auto& value) { output.write(reinterpret_cast<const char*>(&value), sizeof(value)); };
        raw(bombs.bombs); raw(stage.integers_6_to_19); raw(stage.real_4); raw(stage.reals_10_to_19); raw(stage.boss_hp_multiplier);
    }
};
}
