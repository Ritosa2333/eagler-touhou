// Boss分配器新增字段；已有Boss索引、剩余符卡与保护状态继续由各原Context共同持有。
#pragma once
#include <array>
#include <cstdint>

namespace kaikeidou {
struct BossAllocationContext {
    std::uint32_t field_a7ea44 = 0, field_a7ea48 = 0, field_a7ea50 = 0;
    std::uint32_t field_a7ea58 = 0, field_a7ea5c = 0;
    std::array<std::uint32_t, 8> first_sprite_handles{};
};
}
