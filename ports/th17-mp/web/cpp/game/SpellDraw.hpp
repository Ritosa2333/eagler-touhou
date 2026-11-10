#pragma once
#include "SpellCard.hpp"
#include "AnmManager.hpp"
#include "HudDrawData.hpp"
namespace th17 {
bool draw_spell_card(SpellCard&,AnmManager&,const SpellRecords&,i32 character,i32 animal,u32 mode_flags,HudDrawServices&,std::string& error);
}
