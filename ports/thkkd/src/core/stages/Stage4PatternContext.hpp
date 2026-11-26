// 原A785A8..A785F4的整数槽20..39，保留两组索引数组之间的原字段。
#pragma once
#include <array>
#include <cstdint>
namespace kaikeidou {
struct Stage4PatternContext {
    std::array<std::uint32_t,20> integers_20_to_39{};
};
}
