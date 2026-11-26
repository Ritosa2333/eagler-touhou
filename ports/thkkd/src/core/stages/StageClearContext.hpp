// 通关奖励读取的原全局；保存数组按原难度、角色、武器、关卡顺序索引。
#pragma once
#include <cstdint>
namespace kaikeidou {
struct StageClearContext {
    std::uint32_t stage = 0;          // B3E2EC
    std::uint32_t difficulty = 0;     // A7E610
    std::uint32_t character = 0;      // A74C0C
    std::uint32_t weapon = 0;         // A7E614
    std::uint32_t practice_mode = 0;  // A7E618
};
}
