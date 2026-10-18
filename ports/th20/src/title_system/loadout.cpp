#include "loadout.hpp"
#include "../pause_system/menu_support.hpp"
#include "../gameplay/player_state.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::title {
void update_loadout(TitleInf& o,LoadoutEnvironment& e){
    auto& selection=e.selection;auto& m=selection.main;auto& s=m.session;auto& c=o.cursor;
    const int character_index=s.player_table.field_1e0==4?14:12;
    auto character=[&]{return static_cast<int>(s.contexts[0].current_player->fields_00[2]);};
    auto profile=[&]{return static_cast<int>(s.contexts[0].current_player->fields_00[3]);};
    auto select_character_animation=[&](int value){selection.child_signal(o,character_index,character()+6,value/2+37,false);};
    auto erase=[&](int index){m.interrupt(o,index,1,true);};
    auto show_profile=[&](int value,int offset){selection.signal(o,13,static_cast<short>(value+offset),true);};
    auto start_game=[&]{
        if(o.age.current>=40){
            s.contexts[0].current_player->fields_00[3]=static_cast<unsigned>(c.current);selection.last_profile=profile();pause::save_cursor(c);
            gameplay::player_state::write(s.player_table,0x204,-1);set_state(o,2);e.request_start(static_cast<int>(s.player_table.field_1e0)<4?1:7);
        }
    };
    auto proceed=[&]{select_character_animation(profile());pause::save_cursor(c);erase(36);set_state(o,s.mode==2?18:8);start_game();};
    switch(o.phase){
    case 0:
        c.count=3;m.spawn(o,13);
        for(int i=0;i<9;++i)if(e.clear_count(static_cast<int>(s.player_table.field_1e0),character(),i)==0)selection.child_visible(o,13,i+22,false);
        o.previous5960=o.previous5964=-1;
        if(selection.menu_selection==4||selection.menu_selection==5){proceed();break;}
        e.open_stones();o.phase=1;
        [[fallthrough]];
    case 1:if(o.age.current>30){m.spawn(o,36);set_phase(o,2);}break;
    case 2:
        if(e.stone_display_state()!=o.previous5964){
            o.previous5964=e.stone_display_state();
            switch(e.stone_display_state()){case 0:m.interrupt(o,13,3,false);break;case 1:show_profile(o.previous5960,7);break;case 2:show_profile(o.previous5960,37);break;}
        }
        if(profile()!=o.previous5960){
            m.interrupt(o,13,3,false);
            if(e.stone_display_state()!=0&&o.previous5960>0)show_profile(o.previous5960,17);
            o.previous5960=profile();
            switch(e.stone_display_state()){case 1:show_profile(o.previous5960,7);break;case 2:show_profile(o.previous5960,37);break;}
            select_character_animation(o.previous5960);
        }
        if(e.stone_visibility()==3)set_phase(o,3);else if(e.stone_visibility()==0)set_phase(o,4);
        break;
    case 3:
        if(!e.effects_ready()){recovered::timer_add(o.age,-1.f,state::timer_rate);break;}
        if(o.age.current==10){if(s.mode!=0){proceed();break;}e.loading_transition();}
        start_game();break;
    case 4:
        if(o.age.current>=6){
            for(int i=0;i<9;++i)selection.child_visible(o,13,i+22,false);
            selection.last_profile=profile();erase(13);select_character_animation(profile());erase(36);set_state(o,6);pause::restore_cursor(c);selection.commit_progress();
        }
        break;
    }
}
}
