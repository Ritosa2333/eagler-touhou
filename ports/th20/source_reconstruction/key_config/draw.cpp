#include "key_config.hpp"
#include "data.hpp"
#include "../text_renderer/menu_style.hpp"
#include <cstdio>
namespace th20::source::key_config {
const char* button_name(int kind,int button){
    std::lock_guard lock(runtime::shared_locks().slot(15));
    // The original compares signed bounds without a lower guard; valid stored bindings are nonnegative here.
    if(kind==2&&button<12)return data::xinput_names[button];
    if((kind==0||kind==2)&&button<256)return data::key_names[button];
    static char buffer[16]{};sprintf_s(buffer,10,"Button %d",button);return buffer;
}
namespace {
void align(text::Renderer& r,unsigned x,unsigned y){r.fields_1a1d4[8]=x;r.fields_1a1d4[9]=y;}
void description_state(text::Renderer& r){r.fields_1a1d4[3]=0;r.color=0xffffffff;r.fields_1a1d4[4]=3;r.color=0xffc0c080;r.field_1a1c8=0xffffffff;r.shadow_color=0xff000000;align(r,0,0);}
void finish(text::Renderer& r){r.color=0xffffffff;r.scale_x=r.scale_y=1;align(r,1,1);}
}
void draw_devices(KeyConfigInf& o,text::Renderer& r,input::Controller& input){
    namespace d=data;r.fields_1a1d4[3]=7;auto position=o.position;align(r,0,1);r.color=0xffffffff;r.shadow_color=0xff000000;position.x+=d::f_00571058;r.write_ascii_format(position,"Controller Select");
    position.y+=d::f_0056fc04;auto saved=position;text::style_menu_line(r,position,0,o.cursor,o.transition_age.current,o.selection_age.current);
    const auto selected=input.selected_device(0);if(selected<1)r.write_ascii_format(position,"<%s> Setting",d::device_labels[selected]);else r.write_ascii_format(position,"<%s%d> Setting",d::device_labels[1],selected);
    saved.y+=d::f_0056fd48;position=saved;r.color=0xffffffff;saved.y+=d::f_0056fd48;position=saved;r.color=0xffffffff;
    text::style_menu_line(r,position,1,o.cursor,o.transition_age.current,o.selection_age.current);r.write_ascii_format(position,"KeyConfig ALL Reset");
    saved.y+=d::f_0056fd48;position=saved;r.color=0xffffffff;text::style_menu_line(r,position,2,o.cursor,o.transition_age.current,o.selection_age.current);r.write_ascii_format(position,"Quit");
    saved.y+=d::f_0056fa28;position=saved;r.color=0xffa0a0b0;r.shadow_color=0xff000000;r.fields_1a1d4[3]=0;r.color=0xffffffff;align(r,1,1);description_state(r);
    const auto x=o.position.x+d::f_00571058;
    switch(o.cursor.current){case 0:r.write_text_literal({x,o.position.y+d::f_0056f10c,0},d::s_00570ebc);break;case 1:r.write_text_literal({x,o.position.y+d::f_0056ec9c,0},d::s_00570edc);break;case 2:r.write_text_literal({x,o.position.y+d::f_0056ec9c,0},d::s_00570efc);break;}
    finish(r);
}
void draw_bindings(KeyConfigInf& o,text::Renderer& r,input::Controller& input){
    namespace d=data;const int kind=input.devices[input.selected[o.selected_slot]].kind;const auto* bindings=o.bindings[kind==1?0:kind==2?1:2];
    r.fields_1a1d4[3]=7;auto position=o.position;align(r,0,1);r.color=0xffffffff;r.shadow_color=0xff000000;position.x+=d::f_00571058;r.write_ascii_format(position,"Key Config");
    position.y+=d::f_0056fd48;r.color=0xff808080;r.shadow_color=0xff000000;r.write_ascii_format(position,kind==1?"DirectInput":kind==2?"XInput":"KeyBoard");
    r.color=0xffffffff;position.x-=d::f_00571054;position.y=position.y+d::f_0056fd48+d::f_00571050;align(r,1,1);
    const int count=kind==0?9:6;
    for(int index=0;index<count;++index){
        const auto saved=position;text::style_menu_line(r,position,index,o.cursor,0,o.selection_age.current);r.write_ascii_format(position,kind==0?d::key_labels[index]:d::pad_labels[index]);
        if(index<count-(kind==0?1:2)){position.x+=d::f_0057105c;align(r,0,1);r.write_ascii_format(position,button_name(kind==0?0:kind==1?1:2,bindings[index]));align(r,1,1);}
        position=saved;position.y+=d::f_0056fd48;
    }
    description_state(r);const auto x=o.position.x+d::f_00571058;
    if(kind==1||kind==2)r.write_text_literal({x,o.position.y+d::f_0056f10c,0},d::pad_description[o.cursor.current]);
    else {r.write_text_literal({x,o.position.y+d::f_0056fe88,0},d::s_00571030);r.write_text_literal({x,o.position.y+d::f_00571060,0},d::key_description[o.cursor.current]);}
    finish(r);
}
int draw(KeyConfigInf& o,text::Renderer& r,input::Controller& input){if(o.state==1)draw_devices(o,r,input);else if(o.state==2)draw_bindings(o,r,input);r.fields_1a1d4[3]=0;r.color=0xffffffff;return 1;}
}
