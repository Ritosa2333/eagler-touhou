// 道具池为 100 个 64 字节槽；0x00657e00 只查找首个空槽，不修改槽位或游标。
#pragma once
#include "core/shared/ObjectRecord.hpp"

namespace kaikeidou {
using Item = ObjectRecord<64>;
struct ItemPool { std::array<Item, 100> items{}; };
inline std::int32_t find_free_item(const ItemPool& pool) noexcept {
    for (std::size_t index = 0; index < pool.items.size(); ++index)
        if (pool.items[index].word(0) == 0) return static_cast<std::int32_t>(index);
    return -1;
}
}
