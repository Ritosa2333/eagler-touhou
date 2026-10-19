#include "title.hpp"
#include "../pause_system/menu_support.hpp"
namespace th20::source::title {
int update_main_menu(TitleInf& o,MainEnvironment& e){
    auto& c=o.cursor;
    switch(o.phase){
    case 0:
        e.interrupt_handle(o.handle390,1);o.handle390=0;c.count=10;c.wrapping=1;
        if(!e.extra_unlocked())pause::exclude(c,1);
        if(e.session.mode==2){c.select(3);set_mode(e.session,0);}else if(e.session.mode!=0){c.select(2);set_mode(e.session,0);}
        set_phase(o,1);
        if(!(o.ui_flags&2)){
            if(!e.exists(o.handles[0])){e.spawn(o,0);e.interrupt(o,0,2,false);}
            if(!e.exists(o.handles[31])){e.spawn(o,31);e.interrupt(o,31,2,false);}
            if(o.previous_state!=3)e.interrupt(o,0,2,false);
            recovered::timer_set(o.age,120);
        }else{e.spawn(o,0);e.spawn(o,31);o.ui_flags&=~2u;}
        [[fallthrough]];
    case 1:
        if(o.age.current==120&&!e.decoration_exists(o))e.spawn_decoration(o);
        if(o.age.current>130)set_phase(o,2);
        break;
    case 2:
        if(!e.notice_present()){if(e.notice_pending()){e.create_notice();return 1;}}
        else{if(!e.notice_finished())return 1;e.retire_notice();}
        c.snapshot();if(e.repeated(0x10))c.move(-1);if(e.repeated(0x20))c.move(1);
        if(c.changed()){e.sound(10);recovered::timer_set(o.selection_age,8);}
        if(e.pressed(0x106)){
            if(c.current==9){e.sound(9);set_phase(o,4);break;}
            e.sound(9);c.select(9);
        }
        if(!e.pressed(0x80001))return 1;
        recovered::timer_set(o.flash_age,30);
        if(c.current>=0&&c.current<=8){e.sound(7);e.interrupt(o,31,1,true);e.interrupt_handle(o.handle474,1);set_phase(o,4);}
        else if(c.current==9){e.sound(9);set_phase(o,4);}
        break;
    case 4:
        if(o.age.current<20)break;
        switch(c.current){
        case 0:set_mode(e.session,0);e.interrupt(o,0,3,false);set_state(o,5);pause::save_cursor(c);c.select(e.last_difficulty);e.session.player_table.field_1e0=e.last_difficulty;break;
        case 1:set_mode(e.session,0);e.interrupt(o,0,3,false);set_state(o,5);pause::save_cursor(c);e.session.player_table.field_1e0=4;c.select(0);break;
        case 2:set_mode(e.session,1);e.interrupt(o,0,3,false);pause::save_cursor(c);c.select(e.last_difficulty);e.session.player_table.field_1e0=e.last_difficulty;set_state(o,5);break;
        case 3:set_mode(e.session,2);e.interrupt(o,0,3,false);set_state(o,6);pause::save_cursor(c);c.select(e.last_character);set_character(e.session,e.last_character);break;
        case 4:e.interrupt(o,0,3,false);set_state(o,12);pause::save_cursor(c);break;
        case 5:e.interrupt(o,0,3,false);set_state(o,10);pause::save_cursor(c);c.select(0);break;
        case 6:e.interrupt(o,0,3,false);set_state(o,14);pause::save_cursor(c);break;
        case 7:e.interrupt(o,0,3,false);set_state(o,3);pause::save_cursor(c);break;
        case 8:e.interrupt(o,0,3,false);set_state(o,17);pause::save_cursor(c);break;
        case 9:set_state(o,2);break;
        }
        break;
    }
    return 1;
}
}
