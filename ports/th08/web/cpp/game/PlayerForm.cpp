#include "PlayerForm.hpp"
namespace th08 {
void update_player_form(PlayerFormState& s,PlayerMovementState& movement,PlayerOption* options,u8 character,u16 buttons,i32 bomb,i32 bomb_type,bool always_hitbox,PlayerFormActions& actions){
    const u8 focused=bomb?(bomb_type&1)!=0:(buttons&4)!=0;
    if(s.focused==focused)s.frames=wrapping_add(s.frames,1);
    else{
        if(focused){if(character<4)PlayerOptions::initialize(options,character);}
        else if(character<4){
            for(u32 i=0;i<(character==3?2u:4u);++i){auto& option=options[i];if(option.state&&option.state!=3){option.state=3;option.timer.set(0);}}
            if(character==3){auto& option=options[2];std::memset(&option,0,sizeof(option));option.update=PlayerOptionKind::YoumuTeam;option.draw=1;option.state=1;option.timer.set(0);option.index=2;for(auto& position:movement.history)position=movement.position;}
        }
        if(character<4){actions.animation(focused?5:0);movement.delta.x=0;if(s.frames>=4)actions.effect(focused?29:28,movement.position,focused?0x80ff8080:0x808080ff);}
        s.frames=0;s.transition.set(0);
    }
    const bool show_hitbox=focused||always_hitbox;
    if(show_hitbox){if(!s.focus_effect)s.focus_effect=actions.focus_effect(movement.position);}
    else{if(s.focus_effect)s.focus_effect->SetInterrupt(1);s.focus_effect=nullptr;}
    if(s.frames>=7)s.youkai=focused;s.focused=focused;if(character>=4)s.youkai=character&1;
}
}
