#include "replay_menu.hpp"
#include "../pause_system/menu_support.hpp"
#include "../gameplay/player_state.hpp"
#include "../runtime_state/state.hpp"
#include <algorithm>
#include <stdexcept>
namespace th20::source::title {
int last_replay=0;
int update_replay_menu(TitleInf& o,ReplayMenuEnvironment& e){
    auto& selection=e.selection;auto& m=selection.main;auto& s=m.session;auto& c=o.cursor;auto& pages=o.cursor108;
    auto selected=[&]() -> replay::ReplayInf* {const auto index=o.words5734[1];if(index>=100)throw std::out_of_range("Replay metadata slot");return static_cast<replay::ReplayInf*>(o.metadata[index]);};
    auto confirm_stage=[&]{o.words5734[2]=static_cast<unsigned>(c.current);set_phase(o,3);o.ui_flags|=4u;m.sound(50);e.fade(0.05f);};
    switch(o.phase){
    case 0:
        c.count=25;c.select(e.last_replay%25);pages.count=3;pages.select(e.last_replay/25);pages.wrapping=1;e.last_replay=0;
        if(o.handle390==0)selection.spawn_text_overlay(o);m.spawn(o,38);set_phase(o,1);std::fill(std::begin(o.metadata),std::end(o.metadata),nullptr);o.words5734[0]=0;o.ui_flags&=~12u;e.begin_read(o);
        if(!m.exists(o.handles[0])){m.spawn(o,0);m.interrupt(o,0,3,false);}
        [[fallthrough]];
    case 1:if(o.age.current>6)set_phase(o,2);break;
    case 2:{
        c.snapshot();pages.snapshot();if(m.repeated(0x10))c.move(-1);if(m.repeated(0x20))c.move(1);if(m.repeated(0x40))pages.move(-1);if(m.repeated(0x80))pages.move(1);
        if(pages.changed())m.sound(10);if(c.changed())m.sound(10);
        if(m.pressed(0x106)){set_phase(o,5);m.sound(9);o.ui_flags|=4u;return 1;}
        if(!m.pressed(0x80001))return 1;
        const int index=c.current+pages.current*25;if(index<0||index>=100)throw std::out_of_range("Replay metadata slot");if(!o.metadata[index])return 1;
        set_phase(o,4);o.words5734[1]=static_cast<unsigned>(c.current+pages.current*25);pause::save_cursor(c);m.sound(7);c.count=7;c.select(0);
        for(int i=0;i<7;++i)if(!selected()->playback[i+1].stage)pause::exclude(c,i);c.move(-1);c.move(1);
        if((selected()->user->flags&2u)!=0)confirm_stage();break;
    }
    case 3:
        if(!e.effects_ready()){recovered::timer_add(o.age,-1.f,state::timer_rate);break;}
        if(o.age.current==2)e.loading_transition();
        if(o.age.current>=32&&(o.ui_flags&8u)){
            set_state(o,2);auto& replay=*selected();e.request_start(recovered::signed_bits(o.words5734[2]+1u),replay.filename);auto& user=*replay.user;
            set_character(s,static_cast<int>(user.fields_d0[2]));auto& player=*s.contexts[0].current_player;player.fields_00[3]=user.stones[0];player.fields_00[5]=user.stones[1];player.fields_00[4]=user.stones[2];player.fields_00[6]=user.stones[3];s.player_table.field_1e0=static_cast<unsigned>(user.difficulty);
            set_mode(s,(user.flags&2u)?2:0);gameplay::player_state::write(s.player_table,0x204,(user.flags&2u)?std::clamp(user.spell,-1,9999):-1);
            selection.menu_selection=2;e.last_replay=static_cast<int>(o.words5734[1]);
        }
        break;
    case 4:
        if(o.age.current<15)return 1;c.snapshot();if(m.repeated(0x10))c.move(-1);if(m.repeated(0x20))c.move(1);if(c.changed())m.sound(10);
        if(m.pressed(0x106)){pause::restore_cursor(c);c.count=25;c.excluded.clear();set_phase(o,2);m.sound(9);return 1;}
        if(m.pressed(0x80001))confirm_stage();break;
    case 5:
        if(o.age.current>=6&&(o.ui_flags&8u)){for(auto* value:o.metadata)e.retire(value);std::fill(std::begin(o.metadata),std::end(o.metadata),nullptr);m.interrupt(o,38,1,true);m.interrupt_handle(o.handle390,1);o.handle390=0;set_state(o,1);pause::restore_cursor(c);}
        break;
    }
    return 1;
}
}
