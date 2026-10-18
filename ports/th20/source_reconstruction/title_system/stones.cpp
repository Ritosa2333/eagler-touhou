#include "stones.hpp"
#include "stones_data.hpp"
#include "../pause_system/menu_support.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::title {
int last_stone_record=0;StonesUnlockState stones_unlock_state;
void advance_stones_unlock(StonesUnlockState& s,StonesEnvironment& e){
    s.previous=s.current;int result=e.keyboard(s.current);
    if(result==2){
        s.pressed.fill(0);constexpr unsigned scan[]{0x1e,0x30,0x2e,0x20,0x12,0x21,0x22,0x23,0x17,0x24,0x25,0x26,0x32,0x31,0x18,0x19,0x10,0x13,0x1f,0x14,0x16,0x2f,0x11,0x2d,0x15,0x2c};
        for(unsigned i=0;i<26;++i)s.pressed[scan[i]]=s.current[0x41+i];s.current=s.pressed;result=1;
    }
    if(result==1){
        for(unsigned i=0;i<256;++i)s.pressed[i]=(s.current[i]^s.previous[i])&s.current[i];
        if(s.matched<8){if(s.pressed[stones_data::sequence[s.matched]]&0x80){++s.matched;s.idle=0;}else{unsigned combined=0;for(unsigned i=0;i<0x39;++i)combined|=s.pressed[i];if(combined&0x80)s.matched=0;}}
        else{e.unlock_progress();e.main.sound(17);s.matched=0;}
    }
    ++s.idle;if(s.idle>300){s.matched=0;s.idle=0;}
}
int update_stones(TitleInf& o,StonesEnvironment& e){
    auto& c=o.cursor;auto& m=e.main;auto show_record=[&]{if(e.title(c.current)){o.words58d8[9]=0;o.words58d8[10]=unsigned(c.current);}};
    switch(o.phase){
    case 0:if(!o.handle390)e.background(o);set_phase(o,1);c.count=41;c.wrapping=1;c.select(e.last_record);e.heading(o,true);show_record();break;
    case 1:
        c.snapshot();if(m.repeated(0x10))c.move(-10);if(m.repeated(0x20))c.move(10);if(m.repeated(0x40)){if(c.current%10==0)c.move(9);else c.move(-1);}if(m.repeated(0x80)){if(c.current%10==9)c.move(-9);else c.move(1);}
        if(c.changed()){m.sound(10);show_record();}
        if(m.pressed(0x80001)){
            if(c.current<=17){if(e.achieved(unsigned(c.current))){e.ending(c.current);return 1;}}
            else if(c.current<=33&&e.achieved(unsigned(c.current))){m.sound(7);e.fade(stones_data::f_0056e0ec);set_phase(o,2);break;}
        }
        if(m.pressed(0x106)){set_phase(o,3);m.sound(9);e.heading(o,false);}
        if(c.current==13){if(m.pressed(0x8010f)){e.unlock.matched=0;e.unlock.idle=0;}advance_stones_unlock(e.unlock,e);}break;
    case 2:
        if(!e.effects_ready()){recovered::timer_add(o.age,-1.f,state::timer_rate);return 1;}
        if(o.age.current==10)e.loading();if(o.age.current>=40)e.prepare_replay(o);break;
    case 3:if(o.age.current>=20){e.last_record=c.current;set_state(o,10);pause::restore_cursor(c);}break;
    }
    ++o.words58d8[9];return 1;
}
}
