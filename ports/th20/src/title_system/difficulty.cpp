#include "selection.hpp"
#include "../pause_system/menu_support.hpp"
namespace th20::source::title {
int update_difficulty(TitleInf& o,SelectionEnvironment& e){
    auto& m=e.main;auto& s=m.session;auto& c=o.cursor;auto difficulty=[&]{return static_cast<int>(s.player_table.field_1e0);};const int index=difficulty()<4?58:59;
    auto signal=[&](int event,bool execute=false){e.signal(o,index,static_cast<short>(event),execute);};
    auto erase=[&](int i){m.interrupt(o,i,1,true);};auto deactivate=[&]{m.interrupt(o,index,3,false);};
    auto finish=[&]{set_state(o,6);if(difficulty()<4){s.player_table.field_1e0=c.current;m.last_difficulty=difficulty();}pause::save_cursor(c);c.wrapping=1;c.count=2;c.select(m.last_character);set_character(s,m.last_character);};
    switch(o.phase){
    case 0:
        if(o.handle390==0)e.spawn_text_overlay(o);c.count=difficulty()<4?4:1;c.wrapping=0;erase(index);m.spawn(o,index);deactivate();signal(c.current+13);
        if(e.menu_selection==4){c.select(difficulty());deactivate();signal(c.current+7,true);signal(6,true);e.child_signal(o,index,c.current+48,2,false);finish();break;}
        m.spawn(o,34);set_phase(o,1);
        if(difficulty()<4){for(int i=0;i<4;++i)if(!e.all_cleared(i,-1))e.child_visible(o,index,i+17,false);}else if(!e.all_cleared(4,-1))e.child_visible(o,index,21,false);
        [[fallthrough]];
    case 1:if(o.age.current>6)set_phase(o,2);break;
    case 2:
        if(difficulty()<4){c.snapshot();if(m.repeated(0x10)||m.repeated(0x40))c.move(-1);if(m.repeated(0x20)||m.repeated(0x80))c.move(1);if(c.changed()){m.sound(10);deactivate();signal(c.current+7);}}
        if(m.pressed(0x106)){set_phase(o,4);m.sound(9);erase(index);}
        else if(m.pressed(0x80001)){signal(6);e.child_signal(o,index,difficulty()<4?c.current+48:52,2,false);set_phase(o,3);m.sound(7);}
        break;
    case 3:if(o.age.current<14)return 1;erase(34);finish();break;
    case 4:
        if(o.age.current>=6){
            erase(34);
            if(s.mode!=0){m.last_difficulty=c.current;s.player_table.field_1e0=m.last_difficulty;set_state(o,1);m.interrupt_handle(o.handle390,1);o.handle390=0;set_state(o,1);}
            else if(difficulty()<4){set_state(o,1);s.player_table.field_1e0=c.current;m.last_difficulty=c.current;}
            else{s.player_table.field_1e0=m.last_difficulty;set_state(o,1);m.interrupt_handle(o.handle390,1);o.handle390=0;set_state(o,1);}
            pause::restore_cursor(c);
        }
        break;
    }
    return 1;
}
}
