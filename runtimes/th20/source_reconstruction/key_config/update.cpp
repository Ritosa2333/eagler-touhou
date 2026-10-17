#include "key_config.hpp"
#include "data.hpp"
#include "../pause_system/menu_support.hpp"
#include <cstring>
namespace th20::source::key_config {
namespace {
input::Device& selected_device(const KeyConfigInf& o,input::Controller& input){return input.devices[input.selected[o.selected_slot]];} //4c5e20 reads raw selected[slot]
int group(int kind){return kind==1?0:kind==2?1:2;}
std::uint16_t* keys(platform::KeyBindings& b,int g){return g==0?b.pad:g==1?b.alternate_pad:b.keyboard;}
const std::uint16_t* keys(const platform::KeyBindings& b,int g){return g==0?b.pad:g==1?b.alternate_pad:b.keyboard;}
void move_cursor(KeyConfigInf& o,Environment& e,int slot){o.cursor.snapshot();if(e.repeated(slot,0x10))o.cursor.move(-1);if(e.repeated(slot,0x20))o.cursor.move(1);if(o.cursor.changed()){e.play_effect(10);recovered::timer_set(o.selection_age,8);}}
}
void set_state(KeyConfigInf& o,int value){o.state=value;o.phase=0;recovered::timer_set(o.age,0);}
void set_phase(KeyConfigInf& o,int value){o.phase=value;recovered::timer_set(o.age,0);}
void assign_button(KeyConfigInf& o,Environment& e,int action,int button){
    const int kind=selected_device(o,e.input).kind;auto* selected=o.bindings[group(kind)];
    if(selected[action]==button)return;
    for(int i=0;i<(kind==0?8:4);++i)if(i!=action&&selected[i]==button)selected[i]=selected[action];
    selected[action]=static_cast<std::int16_t>(button);e.play_effect(7);recovered::timer_set(o.selection_age,8);
}
int update_devices(KeyConfigInf& o,Environment& e){
    switch(o.phase){
    case 0:o.cursor.count=3;o.cursor.select(o.selected_slot);o.selected_devices[0]=e.input.selected_device(0);o.selected_devices[1]=e.input.selected_device(1);recovered::timer_set(o.transition_age,0);set_phase(o,1);[[fallthrough]];
    case 1:if(o.age.current>6){set_phase(o,2);if(o.refresh_devices){e.rebuild_devices();o.refresh_devices=0;}}break;
    case 2:
        move_cursor(o,e,2);
        switch(o.cursor.current){
        case 0:
            if(e.pressed(2,0x40)){o.selected_devices[0]=recovered::signed_bits(static_cast<unsigned>(o.selected_devices[0])-1u);if(o.selected_devices[0]<0)o.selected_devices[0]=e.input.device_count-1;e.play_effect(10);recovered::timer_set(o.selection_age,8);}
            if(e.pressed(2,0x80)){o.selected_devices[0]=recovered::signed_bits(static_cast<unsigned>(o.selected_devices[0])+1u);if(o.selected_devices[0]>=e.input.device_count)o.selected_devices[0]=0;e.play_effect(10);recovered::timer_set(o.selection_age,8);}
            e.input.selected[0]=o.selected_devices[0];if(!e.pressed(2,0x8010f))return 1;
            set_phase(o,3);recovered::timer_set(o.transition_age,0);e.play_effect(7);goto transition;
        case 1:
            if(!e.pressed(2,0x8010f))return 1;e.play_effect(7);e.input.mappings[0]=e.defaults;e.input.mappings[1]=e.defaults;e.configuration.bindings[0]=e.defaults;e.configuration.bindings[1]=e.defaults;e.rebuild_devices();return 1;
        case 2:if(!e.pressed(2,0x8010f))return 1;set_phase(o,4);e.play_effect(7);break;
        default:return 1;
        }
        [[fallthrough]];
    case 4:e.configuration.value_68=static_cast<unsigned>(e.input.selected_device(0));e.configuration.value_6c=static_cast<unsigned>(e.input.selected_device(1));set_state(o,3);break;
    case 3:transition:
        recovered::timer_tick(o.transition_age,e.timer_rate);
        if(o.age.current>=10){o.selected_slot=o.cursor.current;pause::save_cursor(o.cursor);set_state(o,2);}break;
    }
    return 1;
}
int update_bindings(KeyConfigInf& o,Environment& e){
    auto& device=selected_device(o,e.input);const auto kind=device.kind;const auto g=group(kind);
    auto* selected=o.bindings[g];auto* target=keys(e.input.mappings[o.selected_slot],g);const auto* defaults=keys(e.defaults,g);
    switch(o.phase){
    case 0:
        o.cursor.count=kind==0?9:6;o.cursor.select(0);recovered::timer_set(o.transition_age,0);set_phase(o,1);
        for(int k=0;k<3;++k)std::memcpy(o.bindings[k],keys(e.input.mappings[o.selected_slot],k),16);set_phase(o,1);[[fallthrough]];
    case 1:if(o.age.current>6)set_phase(o,2);break;
    case 2:{
        bool any=false;
        if(kind!=0){
            move_cursor(o,e,o.selected_slot);for(int i=0;i<31;++i)if(device.raw[i]&0x80)any=true;
            if(o.cursor.current==4){if(!any||o.age.current<20)return 1;std::memcpy(selected,defaults,16);e.play_effect(7);recovered::timer_set(o.age,0);return 1;}
            if(o.cursor.current!=5){for(int i=0;i<31;++i)if(device.raw[i]&0x80){assign_button(o,e,o.cursor.current,i);break;}return 1;}
        }else {
            move_cursor(o,e,3);for(auto code:data::allowed_keys)if(device.raw[code]&0x80)any=true;
            if(o.cursor.current!=8){
                if(o.age.current<15)return 1;
                for(auto code:data::allowed_keys)if(device.raw[code]&0x80){assign_button(o,e,o.cursor.current,code);recovered::timer_set(o.age,0);o.cursor.move(1);e.play_effect(7);recovered::timer_set(o.selection_age,8);break;}return 1;
            }
            if(e.repeated(3,0x80001))any=true;if(o.age.current<15)return 1;
        }
        if(any){std::memcpy(target,selected,16);e.configuration.bindings[o.selected_slot]=e.input.mappings[o.selected_slot];e.play_effect(9);set_phase(o,3);}break;
    }
    case 3:recovered::timer_tick(o.transition_age,e.timer_rate);if(o.age.current>=10){pause::restore_cursor(o.cursor);set_state(o,1);}break;
    case 4:pause::restore_cursor(o.cursor);set_state(o,1);break;
    }
    return 1;
}
int update(KeyConfigInf& o,Environment& e){
    switch(o.state){case 0:set_state(o,1);break;case 1:update_devices(o,e);break;case 2:update_bindings(o,e);break;case 3:if(o.age.current>=10){e.retire(o);return 1;}break;}
    recovered::timer_tick(o.age,e.timer_rate);if(o.selection_age.current>0)recovered::timer_add(o.selection_age,-1.f,e.timer_rate);return 1;
}
}
