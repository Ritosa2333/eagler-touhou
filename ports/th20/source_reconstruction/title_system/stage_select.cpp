#include "stage_select.hpp"
#include "stage_select_data.hpp"
#include "../pause_system/menu_support.hpp"
#include "../runtime_state/state.hpp"
#include "../gameplay/player_state.hpp"
#include <stdexcept>
namespace th20::source::title {
int last_stage=stage_select_data::initial_last_stage;

int update_stage(TitleInf& o,StageEnvironment& e){
    auto& c=o.cursor;
    switch(o.phase){
    case 0:c.count=6;c.select(e.last_stage);e.spawn_heading(o);set_phase(o,1);if(e.menu_selection==4)e.menu_selection=1;
        [[fallthrough]];
    case 1:if(o.age.current>10)set_phase(o,2);break;
    case 2:
        c.snapshot();if(e.repeated(0x10))c.move(-1);if(e.repeated(0x20))c.move(1);if(c.changed())e.sound(10);
        if(e.pressed(0x106)){set_phase(o,4);e.sound(9);e.last_stage=c.current;}
        else if(e.pressed(0x80001)){
            if(!stage_available(e.profile(),gameplay::player_state::difficulty(e.session.player_table),c.current))e.sound(16);
            else{set_phase(o,3);e.sound(7);e.sound(50);e.last_stage=c.current;e.session.fields_74[0]=0;e.session.fields_74[0]=unsigned(e.keyboard_number());e.fade(stage_select_data::f_0056fe5c);}
        }break;
    case 3:
        if(!e.effects_ready())recovered::timer_add(o.age,-1.f,state::timer_rate);
        else{if(o.age.current==10)e.loading();if(o.age.current>=40){pause::save_cursor(c);set_state(o,2);e.request_start(c.current+1);e.menu_selection=4;e.last_stage=c.current;}}
        break;
    case 4:if(o.age.current>=6){e.retire_heading(o);set_state(o,7);pause::restore_cursor(c);}break;
    }
    return 1;
}
}
