// 将原符卡统计地址映射到两张存档表，保留难度5分支提前0x4c8字节的原基址。
#pragma once
#include "core/frontend/SaveData.hpp"
#include <stdexcept>

namespace kaikeidou {
inline std::uint32_t& spell_record_word(SaveData& data, std::uint32_t address) {
    constexpr std::uint32_t base = 0xa7f74c;
    if (address < base || (address & 3u)) throw std::out_of_range("Spell record address outside word view");
    auto index = static_cast<std::size_t>((address - base) / 4u);
    if (index < data.spells.size()) return data.spells[index];
    index -= data.spells.size();
    if (index < data.extra_spells.size()) return data.extra_spells[index];
    throw std::out_of_range("Spell record address outside save tables");
}
}
