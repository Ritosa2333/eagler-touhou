#include "SpellRecords.hpp"
namespace th17 {
bool SpellRecords::begin(i32 spell,i32 loadout,bool practice,const std::string& name){if(u32(spell)>=spell_count||u32(loadout)>=loadout_count)return false;auto& p=loadouts[loadout][spell];auto& a=aggregate[spell];p.name=name;if(p.attempts[practice]<99999)p.attempts[practice]=wrapping_add(p.attempts[practice],1);a.name=name;if(a.attempts[practice]<99999)a.attempts[practice]=wrapping_add(a.attempts[practice],1);return true;}
bool SpellRecords::capture(i32 spell,i32 loadout,bool practice,bool& all_captured){if(u32(spell)>=spell_count||u32(loadout)>=loadout_count)return false;auto& p=loadouts[loadout][spell];auto& a=aggregate[spell];if(p.captures[practice]<99999)p.captures[practice]=wrapping_add(p.captures[practice],1);if(a.captures[practice]<99999)a.captures[practice]=wrapping_add(a.captures[practice],1);u32 count=0;for(const auto& entry:aggregate)if(entry.captures[0]||entry.captures[1])count++;all_captured=count>100;return true;}
}
