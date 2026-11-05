#include "SpellDraw.hpp"
namespace th15 {
namespace {
std::string digits(i32 value,u32 minimum){auto text=std::to_string(value);const auto sign=value<0?1u:0u;if(text.size()-sign<minimum)text.insert(sign,minimum-(text.size()-sign),'0');return text;}
}
std::string spell_history_caption(i32 captures,i32 attempts,bool complete){return !complete&&captures>=100?"MASTER":digits(captures,2)+"/"+(!complete&&attempts>=100?"99+":digits(attempts,2));}
bool draw_spell_card(SpellCard& spell,AnmManager& animations,const RecordStore& records,SpellDrawContext& context,HudDrawServices& services,std::string& error){
 const auto& state=spell.state();if(!(state.flags&1))return true;
 auto* banner=animations.registry.find(spell.handles[3]);if(!banner){spell.handles[3]=0;return true;}
 const i32 character=wrapping_add(context.character,context.subcharacter);if(character<0||character>=5||spell.identifier<0||spell.identifier>=119){error="Spell draw record selection outside range";return false;}
 auto& style=context.style;style.font=2;style.coordinate_space=context.disable_master_display?0:2;style.color=(style.color&0xffffffu)|(banner->visual.color&0xff000000u);
 struct Restore {HudTextStyle& style;~Restore(){style.font=0;style.coordinate_space=0;style.color|=0xff000000u;}} restore{style};
 HudTextDraw text;text.style=style;text.position={266,35,0};
 if(state.flags&2){text.text=std::to_string(state.bonus);if(text.text.size()<8)text.text.insert(0,8-text.text.size(),' ');}else{text.position.x=282;text.text="$";}
 if(!services.hud_text(text)){error="Spell bonus caption submission failed";return false;}
 const auto& record=records.characters[character].modes[(context.mode_flags&0x300)==0?1:0].spells[spell.identifier];const auto index=(context.mode_flags&0x30)==0x20?1u:0u;
 const auto captures=record.captures[index],attempts=record.attempts[index];text.position={360,35,0};
 // Purple bypasses both >=100 branches and sets the caption's coordinate
 // space to zero (420296). It does not truncate either history count.
 text.text=spell_history_caption(captures,attempts,context.disable_master_display);
 if(!services.hud_text(text)){error="Spell history caption submission failed";return false;}return true;
}
}
