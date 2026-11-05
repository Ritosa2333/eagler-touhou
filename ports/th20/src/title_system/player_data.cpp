#include "player_data.hpp"
#include "practice_data.hpp"
#include "../pause_system/menu_support.hpp"
#include <algorithm>
namespace th20::source::title {
int card_count_by_difficulty(int difficulty){return int(std::count(std::begin(practice_data::difficulties),std::end(practice_data::difficulties),difficulty));}
int update_player_data_menu(TitleInf& o,SelectionEnvironment& e){auto& m=e.main;auto& c=o.cursor;constexpr int index=78;
    switch(o.phase){
    case 0:
        if(!o.handle390)e.spawn_text_overlay(o);c.count=3;c.wrapping=1;m.interrupt(o,index,1,true);m.spawn(o,index);m.interrupt(o,index,3,false);e.signal(o,index,static_cast<short>(unsigned(c.current)+7),false);m.spawn(o,46);set_phase(o,1);[[fallthrough]];
    case 1:if(o.age.current>6)set_phase(o,2);break;
    case 2:
        c.snapshot();if(m.repeated(0x10)||m.repeated(0x40))c.move(-1);if(m.repeated(0x20)||m.repeated(0x80))c.move(1);
        if(c.changed()){m.sound(10);m.interrupt(o,index,3,false);e.signal(o,index,static_cast<short>(unsigned(c.current)+7),false);}
        if(m.pressed(0x106)){set_phase(o,4);m.sound(9);m.interrupt(o,index,1,true);}
        else if(m.pressed(0x80001)){e.signal(o,index,6,false);e.child_signal(o,index,m.session.player_table.field_1e0<4?c.current+48:52,2,false);set_phase(o,3);m.sound(7);}break;
    case 3:if(o.age.current>=14){m.interrupt(o,46,1,true);set_state(o,c.current==2?23:11);pause::save_cursor(c);}break;
    case 4:if(o.age.current>=6){m.interrupt(o,46,1,true);set_state(o,1);m.interrupt_handle(o.handle390,1);o.handle390=0;set_state(o,1);pause::restore_cursor(c);}break;
    }return 1;
}
int update_player_data_detail(TitleInf& o,DataEnvironment& e){auto& s=e.selection;auto& m=s.main;auto& c=o.cursor;auto& d=o.cursorbc;auto& page=o.cursor108;
    switch(o.phase){
    case 0:
        e.open_stones();e.select_stone(0);o.words58d8[8]=unsigned(c.current);
        if(o.words58d8[8]==1){page.count=(card_count_by_difficulty(d.current)+9)/10;page.select(0);page.wrapping=1;}
        c.count=16;c.select(0);d.count=5;d.select(1);d.wrapping=1;set_phase(o,1);m.spawn(o,72);m.spawn(o,d.current+64);for(int i:{73,74,75,76,69,70,71,39})m.spawn(o,i);[[fallthrough]];
    case 1:if(o.age.current>6)set_phase(o,2);break;
    case 2:
        c.snapshot();d.snapshot();page.snapshot();
        if(m.repeated(0x10)){d.move(-1);m.interrupt(o,75,2,false);}if(m.repeated(0x20)){d.move(1);m.interrupt(o,76,2,false);}
        if(d.changed()){m.sound(10);m.interrupt(o,d.previous+64,1,true);m.spawn(o,d.current+64);if(o.words58d8[8]==1)page.select(0);page.count=(card_count_by_difficulty(d.current)+9)/10;}
        if(m.repeated(0x40)){c.move(-1);m.interrupt(o,73,2,false);}if(m.repeated(0x80)){c.move(1);m.interrupt(o,74,2,false);}
        if(c.changed()){m.sound(10);e.select_stone(c.current%8);s.signal(o,72,c.current/2+37,true);recovered::timer_set(o.age,0);}
        if(o.words58d8[8]==1&&m.pressed(0x80001)){page.move(1);m.sound(7);}
        if(d.current==3&&c.current==7)e.update_unlock_sequence();
        if(m.pressed(0x106)){set_phase(o,3);m.sound(9);m.interrupt(o,d.current+64,1,true);for(int i:{73,74,75,76,69,70,71,77,72})m.interrupt(o,i,1,true);for(int i=0;i<10;++i)m.interrupt_handle(o.handles394[i],1);}break;
    case 3:if(o.age.current>=6){e.hide_stones();m.interrupt(o,39,1,true);set_state(o,10);pause::restore_cursor(c);}break;
    }return 1;
}
}
