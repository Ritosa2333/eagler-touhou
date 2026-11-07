#pragma once
#include "SpellCatalog.hpp"
#include <string>
#include <string_view>
#include <unordered_map>

namespace th17 {
bool valid_utf8(std::string_view text);
// The launcher's ETL1 format stores UTF-8 values by numeric ID and row.
class LocalizationTable {
    std::unordered_map<u64, std::string> entries;
public:
    bool open(const u8* bytes, size_t size);
    const std::string* find(u32 id, u16 row = 0) const;
    const std::string* spell_name(i32 id) const;
};
}
