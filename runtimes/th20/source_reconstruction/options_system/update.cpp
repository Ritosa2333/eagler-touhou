#include "options.hpp"
namespace th20::source::options {
void set_state(OptionInf& o,int state){o.state=state;recovered::timer_set(o.age,0);}
namespace {
void scaled_window(int& mode,std::uint8_t scale){switch(scale){case 0:mode=3;break;case 1:mode=4;break;case 2:case 5:mode=5;break;case 3:mode=6;break;case 4:mode=7;break;}}
void switch_display(OptionInf& o,Environment& e,bool next){
    e.screen_change_effect();o.state=4;recovered::timer_set(o.age,0);
    auto& mode=e.display_mode;const auto scale=e.configuration.scale_choice;
    if(next)switch(mode){case 0:mode=3;break;case 1:mode=4;break;case 2:scaled_window(mode,scale);break;case 3:case 4:case 5:case 6:case 7:mode=8;break;case 8:mode=9;break;case 9:switch(scale){case 0:mode=0;break;case 1:mode=1;break;case 2:case 3:case 4:case 5:mode=2;break;}break;}
    else switch(mode){case 0:case 1:case 2:mode=9;break;case 3:mode=0;break;case 4:mode=1;break;case 5:case 6:case 7:mode=2;break;case 8:scaled_window(mode,scale);break;case 9:mode=8;break;}
}
void change_volume(std::uint8_t& value,bool next){
    if(next){value=static_cast<std::uint8_t>(value+5u);if(static_cast<std::int8_t>(value)>100)value=100;}
    else if(static_cast<std::int8_t>(value)<5)value=0;else value=static_cast<std::uint8_t>(value-5u);
}
}
int update(OptionInf& o,Environment& e){
    if(e.key_config_active())return 1;
    switch(o.state){
    case 0:o.cursor.count=6;o.cursor.select(0);set_state(o,1);[[fallthrough]];
    case 1:if(o.age.current>6)set_state(o,2);break;
    case 2:{
        o.cursor.snapshot();if(e.repeated(0x10))o.cursor.move(-1);if(e.repeated(0x20))o.cursor.move(1);
        if(o.cursor.changed()){e.play_effect(10);recovered::timer_set(o.selection_age,8);}
        if(o.cursor.current==2&&o.age.current!=o.age.previous&&o.age.current%60==0)e.play_effect(2);
        if(e.repeated(0x40))switch(o.cursor.current){case 0:switch_display(o,e,false);break;case 1:change_volume(e.configuration.value_7e,false);e.apply_volume();break;case 2:change_volume(e.configuration.value_7f,false);e.apply_volume();break;}
        if(e.repeated(0x80))switch(o.cursor.current){case 0:switch_display(o,e,true);break;case 1:change_volume(e.configuration.value_7e,true);e.apply_volume();break;case 2:change_volume(e.configuration.value_7f,true);e.apply_volume();break;}
        bool close=false;
        if(e.pressed(0x106)){if(o.cursor.current==5)close=true;else o.cursor.select(5);}
        if(!close&&o.allow_escape!=0&&e.pressed(0x100))close=true;
        if(!close&&e.pressed(0x80001))switch(o.cursor.current){
        case 3:recovered::timer_set(o.age,0);o.state=3;recovered::timer_set(o.key_config_age,30);e.play_effect(7);break;
        case 4:e.configuration.value_7e=100;e.configuration.value_7f=80;e.play_effect(7);break;
        case 5:close=true;break;
        }
        if(close){e.play_effect(9);set_state(o,5);}break;
    }
    case 3:if(o.age.current>30&&o.cursor.current==3){e.create_key_config(o.position);o.state=0;}break;
    case 4:if(o.age.current>3){e.reset_device();o.state=2;recovered::timer_set(o.age,0);}break;
    case 5:if(o.age.current>=10){e.save_configuration();e.retire(o);return 1;}break;
    }
    recovered::timer_tick(o.age,e.timer_rate);
    if(o.selection_age.current>0)recovered::timer_add(o.selection_age,-1.f,e.timer_rate);
    if(o.key_config_age.current>0)recovered::timer_add(o.key_config_age,-1.f,e.timer_rate);
    return 1;
}
}
