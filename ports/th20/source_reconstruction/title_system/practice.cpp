#include "practice.hpp"
#include "practice_data.hpp"
#include "stage_select_data.hpp"
#include "../pause_system/menu_support.hpp"
#include "../runtime_state/state.hpp"
#include <stdexcept>
namespace th20::source::title {
int practice_resume_stage=practice_data::initial_resume[0],practice_resume_difficulty=practice_data::initial_resume[1],practice_resume_boss=practice_data::initial_resume[2];
namespace {
void open_boss(TitleInf& o,PracticeEnvironment& e){e.selection.main.interrupt(o,44,1,true);set_state(o,19);o.words58d8[0]=unsigned(o.cursor.current);pause::save_cursor(o.cursor);o.cursor.select(0);}
void open_difficulty(TitleInf& o,PracticeEnvironment& e){set_state(o,20);o.words58d8[1]=unsigned(o.cursor.current);pause::save_cursor(o.cursor);e.selection.main.interrupt(o,37,1,true);o.cursor.select(0);}
int selected_event(const menu::Cursor& c){return static_cast<short>(unsigned(c.current)+7u);}
}
int update_practice_stage(TitleInf& o,PracticeEnvironment& e){auto& m=e.selection.main;auto& c=o.cursor;
    switch(o.phase){
    case 0:
        if(!o.handle390)e.selection.spawn_text_overlay(o);c.count=7;c.excluded.clear();e.selection.signal(o,12,47,true);if(!m.exists(o.handles[85]))m.spawn(o,85);set_phase(o,1);
        if(e.stage.menu_selection==5){c.select(e.resume_stage);e.resume_stage=-1;m.interrupt(o,85,3,false);e.selection.signal(o,85,selected_event(c),true);e.selection.signal(o,85,6,true);m.interrupt(o,136,3,false);open_boss(o,e);break;}
        m.spawn(o,44);[[fallthrough]];
    case 1:if(o.age.current>10){set_phase(o,2);m.interrupt(o,85,3,false);e.selection.signal(o,85,selected_event(c),false);m.interrupt(o,136,3,false);}break;
    case 2:
        c.snapshot();if(m.repeated(0x10))c.move(-1);if(m.repeated(0x20))c.move(1);if(c.changed()){m.sound(10);m.interrupt(o,85,3,false);e.selection.signal(o,85,selected_event(c),false);}
        if(m.pressed(0x106)){set_phase(o,4);m.sound(9);e.stage.last_stage=c.current;}
        else if(m.pressed(0x80001)){e.selection.signal(o,85,6,false);set_phase(o,3);m.sound(7);e.stage.last_stage=c.current;}break;
    case 3:if(o.age.current<20)return 1;open_boss(o,e);break;
    case 4:if(o.age.current>=6){m.interrupt(o,44,1,true);m.interrupt(o,85,1,true);e.selection.signal(o,12,48,true);set_state(o,7);pause::restore_cursor(c);}break;
    }return 1;
}
int update_practice_boss(TitleInf& o,PracticeEnvironment& e){auto& m=e.selection.main;auto& c=o.cursor;const int stage=recovered::signed_bits(o.words58d8[0]);if(stage<0||stage>=7)throw std::out_of_range("Spell practice stage");
    switch(o.phase){
    case 0:
        c.count=practice_data::boss_counts[stage];c.excluded.clear();if(!m.exists(o.handles[37]))m.spawn(o,37);if(!m.exists(o.handles[86]))m.spawn(o,86);set_phase(o,1);
        for(int i=0;i<13-practice_data::boss_counts[stage];++i)e.selection.signal(o,86,static_cast<short>(39-i),true);
        if(e.stage.menu_selection==5){c.select(e.resume_boss);e.resume_boss=-1;m.interrupt(o,86,3,false);e.selection.signal(o,86,selected_event(c),true);e.selection.signal(o,86,6,true);open_difficulty(o,e);break;}
        [[fallthrough]];
    case 1:if(o.age.current>10){set_phase(o,2);m.interrupt(o,86,3,false);e.selection.signal(o,86,selected_event(c),false);}break;
    case 2:
        c.snapshot();if(m.repeated(0x10))c.move(-1);if(m.repeated(0x20))c.move(1);if(c.changed()){m.sound(10);m.interrupt(o,86,3,false);e.selection.signal(o,86,selected_event(c),false);}
        if(m.pressed(0x106)){set_phase(o,4);m.sound(9);}else if(m.pressed(0x80001)){if(!e.group_available(stage,c.current))m.sound(16);else{e.selection.signal(o,86,6,false);set_phase(o,3);m.sound(7);}}break;
    case 3:if(o.age.current>=14)open_difficulty(o,e);break;
    case 4:if(o.age.current>=6){for(int i=0;i<5;++i)e.card_interrupt(o,i,1);for(int i=127;i<134;++i)e.delete_animation(o,i);for(int i:{37,86,85})m.interrupt(o,i,1,true);set_state(o,18);pause::restore_cursor(c);return 1;}break;
    }
    e.refresh_cards(o,stage,c.current,-1);return 1;
}
int update_practice_difficulty(TitleInf& o,PracticeEnvironment& e){auto& m=e.selection.main;auto& c=o.cursor;
    switch(o.phase){
    case 0:c.count=5;c.select(0);if(e.resume_difficulty>=0){c.select(e.resume_difficulty);e.resume_difficulty=-1;}e.stage.menu_selection=1;e.select_card(o,c.current);set_phase(o,1);
        [[fallthrough]];
    case 1:if(o.age.current>10)set_phase(o,2);break;
    case 2:
        c.snapshot();if(m.repeated(0x10))c.move(-1);if(m.repeated(0x20))c.move(1);if(c.changed()){m.sound(10);e.select_card(o,c.current);}
        if(m.pressed(0x106)){set_phase(o,4);m.sound(9);}else if(m.pressed(0x80001)){
            if(c.current<0||c.current>=5)throw std::out_of_range("Spell practice difficulty");if(!e.card_playable(recovered::signed_bits(o.words58d8[3+c.current]))){m.sound(16);break;}
            e.card_interrupt(o,c.current,6);set_phase(o,3);m.sound(7);e.stage.fade(stage_select_data::f_0056fe5c);m.sound(50);
        }break;
    case 3:
        if(!e.stage.effects_ready()){recovered::timer_add(o.age,-1.f,state::timer_rate);return 1;}
        if(o.age.current==10)e.stage.loading();if(o.age.current>=40){pause::save_cursor(c);set_state(o,2);e.launch(recovered::signed_bits(o.words58d8[0])+1,recovered::signed_bits(o.words58d8[3+c.current]));e.stage.menu_selection=5;e.resume_stage=recovered::signed_bits(o.words58d8[0]);e.resume_boss=recovered::signed_bits(o.words58d8[1]);e.resume_difficulty=c.current;}break;
    case 4:if(o.age.current>=6){m.interrupt(o,86,1,true);set_state(o,19);c.excluded.clear();pause::restore_cursor(c);return 1;}break;
    }
    e.refresh_cards(o,recovered::signed_bits(o.words58d8[0]),recovered::signed_bits(o.words58d8[1]),c.current);for(int i=0;i<5;++i)if(recovered::signed_bits(o.words58d8[3+i])<0)pause::exclude(c,i);return 1;
}
}
