#include "StageDefinition.hpp"
namespace th15 {
DialogueResources StageDefinition::dialogue_resources(i32 character,i32 text,i32 player,i32 logo,i32 balloons,const std::array<i32,6>& banks)const noexcept{
    DialogueResources out;out.character=character;out.text=text;out.player=player;out.logo=logo;out.balloons=balloons;auto mapped=[&](DialoguePortrait p){p.resource=p.resource>=0&&p.resource<i32(banks.size())?banks[u32(p.resource)]:-1;return p;};for(u32 i=0;i<3;i++){out.enemies[i]=mapped(portraits[i]);out.names[i]=mapped(names[i]);}return out;
}
namespace {const std::array<StageDefinition,8> definitions{{
#include "StageDefinitions.inc"
}};}
const StageDefinition* stage_definition(u32 stage)noexcept{return stage<definitions.size()?&definitions[stage]:nullptr;}
SpellVisualResources StageDefinition::spell_visuals(i32 age,i32 ascii,i32 name,i32 effect,const std::array<i32,6>& banks)const noexcept{
    SpellVisualResources result;result.ascii=ascii;result.name=name;result.effect=effect;result.secondary=age<43&&spell_backgrounds[1].resource!=-1;
    const auto resolve=[&](i32 alias){return alias>=0&&alias<i32(banks.size())?banks[u32(alias)]:-1;};
    for(u32 i=0;i<3;i++){result.backgrounds[i]=spell_backgrounds[i];result.backgrounds[i].resource=resolve(spell_backgrounds[i].resource);result.backgrounds[i].overlay_resource=resolve(spell_backgrounds[i].overlay_resource);}return result;
}
}
