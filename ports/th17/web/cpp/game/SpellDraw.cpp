#include "SpellDraw.hpp"
#include <cstdio>
namespace th17 {
bool draw_spell_card(SpellCard& spell,AnmManager& animations,const SpellRecords& records,i32 character,i32 animal,u32 mode_flags,HudDrawServices& output,std::string& error){
    const auto& state=spell.state();if(!(state.flags&1))return true;
    auto* banner=animations.registry.find(spell.handles[3]);if(!banner){spell.handles[3]=0;return true;}
    if(u32(character)>=3||u32(animal)>=3||u32(state.id)>=SpellRecords::spell_count){error="Spell draw record outside range";return false;}
    HudTextDraw text;text.position={266,35,0};text.style.font=text.style.coordinate_space=2;text.style.color=(text.style.color&0xffffff)|(banner->visual.color&0xff000000);
    char value[64];if(state.flags&2){std::snprintf(value,sizeof(value),"%8d",state.bonus);text.text=value;}else{text.position.x=282;text.text="$";}
    if(!output.hud_text(text)){error="Spell bonus caption submission failed";return false;}
    const auto& record=records.loadouts[u32(character*3+animal)][u32(state.id)];const u32 index=(mode_flags>>4&3)==2?1:0;const i32 captures=record.captures[index],attempts=record.attempts[index];
    text.position={360,35,0};if(captures>=100)text.text="MASTER";else{if(attempts>=100)std::snprintf(value,sizeof(value),"%.2d/99+",captures);else std::snprintf(value,sizeof(value),"%.2d/%.2d",captures,attempts);text.text=value;}
    if(!output.hud_text(text)){error="Spell history caption submission failed";return false;}return true;
}
}
