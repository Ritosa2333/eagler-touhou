#pragma once
#include "Types.hpp"
#include <array>
#include <memory>
#include <string>
namespace th17 {
namespace multiplayer { class FrameImage; }
struct SpellRecord {std::string name;std::array<i32,2> captures{},attempts{};};
class SpellRecords {
    struct Checkpoint;
    std::weak_ptr<const Checkpoint> rollback_record;
public:
    static constexpr u32 spell_count=101,loadout_count=9;
    std::array<std::array<SpellRecord,spell_count>,loadout_count> loadouts;
    std::array<SpellRecord,spell_count> aggregate;
    void invalidate_rollback()noexcept{rollback_record.reset();}
    void save_rollback(multiplayer::FrameImage&);
    bool begin(i32 spell,i32 loadout,bool practice,const std::string& name);
    bool capture(i32 spell,i32 loadout,bool practice,bool& all_captured);
};
}
