#pragma once
#include "SpellCard.hpp"
#include "Dialogue.hpp"
namespace th15 {
struct StageDefinition {
    i32 id=0;const char* background=nullptr;const char* script=nullptr;
    std::array<const char*,2> music{};std::array<const char*,4> dialogue{};const char* logo=nullptr;
    std::array<i32,2> music_unlock{};std::array<SpellBackground,3> spell_backgrounds{};
    std::array<DialoguePortrait,3> portraits{},names{};
    std::array<i32,2> hud_banners{{-1,-1}};
    DialogueResources dialogue_resources(i32 character,i32 text,i32 player,i32 logo,i32 balloons,const std::array<i32,6>& banks)const noexcept;
    SpellVisualResources spell_visuals(i32 age,i32 ascii,i32 name,i32 effect,const std::array<i32,6>& banks)const noexcept;
};
const StageDefinition* stage_definition(u32 stage)noexcept;
}
