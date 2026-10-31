#pragma once
#include "Types.hpp"
#include <array>
#include <string>
namespace th17 {
struct SpellRecord {std::string name;std::array<i32,2> captures{},attempts{};};
class SpellRecords {
public:
    static constexpr u32 spell_count=101,loadout_count=9;
    std::array<std::array<SpellRecord,spell_count>,loadout_count> loadouts;
    std::array<SpellRecord,spell_count> aggregate;
    bool begin(i32 spell,i32 loadout,bool practice,const std::string& name);
    bool capture(i32 spell,i32 loadout,bool practice,bool& all_captured);
};
}
