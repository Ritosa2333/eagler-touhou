#include "selection.hpp"
#include "../pause_system/menu_support.hpp"
namespace th20::source::title {
void update_character(TitleInf& o,SelectionEnvironment& e){
    auto& m=e.main;auto& s=m.session;auto& c=o.cursor;const int difficulty=static_cast<int>(s.player_table.field_1e0),index=difficulty==4?14:12;
    auto signal=[&](int event,bool execute=false){e.signal(o,index,static_cast<short>(event),execute);};
    auto confirm=[&]{for(int child:{c.current+6,c.current+8,4,5})e.child_signal(o,index,child,6,false);};
    auto finish=[&]{
        e.commit_progress();set_state(o,7);set_character(s,c.current);m.last_character=static_cast<int>(s.contexts[0].current_player->fields_00[2]);pause::save_cursor(c);c.wrapping=1;c.count=9;c.select(e.last_profile);
        for(int slot=0;slot<4;++slot){const int character=static_cast<int>(s.contexts[0].current_player->fields_00[2]);const int profile=e.selected_profile(slot,character);e.set_weapon(slot,character,profile);}
        e.child_visible(o,index,15,false);e.child_visible(o,index,16,false);signal(6);m.interrupt(o,35,1,true);
    };
    switch(o.phase){
    case 0:
        c.count=2;
        if(difficulty==4){if(!e.character_extra_unlocked(c.current))for(int character=0;character<2;++character)if(e.character_extra_unlocked(character)){c.select(character);break;}for(int character=0;character<2;++character)if(!e.character_extra_unlocked(character))pause::exclude(c,character);}
        if(!m.exists(o.handles[35]))m.spawn(o,35);
        if(!m.exists(o.handles[index])){m.interrupt(o,index,1,true);m.spawn(o,index);}
        m.interrupt(o,index,3,false);signal(c.current+7);set_phase(o,1);
        e.child_visible(o,index,15,e.all_cleared(difficulty,0));e.child_visible(o,index,16,e.all_cleared(difficulty,1));
        if(e.menu_selection==4||e.menu_selection==5){c.select(static_cast<int>(s.contexts[0].current_player->fields_00[2]));signal(c.current+7,true);confirm();finish();break;}
        [[fallthrough]];
    case 1:if(o.age.current>6){set_phase(o,2);e.child_signal(o,index,6,e.selected_profile(0,0)/2+37,true);e.child_signal(o,index,7,e.selected_profile(0,1)/2+37,true);}break;
    case 2:
        c.snapshot();if(m.repeated(0x40)){m.sound(10);signal(c.current+25,true);c.move(-1);signal(c.current+13);}if(m.repeated(0x80)){m.sound(10);signal(c.current+19,true);c.move(1);signal(c.current+7);}
        if(m.pressed(0x80001)){confirm();m.sound(7);set_phase(o,3);}else if(m.pressed(0x106)){set_phase(o,4);m.sound(9);}break;
    case 3:if(o.age.current>=14)finish();break;
    case 4:if(o.age.current>=6){e.commit_progress();m.interrupt(o,index,1,true);m.interrupt(o,35,1,true);set_state(o,s.mode==2?1:5);set_character(s,c.current);pause::restore_cursor(c);m.last_character=static_cast<int>(s.contexts[0].current_player->fields_00[2]);}break;
    }
}
}
