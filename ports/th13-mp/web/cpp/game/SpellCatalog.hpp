// Runtime spell metadata comes from the same project ECL assets as gameplay.
// No copied original spell-name or stage/variant directory is embedded here.
#pragma once

#include "Types.hpp"

#include <array>
#include <functional>
#include <string>
#include <vector>

namespace th13 {

struct SpellCatalogVariant {
    i32 id = -1;
    i32 rank = -1;  // intrinsic DAT_004bbd30 rank, including Extra=4/Overdrive=5
    std::string name; // original Shift-JIS bytes, as passed to spellStart
};

class SpellCatalog {
public:
    using Reader = std::function<bool(const std::string&, std::vector<u8>&)>;
    using Check = std::function<bool(i32)>;

    bool load(const Reader& reader);
    bool load_stage(i32 stage, const Reader& reader);
    i32 spell_count(i32 stage) const;
    // Menu rows: ordinary E/N/H/L/OD=0..4; Extra/OD=0..1.
    const SpellCatalogVariant* variant(i32 stage, i32 slot, i32 difficulty) const;
    bool spell_unlocked(i32 stage, i32 slot, const Check& encountered) const;
    bool variant_available(i32 stage, i32 slot, i32 difficulty,
                           const Check& encountered, const Check& captured) const;
    std::string spell_name(i32 stage, i32 slot, const Check& encountered) const;
    static i32 difficulty(i32 id);
    std::string error;

private:
    using Family = std::array<SpellCatalogVariant, 5>;
    std::array<std::vector<Family>, 7> stages_;
    const Family* family(i32 stage, i32 slot) const;
};

} // namespace th13
